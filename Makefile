CC      ?= clang
CFLAGS  ?= -Os -flto -fdata-sections -ffunction-sections -Wall -Wextra -pedantic -fvisibility=hidden
LDFLAGS ?= -Wl,-dead_strip
BINDIR  ?= $(HOME)/bin
VERSION := $(shell cat VERSION 2>/dev/null || echo 0.0.0)

TARGET := counter
SRC    := counter.c

all: $(TARGET)

$(TARGET): $(SRC) VERSION
	$(CC) $(CFLAGS) -DCOUNTER_VERSION=\"$(VERSION)\" $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

install: $(TARGET)
	mkdir -p "$(BINDIR)"
	cp -f $(TARGET) "$(BINDIR)/$(TARGET)"
	-strip -x "$(BINDIR)/$(TARGET)" 2>/dev/null || true
	@echo "Installed to $(BINDIR)/$(TARGET)"

uninstall:
	rm -f "$(BINDIR)/$(TARGET)"

.PHONY: all clean install uninstall

version:
	@echo $(VERSION)
