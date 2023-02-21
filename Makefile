#
# Copyright (C) 2023, Mauro Meneghin
#

CC ?= gcc
RM ?= rm

LIB=libctrace.so
SOURCES=libctrace.c

.PHONY: all clean

all: $(LIB)

$(LIB): $(SOURCES)
	$(CC) -shared -fPIC -o $@ $^ -ldl

clean:
	$(RM) -f -- $(LIB)
