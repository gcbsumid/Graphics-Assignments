SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LDFLAGS = -lGL -lGLU -lGLEW -lassimp -lSDL -lSDL_mixer 
# CXXFLAGS = -g -W -Wall -MMD -std=c++0x -fcolor-diagnostics -ferror-limit=5
CXXFLAGS = -g -W -Wall -MMD -std=c++0x 
CXXFLAGS += $(DEBUGFLAGS)
CXX = g++
# CXX = clang++

MAIN = test 

all: $(MAIN)

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) 

%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	 set -e; $(CC) -M $(CXXFLAGS) $< \
        | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
        [ -s $@ ] || rm -f $@

include $(DEPENDS)

