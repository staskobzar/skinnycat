#
# Makefile
#
# Copyright (C) 2016 Stas Kobzar <stas@modulis.ca>
#

APR_LIBS := $(shell pkg-config --libs apr-1)
CFLAGS   += -g -Wall $(shell pkg-config --cflags apr-1)
LDFLAGS  += $(APR_LIBS)
OBJECTS  := main.o skinny_proto.o skinny_msg.o

ifndef APR_LIBS
  $(error Required library apr-1 not found.\
          Install Apache Poratable Runtime library to continue.)
endif

$(TARGET): $(patsubst %.o,$(BUILDDIR)/%.o,$(OBJECTS))
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo " CC   $@"

$(BUILDDIR)/main.o: $(patsubst %,$(SRCDIR)/%,main.c skinny_proto.h skinny_msg.h)
	@$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
	@echo " LD   $@"

$(BUILDDIR)/skinny_proto.o: $(SRCDIR)/skinny_proto.c $(SRCDIR)/skinny_proto.h
	@$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
	@echo " LD   $@"

$(BUILDDIR)/skinny_msg.o: $(SRCDIR)/skinny_msg.c $(SRCDIR)/skinny_msg.h
	@$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
	@echo " LD   $@"

