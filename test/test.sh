#!/bin/bash

program_path=../myCompiler
red="\033[38;2;255;0;0m"
green="\033[38;2;0;255;0m"
results=0

function test
{
    exec 0< input
    output=$($program_path "$1")
    right_output=$(cat "$2")

    if [ "$output" = "$right_output" ]
    then
        echo -e " <$green OK \e[0m>       $1"
    else
        results=1
        echo -e " <$red ERROR \e[0m>    $1"
    fi
}

echo "--------------------------------------------------------------------"
echo " Start program testing"
echo "--------------------------------------------------------------------"

if [ -e $program_path ]
then
    test test_program right_output
else
    results=1
    echo ""
    echo "    $program_path: No such file"
    echo ""
fi


echo "--------------------------------------------------------------------"

if [ $results -eq 1 ]
then
    echo -e " results:$red fail\e[0m"
else
    echo -e " results:$green success\e[0m"
fi

echo "--------------------------------------------------------------------"