#any questions see: http://opensourceforu.com/2012/06/gnu-make-in-detail-for-beginners/

CC = g++
CFLAGS = -Wall -g
DEPS = libraries/dinamicList.h libraries/strings.h libraries/apc/apc.h libraries/nspLib.h libraries/hungarian/hungarian.h libraries/cost_matrix.h libraries/operators.h 
OBJ = main1.o libraries/dinamicList.c libraries/strings.c libraries/apc/apc.c libraries/nspLib.c libraries/hungarian/hungarian.c libraries/cost_matrix.c libraries/operators.c

main1: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "Cleaning up.."
	rm -rf *.o
	rm -rf libraries/*.o
	rm main

go: libraries/dinamicList.c libraries/strings.c libraries/nspLib.c libraries/hungarian/hungarian.c libraries/apc/apc.c main1.c
	g++ -o main1 libraries/dinamicList.c libraries/strings.c libraries/nspLib.c libraries/hungarian/hungarian.c libraries/apc/apc.c main1.c