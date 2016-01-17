DESTDIR?=""

INCLUDE=
LIBS_UDEV= `pkg-config libudev --libs` -lrt
LIBS= -lusb-1.0 $(LIBS_UDEV)

OBJS= main.o UT612ByteStreamParser.o UT612ByteSource.o hidapi/hid.o

EXEC= ut612

COMPILER_FLAGS+= -Wall -O3

$(EXEC): $(OBJS) $(wildcard *.h) $(wildcard *.hpp)
	$(CXX) $(COMPILER_FLAGS) -o $(EXEC) $(OBJS) $(LIBS)

%.o:	%.cpp
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

all:	$(EXEC)

.PHONY: test
test : all
	cd unittests && ./shell_tests.sh

install: $(EXEC)
	install $(EXEC) "$(DESTDIR)"/usr/local/bin
	install -m 644 70-ut612.rules "$(DESTDIR)"/etc/udev/rules.d/

uninstall:
	rm "$(DESTDIR)"/usr/local/bin/$(EXEC)
	rm "$(DESTDIR)"/etc/udev/rules.d/70-ut612.rules

prepare:
	apt-get install libudev-dev

clean:
	rm -f $(EXEC) $(OBJS)
