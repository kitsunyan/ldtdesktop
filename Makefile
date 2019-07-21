CC = gcc
CFLAGS = 
LDFLAGS =

DESTDIR =
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib

.PHONY: all clean install

all: libldtdesktop.so

libldtdesktop.so: ldtdesktop.cpp
	$(CC) $< -o $@ \
	$(CFLAGS) -I/usr/include/qt5 -D_GNU_SOURCE \
	$(LDFLAGS) -ldl -fPIC -shared

clean:
	rm -rfv libldtdesktop.so

install: all
	install -Dm644 libldtdesktop.so $(DESTDIR)$(LIBDIR)/libldtdesktop.so
