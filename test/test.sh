#/bin/bash

# This script is just created to automate testing. It is not written very well,
# because I have little experience in writing bash scripts. I will try to improve
# it in the future.

program_path=../mcc
red="\033[38;2;255;0;0m"
green="\033[38;2;0;255;0m"
results=0
option=""

g_start=$(date +%s%N)

function test
{
    cd $1

    local start=$(date +%s%N)

    if [ -e ./input ]
    then
        exec 0< ./input
    fi

    local output=$(../../$program_path $option ./program)
    local right_output=$(cat ./output)

    if [ "$output" = "" ]
    then
        gcc -no-pie -nostartfiles output.s
        output=$(./a.out)
        rm -f a.out
    fi

    local dif=$(echo "scale=4;($(date +%s%N) - $start)/1000000000" | bc)

    if [ "${dif:0:1}" = "." ]
    then
        printf " 0%ss |" $dif
    else
        printf " %ss |" $dif
    fi

    if [ "$output" = "$right_output" ]
    then
        echo -e " $green OK \e[0m            $1"
    else
        results=1
        echo -e " $red ERROR \e[0m         $1"
    fi

    rm -f output.s
    cd ..
}

function test_parser
{
    cd parser
    option="--print-ast"

    echo "parser testing:"
    test test1
    test test2
    test test3
    test test4
    test test5
    test test6
    test test7
    test test8
    test test9
    test test10
    test test11
    test test12
    test test13
    test test14
    test test15
    test test16
    test test17

    option=""
    cd ..
}

function test_error
{
    cd error

    echo "error testing:"
    test test1
    test test2
    test test3
    test test4
    test test5
    test test6
    test test7
    test test8
    test test9
    test test10
    test test11
    test test12
    test test13
    test test14
    test test15
    test test16
    test test17
    test test18
    test test19
    test test20
    test test21
    test test22
    test test23
    test test24
    test test25
    test test26
    test test27
    test test28
    test test29
    test test30
    test test31
    test test32
    test test33
    test test34
    test test35

    cd ..
}

function test_gen()
{
    cd gen

    echo "code generator testing:"
    test test1
    test test2
    test test3
    test test4
    test test5
    test test6
    test test7
    test test8
    test test9
    test test10
    test test11
    test test12
    test test13
    test test14
    test test15
    test test16
    test test17
    test test18
    test test19
    test test20
    test test21
    test test22
    test test23
    test test24
    test test25
    test test26

    cd ..
}

function test_program
{
    cd program

    echo "testing simple programs:"
    test test_program
    test fibonacci
    test fibonacci_rec

    cd ..
}

gcc_path=$(command -v gcc)
nasm_path=$(command -v nasm)

if [ "$nasm_path" = "" ]
then
    echo "error: to run this script, nasm must be installed"
    exit 0
fi
if [ "$gcc_path" = "" ]
then
    echo "error: to run this script, gcc must be installed"
    exit 0
fi

echo "--------------------------------------------------------------------"
echo " Start program testing"
echo "--------------------------------------------------------------------"

if [ -e $program_path ]
then
    test_parser
    echo "--------------------------------------------------------------------"
    test_error
    echo "--------------------------------------------------------------------"
    test_gen
    echo "--------------------------------------------------------------------"
    test_program
else
    results=1
    echo ""
    echo "    $program_path: No such file"
    echo ""
fi

echo "--------------------------------------------------------------------"

g_dif=$(echo "scale=4;($(date +%s%N) - $g_start)/1000000000" | bc);
if [ "${g_dif:0:1}" = "." ]
then
    printf " 0%ss |" $g_dif
else
    printf " %ss |" $g_dif
fi

if [ $results -eq 1 ]
then
    echo -e " results:$red fail\e[0m"
else
    echo -e " results:$green success\e[0m"
fi

echo "--------------------------------------------------------------------"
