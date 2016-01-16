#!/bin/bash

mv app_bill app_bill_new
git checkout -b new
git pull origin tested:tested

git checkout tested

