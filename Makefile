# lml - lisp markup language
# See LICENSE file for copyright and license details.
.POSIX:

# Customize below to fit your system

# paths
PREFIX = /usr/local

lml: lml.c
	$(CC) -o $@ $<

clean:
	rm -f lml

install: lml
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f lml $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/lml

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/lml

.PHONY: clean install uninstall
