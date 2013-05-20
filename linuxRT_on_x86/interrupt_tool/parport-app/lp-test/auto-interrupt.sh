#/!bin/sh
i=10
while i<0
do
	./lp >> test.log
	i=i-1
done
