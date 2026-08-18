CC = cc
CFLAGS = -g
RM = rm -f
SRC = $(wildcard ../*.c) $(wildcard *.c) 
BIN = bin/main
OBJ = $(patsubst %.c, %.o, $(SRC))
SAN = -g -fno-omit-frame-pointer -fsanitize=address,undefined

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

asan: $(SRC)
	$(CC) $(CFLAGS) $(SAN) -O1 $^ -o ./bin/main_asan
	ASAN_OPTIONS=abort_on_error=1:detect_stack_use_after_return=1 ./bin/main_asan


run: $(BIN)
	./$(BIN)

clean: 
	rm -r bin

valg:
	valgrind ./$(BIN)
