TARGET = es_tests
LIBS = -lm -lcunit -luuid -lprotobuf-c
CC = gcc
CFLAGS = -g -Wall -std=c99

.PHONY: all clean test default

default: $(TARGET)
all: default

OBJECTS = $(patsubst ./src/%.c, ./src/%.o, $(wildcard ./src/*.c))
HEADERS = $(wildcard src/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
test:
	./es_tests
