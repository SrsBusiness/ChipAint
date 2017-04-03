CC				=	clang

# TODO: Add -Werror once we have actual code
OBJFLAGS		=	-c -Wall -Iinclude --std=c11
BINFLAGS		=	-Wall -Iinclude --std=c11

SRC				=	src
LIB				=	lib
OBJ				=	obj
BIN				=	bin
TARGET			=	$(BIN)/chip8
SRCFILES		:= $(wildcard $(SRC)/*.c)
OBJECTS 		:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o, $(SRCFILES))

TESTDIR 		= test
UNITTESTDIR		= $(TESTDIR)/unit
UNITTESTBIN 	= $(UNITTESTDIR)/tests

all: OBJFLAGS 	+= -O3
all: BINFLAGS 	+= -O3
all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN)
	$(CC) $(BINFLAGS) -o $@ $(OBJECTS) $(LINKFLAGS) main.c

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(OBJFLAGS) -o $@ $<

.PHONY: tests
test: LINKFLAGS += -lcmocka
test: OBJFLAGS 	+= -g
test: BINFLAGS 	+= -g
test: $(OBJECTS)
	$(CC) $(BINFLAGS) -o $(UNITTESTBIN) $(LINKFLAGS) $(OBJECTS) $(UNITTESTDIR)/*.c
	$(UNITTESTBIN)

.PHONY: clean
clean:
	$(RM) -rf $(OBJ) $(BIN)

$(BIN) $(OBJ):
	mkdir -p $@
