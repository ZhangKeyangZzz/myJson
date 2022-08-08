RM := rm -rf
CC := gcc
AR := ar -crv
CFLAGS := -Wall -O2
TARGET := libjson.a
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)


.PHONY: clean


$(TARGET): $(OBJECTS)
	$(AR) $@ $^


%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^


clean: 
	$(RM) *.o
	$(RM) *.a
