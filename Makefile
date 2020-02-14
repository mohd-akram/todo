# Windows \
!if 1 # \
EXEEXT=.exe # \
!else
.POSIX:
# \
!endif

all: todo$(EXEEXT)

todo$(EXEEXT): src/main.c src/todo.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c src/todo.c

# POSIX \
!if 0

test: .force
	cd test && ./test

.force:

prefix = /usr/local
bindir = $(prefix)/bin
mandir = $(prefix)/share/man

-include config.mk

install: todo
	install -d $(DESTDIR)$(bindir)
	install -s todo $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(mandir)/man1
	install man/todo.1 $(DESTDIR)$(mandir)/man1

# \
!endif
