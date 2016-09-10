#
# skinnycat
#
SRCDIR := src
TARGET := bin/skinnycat

CFLAGS := -g -Wall $(shell pkg-config --cflags apr-1)
LIBS := $(shell pkg-config --libs apr-1)


all: $(TARGET)

$(TARGET): $(SRCDIR)/main.o $(SRCDIR)/skinny_proto.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(SRCDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/skinny_proto.h

$(SRCDIR)/skinny_proto.o: $(SRCDIR)/skinny_proto.c $(SRCDIR)/skinny_proto.h

.PHONY: clean
clean:
	rm -f $(TARGET) $(SRCDIR)/*.o
