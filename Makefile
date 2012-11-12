CC = gcc
CFLAGS = -O2 -Wall -march=native -mtune=native

TARGET = ask
prefix = $(shell echo $$HOME)

all: $(TARGET)

$(TARGET): ask.o
	$(CC) $(CFLAGS) ask.o -o $(TARGET)

ask.o:
	$(CC) $(CFLAGS) -c ask.c

clean:
	@rm -f ask.o

clobber:
	@rm -f ask.o $(TARGET)

install:
	install -m 755 $(TARGET) $(prefix)/bin/$(TARGET)

uninstall:
	rm -f $(prefix)/bin/$(TARGET)

.PHONY: clean clobber install uninstall
