#!/bin/bash

mv app_bill app_bill_new

# Именуем ветку текущего теста, чтобы вернуться в неё из скрипта
git checkout -b new

# git pull origin tested:tested
# git checkout tested

git pull origin released:released
git checkout released

