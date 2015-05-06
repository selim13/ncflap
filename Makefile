BIN := target/ncflap
SRC := main.c win.c

CFLAGS  := -std=gnu99 -Wall -I. -I/usr/include/ncursesw
LDFLAGS := -lncursesw -lm

CC      = gcc
AS      = as
SIZE    = size
OBJCOPY = objcopy
OBJDUMP = objdump
NM      = nm
GDB     = gdb

OBJ := $(addprefix target/,$(SRC:%.c=%.o))
DEPS := $(addprefix target/,$(SRC:%.c=%.d))

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)
	$(SIZE) -B $@

target/%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MT $(@:%.d=%.o) -c $< -o $@

-include $(wildcard target/*.d)


.PHONY: clean

clean:
	rm -f  $(OBJ) $(BIN)
