CC = cc
CFLAGS = -g -MMD -MP
RM = rm -f
BIN = bin/main
BUILD_DIR = build

SRC = $(wildcard *.c) $(wildcard ./stack/*.c)
OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))
DEP = $(OBJ:.o=.d)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

-include $(DEP)

run: $(BIN)
	./$(BIN) $(ARGS)

clean: 
	$(RM) -r $(BUILD_DIR) bin
		
gdb:
	gdb -iex "set debuginfod enabled off" ./$(BIN)

valg: 
	valgrind ./$(BIN)
