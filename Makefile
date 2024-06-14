EXEC = 2048
CC = gcc
CFLAGS = -Wall
LIBS = -lncurses

$(EXEC): 2048.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

build: $(EXEC)

clean:
	rm -f $(EXEC)

run: $(EXEC)
	./$(EXEC)