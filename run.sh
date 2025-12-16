set -e
trap "echo 'Interrupted!'; exit 1" INT

RAND_SEED=$1

mkdir -p "wls"
mkdir -p "logs"

for i in 10 20 25 30 40 50 75 100 125 150 175 200 250 300 350 400 450 500; do
#for i in 40 50 75 100 125 150 175 200 250 300 350 400 450 500; do
  echo "RAND_SEED=$RAND_SEED"
  echo "BUF_SIZE=$i Started"
  export FPERF_OUTPUT_WL_PATH="wls/fq.$i.txt"
  ./build/fperf $i $RAND_SEED > "logs/fq.$i.log"
  echo "BUF_SIZE=$i Done"
done
