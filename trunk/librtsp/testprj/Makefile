


SOURCES    = test.cpp main.cpp
INCLUDES   = -I../librtsp/inc/ -I../contrib/inc -I../contrib/inc/ffmpeg -I.
LIBS       = -L../librtsp -lrtsp -L../contrib/lib -lpcre++ -lpcre
JRTPLIBS   = -ljrtp
FFMPEGLIBS = -lavdevice -lavformat -lavcodec -lavutil -lpthread -lm -lz -lbz2 
OBJS       = $(patsubst %.cpp, %.o, $(SOURCES))
CCFLAGS    = $(INCLUDES) $(LIBS) $(JRTPLIBS) $(FFMPEGLIBS) -D_DEBUG
OUTPUT     = ../test

STATIC_OUTPUT = ../test_g

all: $(OUTPUT)

$(OUTPUT):
	$(CXX) $(SOURCES) -o $(OUTPUT) $(CCFLAGS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CCFLAGS) $< -o $@

clean:
	-rm *.o
static: $(STATIC_OUTPUT)

$(STATIC_OUTPUT):
	$(CXX) $(SOURCES) -o $(STATIC_OUTPUT) $(CCFLAGS) -static
