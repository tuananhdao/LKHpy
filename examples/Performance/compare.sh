# Copy the built LKH executable to the current directory

echo "$ python3 LKH.py ../DATA/whizzkids96.par"
start_time=`date +%s`
python3 LKH.py ../DATA/whizzkids96.parr >/dev/null && echo run time is $(expr `date +%s` - $start_time) s

echo "$ ./LKH ../DATA/whizzkids96.par"
start_time=`date +%s`
./LKH ../DATA/whizzkids96.par >/dev/null && echo run time is $(expr `date +%s` - $start_time) s