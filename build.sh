g++ -c libraries/dinamicList.c
g++ -c libraries/strings.c
g++ -c libraries/nspLib.c
g++ -c libraries/hungarian/hungarian.c

g++ -c main1.c
g++ -o main1 dinamicList.o strings.o nspLib.o hungarian.o main1.o