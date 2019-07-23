CC = gcc
CFLAGS = 
LDFLAGS =

DESTDIR =
PREFIX = /usr/local
LIBDIR = $(PREFIX)/lib

ifneq ($(wildcard /usr/include/qt5),)
INCLUDE_QT5 = /usr/include/qt5
else
INCLUDE_QT5 = /usr/include/qt
endif

.PHONY: all clean install

all: libldtdesktop.so

libldtdesktop.so: ldtdesktop.cpp
	$(CC) $< -o $@ \
	$(CFLAGS) -I$(INCLUDE_QT5) -D_GNU_SOURCE \
	$(LDFLAGS) -ldl -fPIC -shared

clean:
	rm -rfv libldtdesktop.so

install: all
	install -Dm644 libldtdesktop.so $(DESTDIR)$(LIBDIR)/libldtdesktop.so
