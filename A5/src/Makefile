SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
# LIBDIR = -L/usr/lib
LDFLAGS = -lGL -lGLU -lGLEW -lglfw -lassimp -lSDL -lSDL_mixer -Istb_image -lMagick++
MAGICK = `Magick++-config --cppflags --cxxflags --ldflags --libs`
# CXXFLAGS = -g -W -Wall -MMD -std=c++0x -fcolor-diagnostics -ferror-limit=5
CXXFLAGS = -g -Wall -MMD -std=c++0x
CXXFLAGS += $(DEBUGFLAGS)
CXX = g++
# CXX = clang++

MAIN = slenderman 

all: $(MAIN)

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) # $(LIB_DIR)

%.o: %.cpp
	$(CXX) $(MAGICK) -o $@ -c $(CXXFLAGS)$<

%.d: %.cpp
	 set -e; $(CC) -M $(MAGICK) $(CXXFLAGS) $< \
        | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
        [ -s $@ ] || rm -f $@

include $(DEPENDS)

