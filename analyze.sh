#!/bin/bash

if [ $# -eq 1 ];
then
	grep -E "WARN|ERROR" $1 >> bug.txt
    # Your code here. (1/4)

else
    case $2 in
    "--latest")
    	tail --lines=5 $1
        # Your code here. (2/4)

    ;;
    "--find")
    	grep $3 $1 >> $3.txt
        # Your code here. (3/4)

    ;;
    "--diff")
    	diff $1 $3 > /dev/null
    	if [ $? -eq 0 ]
	then
		echo same
	elif [ $? -eq 1 ]
	then 
		echo different
	fi 
        # Your code here. (4/4)

    ;;
    esac
fi
