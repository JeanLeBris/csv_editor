export OS
export CC=gcc
export CFLAGS= -Wall
export LDFLAGS= -Wall
export EXEC=csv_editor
SRCNAMES= main.c table.c actions.c colors.c config.c
export SRCDIR=src
SRC= $(foreach srcname, $(SRCNAMES), $(SRCDIR)/$(srcname))
OBJNAMES= $(SRCNAMES:.c=.o)
export OBJDIR=obj
OBJ= $(foreach objname, $(OBJNAMES), $(OBJDIR)/$(objname))
export BINDIR=bin
export DEPENDENCIES=

ifeq ($(OS), Windows)
	RMDIR= rmdir
	RMFILE= del /s /q
	COPYFILE= copy
	SHARED_LIBRARY_EXT= dll
	FILE_SLASH=\\

else
ifeq ($(OS), Windows_NT)
	OS=Windows
	RMDIR= rmdir
	RMFILE= del /s /q
	COPYFILE= copy
	SHARED_LIBRARY_EXT= dll
	FILE_SLASH=\\

else
ifeq ($(OS), Linux)
	RMDIR= rmdir
	RMFILE= rm
	COPYFILE= cp
	SHARED_LIBRARY_EXT= so
	FILE_SLASH=/
	LDFLAGS+= -lncurses -lm
endif
endif
endif

export RMDIR
export RMFILE
export COPYFILE
export SHARED_LIBRARY_EXT
export FILE_SLASH

ifeq ($(LIBRARY_TYPE), shared)
else
	ifeq ($(LIBRARY_TYPE), static)
	else
		LIBRARY_TYPE=shared
	endif
endif
export LIBRARY_TYPE

compile:bin obj $(OBJNAMES)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o:
	$(CC) -c $(SRCDIR)/$(@:.o=.c) -o $(OBJDIR)/$@ $(CFLAGS)

.PHONY: compile clean

bin:
	mkdir bin

obj:
	mkdir obj

clean:
	$(RMFILE) $(OBJDIR)$(FILE_SLASH)*
	$(RMDIR) $(OBJDIR)
	$(RMFILE) $(BINDIR)$(FILE_SLASH)*
	$(RMDIR) $(BINDIR)
	$(RMFILE) *.exe
