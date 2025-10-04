#!/usr/bin/env bash
set -e
trap "echo 'Interrupted!'; exit 1" INT

SIZES=(10 20 25 30 40 50 75 100 125 150 175 200 250 300 350 400 450 500)

TC=$1
RAND_SEED=8000
NPROC=${#SIZES[@]}

mkdir -p "wls/$TC"
mkdir -p "logs/$TC"


run_task() {
  i=$1
  echo "RAND_SEED=$RAND_SEED"
  echo "BUF_SIZE=$i Started"
  export WL_FILE="wls/$TC/$i.txt"
  ./build/fperf "$i" "$RAND_SEED" > "logs/$TC/$i.log"
  echo "BUF_SIZE=$i Done"
}

export -f run_task
export RAND_SEED

parallel -j "$NPROC" run_task ::: "${SIZES[@]}"