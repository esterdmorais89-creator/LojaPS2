EE_BIN = lojaps2.elf
EE_OBJS = main.o
EE_LIBS = -ldebug -lpad

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal