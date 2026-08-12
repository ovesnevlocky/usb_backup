CC = cc
CFLAGS = -g
RM = rm -f
SRC = $(wildcard ./*.c)
BIN = bin/main
OBJ = #(patsubst %.c $(SRC))

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

run: $(BIN)
	./$(BIN)
