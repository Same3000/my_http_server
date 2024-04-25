include flags.mk

OBJ = main.o

LIBS = config/libconfig.a daemon/libdaemon.a server/libserver.a utils/libutils.a parse_simple/libparse_simple.a http/libhttp.a logger/liblogger.a

all: httpd

httpd: $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group

debug: $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) -g3 -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group
	#pytest
	

include libs.mk

.PHONY: all

clean:
	${RM} ${OBJ}	
	${RM} ${LIBS}
	${RM} httpd
	${RM} debug
	${RM} server.log
	${RM} test.log
