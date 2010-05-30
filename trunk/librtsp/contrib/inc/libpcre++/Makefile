SOURCES = pcre++.cc \
             get.cc \
         replace.cc \
          search.cc \
            split.cc
INCLUDES = -I.
LIBS = #-lpthread
OBJS = $(patsubst %.cc, %.o, $(SOURCES))
DISTOBJS = $(foreach file, $(OBJS), objs/$(file))
CCFLAGS = $(INCLUDES) $(LIBS)
OUT=../libpcre++.a

all: $(OUT) 
#	-@mkdir objs
#	@echo $(OBJS)
#	@echo $(DISTOBJS)

$(OUT): $(OBJS)
	ar rcs $(OUT) $(OBJS)

$(OBJS): %.o: %.cc
	$(CXX) -c $(CCFLAGS) $< -o $@	
	

clean:
	-rm -rf objs
	-rm *.o
static:
	echo "xx"
