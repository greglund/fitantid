INC=-I./inc -I./main -I./resource/fit
LIB=-L./main -L./resource/fit
CC=gcc
CFLAGS=--std=c99

ifdef SystemRoot
	RM = del /Q
	FixPath = $(subst /,\,$1)
	EXT = .exe
else
	RM = rm -f
	FixPath = $1
	EXT =
endif

subdirs := main/ resource/fit/
sources := $(wildcard $(addsuffix *.c,$(subdirs)))
objects := $(patsubst %.c,%.o,$(sources))

all: $(objects)
	$(CC) $(CFLAGS) $(INC) $(objects) -o fitantid$(EXT)

.c.o::
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

clean:
	$(RM) $(call FixPath,$(objects))
	$(RM) fitantid$(EXT)

