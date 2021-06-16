CC = g++
CPPFLAGS = -Wall -std=c++17
RM=rm -f

t9: t9.cpp
	$(CC) $(CPPFLAGS) -o t9 t9.cpp

all: t9

clean:
	$(RM) t9