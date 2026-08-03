
CC = gcc
CFLAGS= -Wall -g -Wpedantic

# Pattern Rule: Compiles any name into an executable from name.c
# % acts as a wildcard matching the base filename

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
