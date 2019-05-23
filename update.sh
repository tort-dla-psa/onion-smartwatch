#!/bin/sh

updates=$(git submodule foreach git pull origin master | grep "up to date" | wc -l);
directories=$(ls -l submodules | grep ^d | wc -l);
count=$(($directories-$updates));

echo  "$count submodules was updated";

if [ $count -ne 0 ]
then
 	rm build/*.flag && make all extra="-g";
fi
