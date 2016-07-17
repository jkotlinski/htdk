CC=g++
CFLAGS=-I.
DEPS = def/rot.h
OBJ = main.o words.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

htfc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.exe *.o
