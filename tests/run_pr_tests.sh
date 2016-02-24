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

# echo 'Creating databases...'
# . "$DIR/create_schema.sh"

# echo 'Filling databases with test fixtures...'
# . "$DIR/fill_data.sh"

# echo 'Creating daemons configs...'
# python "$DIR/make_configs.py"

# echo 'Starting tested app version...'
# . "$DIR/run.sh"

# echo 'Генерируем тестовые примеры для уже протестированной версии и ждём завершения синхронизации...'
# python "$DIR/sampler.py"

# echo 'Stopping tested app version...'
# . "$DIR/stop.sh"

# echo 'Running tests analyzis...'
python "$DIR/pr_test.py"

