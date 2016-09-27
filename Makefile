#
# Makefile
#
# PROJECT:      skinnycat
# DESCRIPTION:  Small shell utility providing basic Skinny Cisco
#               protocol messages exchange
#
# Copyright (C) 2016 Stas Kobzat <stas@modulis.ca>
#
# TARGETS:
#   all
#   test
#   ctags
#   doc
#   clean

PROJECT   :=  skinnycat
PKG_CFG   :=  $(shell which pkg-config)
TARGET    := bin/$(PROJECT)
BUILDDIR  := build
SRCDIR    := src
DOCS      := doc

ifndef PKG_CFG
  $(error Not found pkg-config utility.\
          Install pkg-config package before continue)
endif

include src/Makefile

include tests/Makefile

.PHONY: doc
doc:
	@doxygen $(DOCS)/Doxygen
	@echo "Documentation generated."

.PHONY: ctags
# generate ctags for vim
# requires apr-1 source in root
ctags:
	@echo "Building tags"
	@test ! -d apr-1.5.2 && \
		wget http://apache.mirror.iweb.ca//apr/apr-1.5.2.tar.gz && \
		tar xfz apr-1.5.2.tar.gz && rm apr-1.5.2.tar.gz; :
	@ctags -R src apr-1.5.2 2> /dev/null

.PHONY: clean
clean: test_clean
	rm -f $(BUILDDIR)/*.o $(TARGET)
	rm -rf $(DOCS)/html

#######################
.PHONY: cov
cov:
	test -d cov || mkdir cov
	$(CC) $(CMOCKA_FLAGS) -fprofile-arcs -ftest-coverage -fPIC -O0 -o cov/skinny_msg \
		$(TESTDIR)/skinny_msg_test.c $(BUILDDIR)/skinny_msg.o $(CMOCKA_LIBS)
	./cov/skinny_msg
	mv *.gcno *.gcda cov
	lcov -t "skinny_msg.c" -o cov/skinny_msg.info -r cov/skinny_msg.info tests/* -d cov/
	genhtml -o cov/ cov/*.info

