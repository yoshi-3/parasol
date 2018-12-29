#!/bin/bash
#!/bin/sh

cd ../2parasol/src

approach="rc"

echo $approach 
for example in 1eq 2wlog 3retail 4accidents 5chess 6connect 7kosarak 8pumsb 9mushroomdo		 
	summary="result/${approach}"
	result="result/${approach}-${example}"
	echo $result
		name="../../benchmark/real/${example}.txt"
		out="${result}"
		echo $out
		./main -i $name -k 12000 --comp -o $result -u $summary
done

cd ../../exec

exit
