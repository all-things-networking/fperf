#!/bin/bash
set -euo pipefail

function cleanup() {
  pkill -f fperf
}

function get_random() {
  RESULT=$RANDOM
  while awk '{print $2}' < rands.log | grep -q "$RESULT"; do
    RESULT=$RANDOM
  done
  echo $RESULT
}

function get_best_time() {
  tail -n 1 seeds.log | awk '{print $1}'
#  awk '{if ($1) print $1}' < "seeds.log" | sort -n | head -1
}


function main() {
  truncate -s 0 output.log

  for i in $(seq 1 6); do
    RAND="$(get_random)"
    echo "$RAND" >> rands.log
    ./build/fperf "$RAND"  >> output.log &
  done

  ELAPSED_TIME=0

  while [ true ]; do
      echo "Elapsed Time: $ELAPSED_TIME"
      if [ -n "$(get_best_time)" ] && [ "$ELAPSED_TIME" -gt "$(get_best_time)" ]; then
        echo "Elapsed Time exceeded best time, skipping this round"
        break;
      fi
      if grep -q "FINISHED" "output.log"; then
        echo "Found finished process"
        BEST_SEED=$(awk '/FINISHED/{print $2}' < output.log | tail -n 1)
        echo "$ELAPSED_TIME $BEST_SEED" >> seeds.log
        break
      fi
      sleep 1
      ELAPSED_TIME=$(( $ELAPSED_TIME + 1))
  done

  cleanup
}

main
