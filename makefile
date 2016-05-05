TARGET = es_tests
LIBS = -lm -lcunit -luuid -lprotobuf-c
CC = gcc
CFLAGS = -g -std=c99 -Wall

.PHONY: all clean test default valgrind

default: $(TARGET)
all: default

OBJECTS = $(patsubst ./src/%.c, ./src/%.o, $(wildcard ./src/*.c))
HEADERS = $(wildcard src/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $@

clean:
	-rm -f src/*.o
	-rm -f $(TARGET)
test:
	./es_tests

valgrind:
	valgrind --leak-check=yes ./es_tests
