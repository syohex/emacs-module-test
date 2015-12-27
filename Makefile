EMACS_ROOT ?= ../..
EMACS ?= emacs

CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -O2 -Wall -fPIC $(CPPFLAGS)

.PHONY : clean test

all: module-test-core.so

module-test-core.so: module-test-core.o
	$(LD) -L . -shared $(LDFLAGS) -o $@ $^

module-test-core.o: module-test-core.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f module-test-core.o module-test-core.so

test:
	$(EMACS) -Q -batch -L . -l test/test.el -f ert-run-tests-batch-and-exit
