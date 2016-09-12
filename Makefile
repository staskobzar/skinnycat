#
# Makefile
#
# PROJECT:      skinnycat
# DESCRIPTION:  Small shell utility providing basic Skinny Cisco
#               protocol messages exchange
#
# Copyright (C) 2016 Stas Kobzat <stas@modulis.ca>
#

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
# generate ctags for vim IDE
# requires apr-1 source in root
ctags:
	@ctags -R src apr-1.5.2

.PHONY: clean
clean: test_clean
	rm -f $(BUILDDIR)/*.o $(TARGET)
	rm -rf $(DOCS)/html
