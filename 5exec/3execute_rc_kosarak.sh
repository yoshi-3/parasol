#!/bin/bash
#!/bin/sh

cd ../2parasol/src

for example in 7kosarak
do		 
	echo $result
	for k in 1000 3000 5000 7000 9000 12000 
	do
		result="result/${example}-${k}"
		name="../../benchmark/real/${example}.txt"
		out="${result}"
		echo $out
		./main -i $name -k $k -o $out -u $example  
	done
done

cd ../../exec

cd ../3skip_lc_ss/src/

for example in 7kosarak
do		 
	echo $result
	for k in 1000 3000 5000 7000 9000 12000 
	do
		result="result/${example}-${k}"
		name="../../benchmark/real/${example}.txt"
		out="${result}"
		echo $out
		./main -i $name -k $k -o $out  
	done
done

cd ../../exec


exit


