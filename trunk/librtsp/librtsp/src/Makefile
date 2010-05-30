SOURCES = FileSource.cpp \
	InetAddr.cpp \
	librtsp.cpp \
	ListenSocket.cpp \
	MediaSource.cpp \
	MP3FileSource.cpp \
	MP3MediaSubSession.cpp \
	MPEG4FileSource.cpp \
	MPEG4MediaSubSession.cpp \
	rtsperrors.cpp \
	RTSPListener.cpp \
	RTSPPacketBuilder.cpp \
	RTSPPacket.cpp \
	RTSPServer.cpp \
	RTSPSessionBase.cpp \
	RTSPSession.cpp \
	ServerMediasession.cpp \
	ServerMediaSubsession.cpp \
	SocketBase.cpp \
	TCPSocket.cpp \
	UDPSocket.cpp \
	H264FileSource.cpp \
	H264MediaSubSession.cpp \
	G729DFileSource.cpp \
	G729DMediaSubSession.cpp \
	util.cpp \
	ffmpegutil.cpp \
	DemuxSource.cpp \
	H264DemuxSource.cpp \
	MP2DemuxSource.cpp \
	MPEG4DemuxSource.cpp \
	MediaDemux.cpp \
	FileDemux.cpp
	
    
INCLUDES = -I../inc/ -I../../contrib/inc/ -I../../contrib/inc/ffmpeg
LIBS = #-lpthread
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))
DISTOBJS = $(foreach file, $(OBJS), objs/$(file))
CCFLAGS = $(INCLUDES) $(LIBS) -D_DEBUG
OUT=../librtsp.a

all: $(OUT) 
#	-@mkdir objs
#	@echo $(OBJS)
#	@echo $(DISTOBJS)

$(OUT): $(OBJS)
	ar rcs $(OUT) $(OBJS)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CCFLAGS) $< -o $@	
	

clean:
	-rm -rf objs
	-rm *.o
static:
	echo "xxx"
