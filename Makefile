CC := gcc
FLAGS := -Wall
OTHERS := -lpthread -lrt

sort : getdata.c mergesort.c
	$(CC) getdata.c mergesort.c -o sort $(FLAGS) $(OTHERS)