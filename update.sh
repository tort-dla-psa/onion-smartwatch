#!/bin/sh

git submodule foreach git pull origin master && rm build/*.flag && make all extra="-g"
