!#/bin/zsh
clear
dt=`date +"%T"`
echo $dt
for o in 3 2 1 0 s z fast
do
	clang++ -std=c++17 -l sqlite3 trc.cpp -o trc$o -O$o
	for s in 7 31 127
	do
		for z in 1048576 2097162 4194324
		do
			ofn=trc-z$z-s$s-o$o.json
			ofn=trc.db
			./trc$o -s $s -f $ofn -z $z -d $dbn
			jsonlint -q $ofn
		done
	done
done
dt=`date +"%T"`
echo $dt "EOJ"
