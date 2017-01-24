#!/bin/bash

mv app_bill app_bill_new
mv tests/comparative tests/comparative_save

# Именуем ветку текущего теста, чтобы вернуться в неё из скрипта
git checkout -b new

# git pull origin tested:tested
# git checkout tested

git pull origin released:released
git checkout released

mv tests/comparative_save tests/comparative
