#!/bin/bash

echo "Building project..."
make clean > /dev/null
make > /dev/null

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

declare -A expected
expected["FCFS"]="515"
expected["SJF"]="461"
expected["STCF"]="393"
expected["RR"]="627"

all_passed=1

echo -e "\nRunning validation against workload1.txt...\n"

for algo in FCFS SJF STCF RR; do
    if [ "$algo" == "RR" ]; then
        output=$(./schedsim --algorithm=$algo --input=tests/workload1.txt --quantum=30 2>/dev/null)
    else
        output=$(./schedsim --algorithm=$algo --input=tests/workload1.txt 2>/dev/null)
    fi

    actual=$(echo "$output" | grep "^Average" | awk -F'|' '{print $5}' | tr -d ' ')
    
    if [[ "$actual" == *"${expected[$algo]}"* ]]; then
        echo "[PASS] $algo: Average TT matched (${expected[$algo]})"
    else
        echo "[FAIL] $algo: Expected ${expected[$algo]}, got '$actual'"
        all_passed=0
    fi
done

if [ $all_passed -eq 1 ]; then
    echo -e "\nAll tests passed successfully!"
    exit 0
else
    echo -e "\nSome tests failed."
    exit 1
fi
