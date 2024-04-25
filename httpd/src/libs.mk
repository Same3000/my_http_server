SRC_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

config/libconfig.a:
	$(MAKE) -C $(SRC_DIR)config
server/libserver.a:
	$(MAKE) -C $(SRC_DIR)server
daemon/libdaemon.a:
	$(MAKE) -C $(SRC_DIR)daemon

utils/libutils.a:
	$(MAKE) -C $(SRC_DIR)utils

parse_simple/libparse_simple.a:
	$(MAKE) -C $(SRC_DIR)parse_simple
http/libhttp.a:
	$(MAKE) -C $(SRC_DIR)http
logger/liblogger.a:	
	$(MAKE) -C $(SRC_DIR)logger
