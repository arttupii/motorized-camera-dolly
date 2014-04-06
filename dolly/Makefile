CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g 
LDFLAGS=-g 
LDLIBS=

SRCS=main.o menu.o
OBJS=main menu

all: main.o menu.o
    g++ -o testi $(OBJS)

menu.o: menu.cpp main.h

main.o: main.cpp 


clean:
    $(RM) $(OBJS)

