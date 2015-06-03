#!/bin/bash

# Process test result file line by line and generate result summaries and logs
# Argument: Name of file that contains test results. Has .abs.test extension
function process_test_result() {
    if [ -d "$1" ]; then
        echo "Invalid call to test_summary_rec"
        return
    elif [ ${1: -9} != ".abs.test" ]; then
        # Warning! Space between : and -9 is necessary
        echo "Not valid file extension"
        return
    fi

    local new_log_file="${1:0: -9}.log"
    if [ -f "$new_log_file" ]; then
        mv -v "$new_log_file" "${new_log_file}.old"
    fi
    while read line; do
        if [ "${line:0:1}" == "#" ]; then
            echo "${line:2}" >> $new_log_file
        elif [ "${line:0:4}" == "PASS" ]; then
            pass=$(($pass + 1))
            echo "$line" >> $new_log_file
            echo "$separator" >> $new_log_file
        elif [ "${line:0:4}" == "FAIL" ]; then
            fail=$(($fail + 1))
            echo "$line" >> $new_log_file
            echo "$separator" >> $new_log_file
            test_log_text="${test_log_text}${line:5} ($new_log_file)\n"
        elif [ "${line:0:5}" == "XFAIL" ]; then
            xfail=$(($xfail + 1))
            echo "$line" >> $new_log_file
            echo "$separator" >> $new_log_file
        elif [ "${line:0:4}" == "SKIP" ]; then
            skip=$(($skip + 1))
            echo "$line" >> $new_log_file
            echo "$separator" >> $new_log_file
        fi;
    done < $1;
    rm -rf $1
}

# Go through the codebase folders recursively searching for .abs.test files
# Argument: Folder to look at
function test_summary_rec() {
    if [ ! -d "$1" ]; then
        echo "Invalid call to test_summary_rec"
        return
    fi
    for file in "$1"/*; do
        if [ -d "$file" ]; then
            test_summary_rec "$file"
        elif [ ${file: -9} == ".abs.test" ]; then
            # Warning! Space between : and -9 is necessary
            process_test_result "$file"
        fi
    done;
}

test_file="$1"
if [ -z "$test_file" ]; then
    echo "Missing test file name"
    exit 1
fi

if [ -f "$test_file" ]; then
    mv -v "$test_file" "${test_file}.old"
fi

separator="================================================================================"
pass=0
xfail=0
skip=0
fail=0
test_log_text="Tests that failed:\n"

echo "Test run on $(date)" >> $test_file

test_summary_rec "."

if [ $fail == 0 ]; then
    test_log_text="${test_log_text}None\n"
fi

total=$(($pass + $xfail + $skip + $fail))
if [ ! $total -eq 0 ]; then
    echo "$separator"
    echo "Test execution result for $total tests run:"
    echo "Passed tests: $pass"
    echo "Failed tests: $fail"
    echo "Failed tests (expected): $xfail"
    echo "Skipped tests: $skip"
    echo ""
    echo -ne "$test_log_text"
    echo "$separator"
fi
