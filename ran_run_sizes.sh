#!/bin/bash

set -e
trap "echo 'Interrupted!'; exit 1" INT

SIZES=(10 20 30 40 50 75 100 150 200 250 300 350 400 450 500)
N=4

for s in "${SIZES[@]}"; do
  for i in $(seq 1 $N); do
      RAND=$RANDOM
      mkdir -p "tmp/$RAND"
      export WL_FILE="tmp/$RAND/leaf_$s.txt"
      (
          ./build/fperf $s "$RAND" &> "tmp/out_${RAND}_${s}.log"
          echo "Task with RAND=$RAND size=$s finished"
      ) &done
done

wait
echo "All jobs completed."