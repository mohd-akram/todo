all: todo

todo: src/main.c src/todo.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c src/todo.c

prefix = /usr/local
bindir = $(prefix)/bin
mandir = $(prefix)/share/man

-include config.mk

install: todo
	install -d $(DESTDIR)$(bindir)
	install todo $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(mandir)/man1
	install man/todo.1 $(DESTDIR)$(mandir)/man1
