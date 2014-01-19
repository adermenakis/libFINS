# #############################################
# Makefile for libFINS library
#
# Author: Alexandros Dermenakis
# email: alexandros.dermenakis@gmail.com
#
# #############################################

LIBNAME=libFINS.so
CFLAGS=-c -Wall -g
LDFLAGS=-g -shared -fPIC
SOURCES=FINS.c
EXECUTABLE=hello

all:
	$(CC) $(LDFLAGS) $(SOURCES) -o $(LIBNAME)

clean:
	rm -f $(LIBNAME)
