CC=cc #clang
CFLAGS=-g -fPIC -Wall -Wextra -Isrc -Iinclude $(OPTFLAGS)
CXXFLAGS=-g -fPIC -Wall -Wextra -Isrc -Iinclude $(OPTFLAGS)
CPPFLAGS=-DNDEBUG
LDFLAGS=-lreadline -lstdc++
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.cpp src/*.cpp src/*.c)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

TARGET=bin/cutie
TARGET_SOURCES=$(wildcard targets/main.c)

LIB=build/lisp.a
SHARED_LIB=$(patsubst %.a,%.so,$(LIB))

# Default build.
all: $(TARGET) $(SHARED_LIB) tests

rebuild: clean all

# Compiles program 
$(TARGET): LDLIBS += $(LIB)
$(TARGET): $(TARGET_SOURCES) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(TARGET_SOURCES) $(LDFLAGS) $(LIB)

# Compiles shared library
$(SHARED_LIB): $(LIB) $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $(OBJECTS)

# Compiles library
$(LIB): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
TEST_SOURCES=$(wildcard tests/*_tests.cpp)
TESTS=$(patsubst %.cpp,%,$(TEST_SOURCES))

.PHONY: tests
tests: LDLIBS += $(LIB)
tests: $(TESTS) $(LIB)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build bin $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true
