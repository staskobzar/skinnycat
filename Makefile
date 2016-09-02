#
# skinnycat
#
SRCDIR := src
TARGET := bin/skinnycat

CFLAGS := -g -Wall $(shell pkg-config --cflags apr-1)
LIBS := $(shell pkg-config --libs apr-1)


all: $(TARGET)

$(TARGET): $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
