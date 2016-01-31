DESTDIR?=""

INCLUDE=
LIBS_UDEV= `pkg-config libudev --libs` -lrt
LIBS= -lusb-1.0 $(LIBS_UDEV)

ut612_OBJS= main.o UT612ByteStreamParser.o UT612ByteSource.o ByteStreamToFile.o hidapi/hid.o
ut612_LIBS= $(LIBS)
unittest_OBJS= unittests/test.o UT612ByteStreamParser.o UT612ByteSource.o ByteStreamToFile.o hidapi/hid.o 
unittest_LIBS= $(LIBS) -lboost_unit_test_framework

EXECS= ut612 unittest

COMPILER_FLAGS+= -Wall -O3 -std=c++0x

all:	$(EXECS)

$(EXECS): $(ut612_OBJS) $(unittest_OBJS) $(wildcard *.h) $(wildcard *.hpp)
	$(CXX) $(COMPILER_FLAGS) -o $@ $($@_OBJS) $($@_LIBS)

%.o:	%.cpp
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)



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
	rm -f $(EXECS) $(ut612_OBJS) $(unittest_OBJS)
