TARGET = QS-term

SRC = qs-term.c

CFLAGS = `pkg-config --cflags gtk+-3.0 vte-2.91`

LIBS = `pkg-config --libs gtk+-3.0 vte-2.91`

PREFIX = /usr/local

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

.PHONY: all clean install uninstall
