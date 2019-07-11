g++ -g -o main src/json.cpp src/main.cpp -lgtest -lpthread --std=c++11
valgrind --tool=memcheck --leak-check=full --show-reachable=yes --trace-children=yes --log-file=./json_valgrind.log ./main
vim json_valgrind.log
