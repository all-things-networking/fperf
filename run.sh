set -e
trap "echo 'Interrupted!'; exit 1" INT
make -j6


for i in 10 20 30 40 50 75 100 150 200; do
  export WL_FILE="./wls/fq.$i.txt"
  ./build/fperf $i > logs/fq.$i.log
done
