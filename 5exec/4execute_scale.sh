#!/bin/bash
#!/bin/sh

cd ../2parasol/src

approach="scale"

echo $approach 
for example in t10i100l1000.data t10i100l2000.data t10i100l3000 t10i100l4000 t10i100l5000
do		 
	summary="result/${approach}"
	result="result/${approach}-${example}"
	echo $result
	for k in 100 1000 10000 
	do
		name="../../benchmark/synthesis/${example}"
		out="${result}"
		echo $out
		./main -i $name -k $k -o $result -u $summary --ibm
	done
done

cd ../../exec

exit


