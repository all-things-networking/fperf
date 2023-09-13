#!/bin/bash

set -euo pipefail

FPERF_PATH=build/autoperf

function run_test() {
  TEST_NAME=$1
  echo "Running test: $TEST_NAME"
  "./$FPERF_PATH" $TEST_NAME
  echo "Test Completed: $TEST_NAME"
}

trap "trap - SIGTERM && kill -- '-$$'" SIGINT SIGTERM EXIT

TESTS="prio rr loom fq_codel leaf_spine_bw"

for t in $TESTS; do
  run_test $t &
done

wait
