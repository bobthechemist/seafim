OBJ = flame.o
DEPS = 
LIBS = -lseabreeze -lusb
CFLAGS = 
CC = gcc
EXTENSION = .c

%.o: %$(EXTENSION) $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

flame: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o
