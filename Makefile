build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o renderer

builddebug:
	gcc -Wall -g -std=c99 ./src/*.c -lSDL2 -lm -o renderer

debug:
	gdb renderer

run:
	./renderer

clean:
	rm renderer