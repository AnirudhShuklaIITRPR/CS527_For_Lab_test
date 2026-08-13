CC = gcc

CFLAGS = -Wall

OBJ = main.o compiler.o processor.o memory.o

simulator.exe: $(OBJ)
	$(CC) $(OBJ) -o minicomputer.exe

main.o: main.c compiler.h memory.h processor.h
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

clean:
	del /Q *.o minicomputer.exe	