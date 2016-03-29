TARGET = es_tests
LIBS = -lm -lcunit -luuid
CC = gcc
CFLAGS = -g -Wall

.PHONY: all clean default

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

