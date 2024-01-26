INCLUDE = -I./include -I/usr/include -I/usr/local/include
LIB = -L/usr/lib/aarch64-linux-gnu
SRC = ./src
BUILD = ./build
FLAG = -g -c $(INCLUDE) -Wno-switch-outside-range

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
	$(BUILD)/formatter.o \
	$(BUILD)/compiler.o
	gcc -g $(INCLUDE) $(LIB) javac.c $^ -o ./javac -lzip
	gcc -g $(INCLUDE) $(LIB) javap.c $^ -o ./javap -lzip

clean:
	rm -rf ./javac
	rm -rf ./javap
	rm -rf $(BUILD)