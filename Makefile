PROGRAM_NAME := imager

CFLAGS := -g -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS := `pkg-config --libs gtk+-3.0`
CC := clang

SOURCES := $(wildcard *.c)
.PHONY: all $(PROGRAM_NAME) clean test 

debug: 
	$(CC) $(CFLAGS) -DDEBUG $(SOURCES) -o $(PROGRAM_NAME)-debug $(LDFLAGS)

all: $(PROGRAM_NAME)

$(PROGRAM_NAME):
	$(CC) $(CFLAGS) -O2 $(SOURCES) -o $(PROGRAM_NAME) -O2 $(LDFLAGS)

clean:
	rm $(PROGRAM_NAME)
