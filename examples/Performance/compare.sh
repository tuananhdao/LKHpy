# Copy the built LKH executable to the current directory

echo "$ python3 LKH.py whizzkids96.par"
start_time=`date +%s`
python3 LKH.py whizzkids96.parr >/dev/null && echo run time is $(expr `date +%s` - $start_time) s

echo "$ ./LKH whizzkids96.par"
start_time=`date +%s`
./LKH whizzkids96.par >/dev/null && echo run time is $(expr `date +%s` - $start_time) s