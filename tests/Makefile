#
# Tests
#
# Copyright (C) 2016 Stas Kobzar <stas@modulis.ca>
#

CMOCKA_LIBS  := $(shell pkg-config --libs cmocka apr-1)
CMOCKA_FLAGS := -g -Wall $(shell pkg-config --cflags cmocka apr-1)
TESTDIR      := tests
TESTS        := $(wildcard $(TESTDIR)/*_test.c)

.PHONY: test test_suit unit_tests test_clean cov e2e_tests

test: $(patsubst %,$(BUILDDIR)/%,$(OBJECTS)) test_suit e2e_tests unit_tests

test_suit:
	@echo " CC   skinny_msg_test"
	@$(CC) $(CMOCKA_FLAGS) -o $(TESTDIR)/skinny_msg_test \
		$(TESTDIR)/skinny_msg_test.c $(BUILDDIR)/skinny_msg.o $(CMOCKA_LIBS)
	@echo " CC   skinnycat_opts_test"
	@$(CC) $(CMOCKA_FLAGS) -o $(TESTDIR)/skinnycat_opts_test \
		$(TESTDIR)/skinnycat_opts_test.c $(BUILDDIR)/skinnycat_opts.o $(CMOCKA_LIBS)

unit_tests:
	@echo "+------------------------------"
	@echo "| RUNNING UNIT TESTS:"
	@echo "+------------------------------"
	@for t in tests/*_test;do ./$$t; done

test_clean:
	@echo Clean tests
	@for t in tests/*_test;do rm -f $$t; done
	@rm -f tests/e2e/*.log tests/e2e/*.sum

# Unit tests coverage
cov:
	@test -d cov || mkdir cov
	@$(CC) $(CMOCKA_FLAGS) -fprofile-arcs -ftest-coverage -fPIC -O0 -o cov/skinny_msg \
		$(TESTDIR)/skinny_msg_test.c $(SRCDIR)/skinny_msg.c $(CMOCKA_LIBS)
		$(TESTDIR)/skinnycat_opts_test.c $(BUILDDIR)/skinnycat_opts.o $(CMOCKA_LIBS)
	@./cov/skinny_msg
	@mv *.gcno *.gcda cov
	@lcov -q -t "skinny_msg.c" -o cov/skinny_msg.info -c -d cov/
	@lcov -t "skinny_msg.c" -o cov/skinny_msg.info -r cov/skinny_msg.info tests/* -d cov/
	@genhtml -q -o cov/ cov/*.info

# End-to-end functional testing
e2e_tests:
	@echo
	@echo "+------------------------------"
	@echo "| RUNNING END 2 END FUNC TESTS:"
	@echo "+------------------------------"
	@echo
	@runtest SKINNYCAT=$(shell pwd)/bin/skinnycat
