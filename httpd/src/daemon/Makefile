include ../flags.mk

MODULE = daemon
OBJ = $(MODULE).o
LIB = lib$(MODULE).a

LIBS = config/libconfig.a server/libserver.a parse_simple/libparse_simple.a

LDLIBS = -lcriterion

TEST_OBJ = tests/$(MODULE)_test.o
TEST_BIN = tests/$(MODULE)_test

$(MODULE): $(LIB)($(OBJ))

check: $(TEST_OBJ) $(MODULE) $(LIBS)
	$(CC) $< -o $(TEST_BIN) -Wl,--start-group $(LIB) $(addprefix $(SRC_DIR), $(LIBS)) -Wl,--end-group $(LDLIBS)
	$(TEST_BIN)

include ../libs.mk

clean:
	$(RM) $(OBJ) $(LIB) $(TEST_BIN) $(TEST_OBJ) $(addprefix $(SRC_DIR), $(LIBS))

.PHONY: all clean $(MODULE)
