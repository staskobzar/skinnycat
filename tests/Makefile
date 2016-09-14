#
# Tests
#
# Copyright (C) 2016 Stas Kobzar <stas@modulis.ca>
#

CMOCKA_LIBS  := $(shell pkg-config --libs cmocka 2> /dev/null)
CMOCKA_FLAGS := -g -Wall $(shell pkg-config --cflags cmocka)
TESTDIR      := tests
TESTS        := $(wildcard $(TESTDIR)/*_test.c)

.PHONY: test test_suit tests_run test_clean

test: $(patsubst %,$(BUILDDIR)/%,$(OBJECTS)) test_suit tests_run

test_suit:
	@echo " CC   skinny_msg_test"
	@$(CC) $(CMOCKA_FLAGS) -o $(TESTDIR)/skinny_msg_test \
		$(TESTDIR)/skinny_msg_test.c $(BUILDDIR)/skinny_msg.o $(CMOCKA_LIBS)

tests_run:
	@echo "+------------------------------"
	@echo "| RUNNING TESTS:"
	@echo "+------------------------------"
	@for t in tests/*_test;do ./$$t; done

test_clean:
	@for t in tests/*_test;do rm -f $$t; done
