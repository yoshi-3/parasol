#!/bin/bash
#!/bin/sh

cd ../2parasol/src/

approach="skip"

echo $approach 
for example in 1eq 2wlog 3retail 4accidents 5chess 6connect 7kosarak 8pumsb 9mushroom
do		
	summary="result/${approach}"
	name="../benchmark/real/${example}.txt"
	./main -i $name -k 12000 -o $summary  
	
done

cd ../../exec

exit


