INCLUDE = ./include
SRC = ./src
BUILD = ./build
FLAG = -g -c -I$(INCLUDE)

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(FLAG) $< -o $@

all:$(BUILD)/helpers/buffer.o \
	$(BUILD)/helpers/vector.o \
	$(BUILD)/lexer.o \
	$(BUILD)/process.o \
	$(BUILD)/fsm.o \
	$(BUILD)/token.o \
	$(BUILD)/node.o \
	$(BUILD)/parser.o \
	$(BUILD)/javac.o
	gcc -g -I$(INCLUDE) main.c $^ -o ./javac

clean:
	rm ./javac
	rm -rf $(BUILD)