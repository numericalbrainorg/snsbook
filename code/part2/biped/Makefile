CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: main

main: main.o os.o ms.o misc.o mat.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

run: main
	./main
	./helper/os_pp.rb
	./helper/sp_pp.rb
	./helper/stick.rb
	./helper/animate.rb
	./helper/animate_post.zsh

clean:
	rm -f main *.o

distclean: clean
	rm -rf *.dat *.mp4 ./animate
