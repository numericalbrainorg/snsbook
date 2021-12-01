CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937
MISC_OBJS = SFMT.o

all: run

main: main.o gr.o go.o pc.o conn.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

main.o: main.c gr.h go.h conn.h param.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<
gr.o: gr.c gr.h param.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<
go.o: go.c go.h param.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<
pc.o: pc.c pc.h param.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<
conn.o: conn.c conn.h param.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

run: main
	./main

clean:
	rm -f main *.o

distclean: clean
	rm -f gr.*.[0-9]* go.*.[0-9]* pc.*.[0-9]*
