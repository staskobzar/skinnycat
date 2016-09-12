#
# Tests
#
# Copyright (C) 2016 Stas Kobzar <stas@modulis.ca>
#

CMOCKA_LIBS := $(shell pkg-config --libs cmocka 2>&1 /dev/null)
TESTDIR     := tests
TESTS       := $(wildcard $(TESTDIR)/*_test.c)

.PHONY: test test_suit tests_run test_clean

test: $(patsubst %,$(BUILDDIR)/%,$(OBJECTS)) test_suit tests_run

test_suit: $(patsubst %.c, %, $(TESTS))
	@echo CC   $^

tests_run:
	@for t in tests/*_test;do ./$$t; done

test_clean:
	@for t in tests/*_test;do rm -f $$t; done
