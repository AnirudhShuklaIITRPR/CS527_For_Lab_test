CC = gcc
CFLAGS = -Wall

minicomputer.exe: main.o compiler.o processor.o memory.o
	$(CC) main.o compiler.o processor.o memory.o -o minicomputer.exe

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c
	$(CC) $(CFLAGS) -c compiler.c

processor.o: processor.c
	$(CC) $(CFLAGS) -c processor.c

memory.o: memory.c
	$(CC) $(CFLAGS) -c memory.c

clean:
	rm -f *.o minicomputer.exe