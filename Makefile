CC=gcc
CFLAGS=-c -Wall -g -rdynamic
LDFLAGS=
SOURCES=utils.c buff.c hashmap.c malloc.c task.c list.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=task_test hashmap_test

all: task_test hashmap_test

task_test: $(OBJECTS) task_test.o
	$(CC) $(LDFLAGS) $(OBJECTS) task_test.o -o task_test

hashmap_test: $(OBJECTS) hashmap_test.o
	$(CC) $(LDFLAGS) $(OBJECTS) hashmap_test.o -o hashmap_test

clean:
	rm -f *.o $(EXECUTABLE)
