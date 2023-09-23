INCLUDE = -I./include
LIB = /usr/lib/x86_64-linux-gnu
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
	$(BUILD)/compiler.o
	gcc -g $(INCLUDE) -L$(LIB) javac.c $^ -o ./javac -lzip
	gcc -g $(INCLUDE) -L$(LIB) javap.c $^ -o ./javap -lzip

clean:
	rm -rf ./javac
	rm -rf $(BUILD)