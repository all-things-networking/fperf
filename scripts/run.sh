#!/bin/bash

TEST_NAME=$1

trap 'pkill -f fperf; exit;' SIGINT SIGTERM

for i in $(seq 1 1000); do
    echo "Round: $i"
    ./perf.sh $TEST_NAME
    echo "----------------------"
done


