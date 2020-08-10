!#/bin/zsh
clear
dt=`date +"%T"`
echo $dt
for o in 3 2 1 0 s z fast
do
	clang++ -std=c++17 trc.cpp -o trc$o -O$o
	for s in 21 321 4321
	do
		for z in 10123456 20123456 40123456 80123456
		do
			ofn=trc$o-s$s-z$z.json
			./trc$o -s $s -f $ofn -z $z
			jsonlint -q $ofn
		done
	done
done
dt=`date +"%T"`
echo $dt "EOJ"
