C = cc
CFLAGS = -g -Wall -Werror -Wpedantic -Wformat=2
RM = rm -f
BIN = bin/main
BUILD_DIR = build

SRC = $(wildcard *.c)\
$(wildcard ./stack/*.c) \
$(wildcard ./backup/*.c) \
$(wildcard ./dir_walk/*.c) \
$(wildcard ./file_stat/*.c) \
$(wildcard ./mem_pool/*.c) \
$(wildcard ./path_utils/*.c) \
$(wildcard ./mkdir/*.c) \
$(wildcard ./usb/*.c)  

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
	rm -rf /mnt/usb/copied
	./$(BIN) $(ARGS)

clean: 
	rm -rf /mnt/usb/copied
	$(RM) -r $(BUILD_DIR) bin
		
gdb:
	rm -rf /mnt/usb/copied
	gdb -iex "set debuginfod enabled off" ./$(BIN)

valg: 
	rm -rf /mnt/usb/copied
	valgrind --track-origins=yes  ./$(BIN)
