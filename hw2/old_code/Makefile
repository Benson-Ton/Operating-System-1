prog := movies_by_year
src := $(wildcard *.c)
obj := $(src:.c=.o)

CC = gcc
CFLAGS = -std=gnu99 -g

$(prog): $(obj)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(obj) $(prog)
