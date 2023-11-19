#!/bin/bash

trap 'pkill -f fperf; exit;' SIGINT SIGTERM

for i in $(seq 1 1000); do
    echo "Round: $i"
    ./perf.sh
    echo "----------------------"
done


