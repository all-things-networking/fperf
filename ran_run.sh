#!/bin/bash

N=6

for i in $(seq 1 $N); do
    RAND=$RANDOM
    export WL_FILE="tmp/wl_$RAND.txt"
   # Run each task in background and print when it finishes
    (
        ./build/fperf 10 "$RAND" &> "tmp/out_${RAND}.log"
        echo "Task with RAND=$RAND finished"
    ) &done

wait
echo "All jobs completed."