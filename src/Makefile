DESTDIR?=""

INCLUDE=
LIBS_UDEV= `pkg-config libudev --libs` -lrt
LIBS= -lusb-1.0 $(LIBS_UDEV)

ut612_OBJS= main.o UT612ByteStreamParser.o UT612ByteSource.o ByteStreamToFile.o hidapi/hid.o
ut612_LIBS= $(LIBS)

unittest_OBJS= unittests/test.o UT612ByteStreamParser.o UT612ByteSource.o ByteStreamToFile.o hidapi/hid.o 
unittest_LIBS= $(LIBS) -lboost_unit_test_framework

EXECS= ut612 unittest
EXEC_installed= ut612

COMPILER_FLAGS+= -Wall -O3 -std=c++0x

ut612: $(ut612_OBJS) $(wildcard *.h) $(wildcard *.hpp) Makefile
	$(CXX) $(COMPILER_FLAGS) -o $@ $($@_OBJS) $($@_LIBS)

unittest: $(unittest_OBJS) $(wildcard *.h) $(wildcard *.hpp) Makefile
	$(CXX) $(COMPILER_FLAGS) -o $@ $($@_OBJS) $($@_LIBS)

%.o:	%.cpp
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

all: ut612 unittest

.PHONY: test
test : ut612 unittest
	cd unittests && ./shell_tests.sh
	./unittest


#
# Code for adding / removing blacklisting of a kernel module which incorrectly tries
# to use this chip as a regular uart. That module always fails and is not supposed to do that.
#
CP210X_BLACKLIST_DESTFILE=$(DESTDIR)/etc/modprobe.d/blacklist-cp210x.conf
CP210X_BLACKLIST_LINE1='\# Blacklisting UT612 LCR Meter (and all CP210x UART Bridges)'
CP210X_BLACKLIST_LINE2='alias usb:v10C4pEA80d*dc*dsc*dp*ic*isc*ip*in* off'
CP210X_BLACKLIST_LINE2_SED_FRIENDLY='alias usb:v10C4pEA80d\*dc\*dsc\*dp\*ic\*isc\*ip\*in\* off'

define removeBlacklist
	@if [ -e "$(CP210X_BLACKLIST_DESTFILE)" ] ; then \
		mv "$(CP210X_BLACKLIST_DESTFILE)" "$(CP210X_BLACKLIST_DESTFILE).bak_ut612" ; \
		cat "$(CP210X_BLACKLIST_DESTFILE).bak_ut612" | \
		tr '\n' '\0' | \
		sed -e 's/'$(CP210X_BLACKLIST_LINE1)'\x00'$(CP210X_BLACKLIST_LINE2_SED_FRIENDLY)'\x00//' \
		| tr '\0' '\n' > "$(CP210X_BLACKLIST_DESTFILE)" && \
		rm "$(CP210X_BLACKLIST_DESTFILE).bak_ut612" ; \
	else \
		true ; \
	fi
endef

define addBlacklist
	$(call removeBlacklist)
	@echo $(CP210X_BLACKLIST_LINE1) >> "$(CP210X_BLACKLIST_DESTFILE)"
	@echo $(CP210X_BLACKLIST_LINE2) >> "$(CP210X_BLACKLIST_DESTFILE)"
endef


.PHONY: install
install: $(EXEC_installed)
	install $(EXEC_installed) $(DESTDIR)/usr/local/bin
	install -m 644 70-ut612.rules $(DESTDIR)/etc/udev/rules.d/
	$(call addBlacklist)
	@echo "added blacklisting of the UT612 LCR meter to $(CP210X_BLACKLIST_DESTFILE)"

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)/usr/local/bin/$(EXEC_installed)
	rm $(DESTDIR)/etc/udev/rules.d/70-ut612.rules
	$(call removeBlacklist)
	@echo "removed blacklisting of the UT612 LCR meter from $(CP210X_BLACKLIST_DESTFILE)" ; \

.PHONY: prepare
prepare:
	apt-get install libudev-dev sed pkg-config build-essential libusb-1.0-0-dev

.PHONY: prepare-all
prepare-all: prepare
	apt-get install libboost-test-dev

.PHONY: clean
clean:
	rm -f $(EXECS) $(ut612_OBJS) $(unittest_OBJS)
