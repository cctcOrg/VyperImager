PROGRAM_NAME := imager

CFLAGS := -g -DDEBUG -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS := `pkg-config --libs gtk+-3.0` -lblkid -lparted -DDEBUG
CC := clang
SRCDIR := src

SOURCES := $(patsubst $(SRCDIR)/%.c,$(SRCDIR)/%.o,$(wildcard $(SRCDIR)/*.c))

.PHONY: all $(PROGRAM_NAME) clean test 

all: $(PROGRAM_NAME)

debug: $(SOURCES)
	$(CC) $(CFLAGS) -DDEBUG $(SOURCES) -o $(PROGRAM_NAME)-debug $(LDFLAGS)

$(PROGRAM_NAME): $(SOURCES)
	$(CC) $(CFLAGS) -O2 $(SOURCES) -o $(PROGRAM_NAME) -O2 $(LDFLAGS)

clean:
	rm -f $(SRCDIR)/*.o
	rm -f $(PROGRAM_NAME)
