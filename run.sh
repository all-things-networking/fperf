make -j6
LOG_FILE=$1
echo > $LOG_FILE
for ((i=10; i<=500; i = i + 5)); do
  gtime -f "$i,%e" -o $LOG_FILE -a ./build/fperf $i
#  (time ./build/fperf 10) 2>&1 | grep cpu | awk '{print $9}'
#  echo "$i done"
done