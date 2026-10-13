EE_BIN = lojaps2.elf
EE_OBJS = main.o

GSKIT = $(PS2DEV)/gsKit
EE_INCS := -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(GSKIT)/include
EE_LDFLAGS := -L$(PS2SDK)/ee/lib -L$(GSKIT)/lib

# Aqui estão todas as bibliotecas juntas na ordem certa!
EE_LIBS = -ldebug -lpad -lgsfontm -lfreetype -lz -lgskit -ldmakit -lm

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal