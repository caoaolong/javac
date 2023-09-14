INCLUDE = -I./include -I/opt/homebrew/include
LIB = /opt/homebrew/lib
SRC = ./src
BUILD = ./build
FLAG = -g -c $(INCLUDE)

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
	$(BUILD)/datatype.o \
	$(BUILD)/parser.o \
	$(BUILD)/loader.o \
	$(BUILD)/javac.o
	gcc -g $(INCLUDE) -L$(LIB) -lzip main.c $^ -o ./javac

clean:
	rm -rf ./javac
	rm -rf $(BUILD)