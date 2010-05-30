all: rtsp test

rtsp:
	cd librtsp && $(MAKE)

test:
	$(MAKE) -C testprj

clean:
	$(MAKE) -C librtsp clean
	$(MAKE) -C testprj clean
	-rm test
static:
	$(MAKE) -C librtsp static
	$(MAKE) -C testprj static
