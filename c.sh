clear
clang++ -std=c++17 trc.cpp -o trc3 -O3
clang++ -std=c++17 trc.cpp -o trc0 -O0
clang++ -std=c++17 trc.cpp -o trcF -Ofast
echo "-O0"
./trc0 -s 23 -f trc0-23.txt -z 2000000
echo "-O3"
./trc3 -s 23 -f trc3-23.txt -z 2000000
echo "-Ofast"
./trcF -s 23 -f trcF-23.txt -z 2000000
