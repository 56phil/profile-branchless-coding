clear
clang++ -std=c++17 trc.cpp -o trc3 -O3
clang++ -std=c++17 trc.cpp -o trc0 -O0
clang++ -std=c++17 trc.cpp -o trcF -Ofast
echo "-O0"
./trc0 -s 3 -f trc0-3.txt -z 100
echo "-O3"
./trc3 -s 3 -f trc3-3.txt -z 100
echo "-Ofast"
./trcF -s 3 -f trcF-3.txt -z 100
