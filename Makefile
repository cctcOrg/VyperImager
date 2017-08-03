PROGRAM_NAME := imager

CFLAGS := -g -Wall -Wextra -O2 `pkg-config --cflags gtk+-3.0`
LDFLAGS := -O2 `pkg-config --libs gtk+-3.0`
CC := clang

SOURCES := $(wildcard *.c)
.PHONY: all $(PROGRAM_NAME) clean test 

all: $(PROGRAM_NAME)

$(PROGRAM_NAME):
	$(CC) $(CFLAGS) $(SOURCES) -o $(PROGRAM_NAME) $(LDFLAGS)

clean:
	rm $(PROGRAM_NAME)
