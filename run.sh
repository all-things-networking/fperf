set -e
trap "echo 'Interrupted!'; exit 1" INT
make clean
make -j6


for i in 10 20 25 30 40 50 75 100 125 150 175 200 250 300 350 400 450 500; do
  echo "BUF_SIZE=$i Started"
  export WL_FILE="./wls/fq.$i.txt"
  ./build/fperf $i > logs/fq.$i.log
  echo "BUF_SIZE=$i Done"
done
