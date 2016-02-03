#!/bin/bash

THIS=`readlink -f "${BASH_SOURCE[0]}"`
DIR=`dirname "${THIS}"`

eval `ssh-agent -s`
ssh-add ~/.ssh/billing_voip

echo "Запустились с новой (тестируемой) версией:"
git log --name-status HEAD^..HEAD

echo 'Сначала компилируем новую версию приложения, чтобы не запускать тесты при ошибках...'
. "$DIR/compile.sh"
[[ $? -ne 0 ]] && exit

echo 'Переключаем репозиторий на протестированную версию...'
. "$DIR/switch_repo_to_tested.sh"
[[ $? -ne 0 ]] && exit

echo "Переключились на протестированную версию:"
git log --name-status HEAD^..HEAD
[[ $? -ne 0 ]] && exit

echo 'Creating databases...'
. "$DIR/create_schema.sh"
[[ $? -ne 0 ]] && exit

echo 'Filling databases with test fixtures...'
. "$DIR/fill_data.sh"
[[ $? -ne 0 ]] && exit

echo 'Creating daemons configs...'
python "$DIR/make_configs.py"
[[ $? -ne 0 ]] && exit

echo 'Компилируем последнюю успешно протестированную версию...'
. "$DIR/compile.sh"
[[ $? -ne 0 ]] && exit

echo 'Starting tested app version...'
. "$DIR/run.sh"
[[ $? -ne 0 ]] && exit

echo 'Генерируем тестовые примеры для уже протестированной версии и ждём завершения синхронизации...'
nice -19 python "$DIR/sampler.py"
[[ $? -ne 0 ]] && exit

echo 'Stopping tested app version...'
. "$DIR/stop.sh"

echo 'Switching back to the new source code version...'
. "$DIR/switch_repo_back.sh"

echo "Переключились обратно на новую (тестируемую) версию:"
git log --name-status HEAD^..HEAD

echo 'Migrating database structures and data...'
. "$DIR/migrate_db.sh"

echo 'Migrating daemons configs...'
python "$DIR/migrate_configs.py"

echo 'Starting new app version...'
. "$DIR/run.sh"

echo 'Генерируем тестовые примеры для новой (тестируемой) версии и ждём завершения синхронизации...'
nice -19 python "$DIR/sampler.py"

echo 'Stopping new app version...'
. "$DIR/stop.sh"

echo 'Running tests analyzis...'
python "$DIR/comparative_test.py"
[[ $? -ne 0 ]] && exit

# В случае успешного прохождения тестов, делаем коммит в tested - ветку, пригодную для деплоя.
CURRENT_HASH=`git rev-parse HEAD`

git checkout tested

git reset --hard ${CURRENT_HASH}

NOW=$(date +"%Y-%m-%d")
git tag -a $NOW -m "Tested $NOW" tested

git push --tags --force origin tested:tested

