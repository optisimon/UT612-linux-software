DESTDIR?=""

INCLUDE=
LIBS=

OBJS= main.o

EXEC= ut612

COMPILER_FLAGS+= -Wall -O3

$(EXEC): $(OBJS) $(wildcard *.h)
	$(CXX) $(COMPILER_FLAGS) -o $(EXEC) $(OBJS) $(LIBS)

%.o:	%.cpp
	$(CXX) -c $(COMPILER_FLAGS) -o $@ $< $(INCLUDE)

all:	$(EXEC)

.PHONY: test
test : all
	cd unittests && ./shell_tests.sh

install: $(EXEC)
	install $(EXEC) "$(DESTDIR)"/usr/bin
	
uninstall:
	rm "$(DESTDIR)"/usr/bin/$(EXEC)

clean:
	rm -f $(EXEC) $(OBJS)
