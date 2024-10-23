CC = gcc
CFLAGS = -Wall -Werror -I.
LDFLAGS =
AR = ar
ARFLAGS = rcs

LIB = libmlpt.a
OBJS = mlpt.o

.PHONY: all clean

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB)