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

echo 'Очищаем счетчики минималок и пакетов перед запуском демонов'
. "$DIR/reset_counters.sh"
[[ $? -ne 0 ]] && exit

echo 'Starting tested app version...'
. "$DIR/run.sh" waitsync
[[ $? -ne 0 ]] && exit

echo 'Генерируем тестовые примеры для уже протестированной версии и ждём завершения синхронизации...'
nice -19 python "$DIR/sampler.py"
[[ $? -ne 0 ]] && exit

echo 'Запускаем auth тесты для уже протестированной версии...'
nice -19 python "$DIR/comparative/auth_caller.py"
[[ $? -ne 0 ]] && exit

echo 'Запускаем call тесты для уже протестированной версии...'
nice -19 python "$DIR/comparative/call_caller.py"
[[ $? -ne 0 ]] && exit

echo 'Stopping tested app version...'
. "$DIR/stop.sh"

echo 'Switching back to the new source code version...'
. "$DIR/switch_repo_back.sh"
[[ $? -ne 0 ]] && exit

echo "Переключились обратно на новую (тестируемую) версию:"
git log --name-status HEAD^..HEAD

echo 'Migrating database structures and data...'
python "$DIR/migrate_db.py"
[[ $? -ne 0 ]] && exit

echo 'Migrating daemons configs...'
python "$DIR/migrate_configs.py"
[[ $? -ne 0 ]] && exit

echo 'Генерируем тестовые затравочные данные для новых таблиц...'
# Скрипт работает с тем же списком миграций, что и migrate_db.py, но ищет файлы с именем миграции
# и суффиксом _seed. Выполняет "миграции" для всех таких файлов.
python "$DIR/migrate_db.py" _seed
[[ $? -ne 0 ]] && exit

echo 'Очищаем счетчики минималок и пакетов перед запуском демонов'
. "$DIR/reset_counters.sh"
[[ $? -ne 0 ]] && exit

echo 'Starting new app version...'
. "$DIR/run.sh" 30
[[ $? -ne 0 ]] && exit

echo 'Wait 6min. Пока у демонов нитка Loader встанет в статус реади.'
sleep 360s

echo 'Генерируем тестовые примеры для новой (тестируемой) версии и ждём завершения синхронизации...'
nice -19 python "$DIR/sampler.py"
[[ $? -ne 0 ]] && exit

echo 'Запускаем auth тесты для новой (тестируемой) версии...'
nice -19 python "$DIR/comparative/auth_caller.py"
[[ $? -ne 0 ]] && exit

echo 'Запускаем call тесты для новой (тестируемой) версии...'
nice -19 python "$DIR/comparative/call_caller.py"
[[ $? -ne 0 ]] && exit

echo 'Running unit tests analysis...'
. "$DIR/unit/start.sh"

echo 'Running functional test analysis...'
. "$DIR/functional/start.sh"


echo 'Stopping new app version...'
. "$DIR/stop.sh"

echo 'Running comparative tests analyzis...'
. "$DIR/comparative/start.sh"

exit 0
# В случае успешного прохождения тестов, делаем коммит в tested - ветку, пригодную для деплоя.
# CURRENT_HASH=`git rev-parse HEAD`

# git checkout tested

# git reset --hard ${CURRENT_HASH}

# NOW=$(date +"%Y-%m-%d")
# git tag -a $NOW -m "Tested $NOW" tested

# git push --tags --force origin tested:tested

