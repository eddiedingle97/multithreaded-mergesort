CC := gcc
FLAGS := -Wall
OTHERS := -lpthread -lrt

sort : sort.c mergesort.c
	$(CC) sort.c mergesort.c -o sort $(FLAGS) $(OTHERS)