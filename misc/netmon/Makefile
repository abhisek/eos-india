ROOT		:= $(PWD)
CC			:= gcc
CFLAGS	:= -DDEBUG -I $(ROOT)/include -ggdb
LDFLAGS	:= -ggdb -lpthread -lpcap

CORE_OBJ	:= src/core/netmon.o src/core/modules.o src/core/debug.o src/core/st_hash.o src/core/ui.o src/core/ui_text.o src/core/conf.o src/core/packet.o src/core/decoder.o src/core/signals.o src/core/utils.o
MOD_OBJ	:= src/core/mod_dummy.o src/core/mod_arp.o
MOD_DECODERS	:= src/decoders/arp.o src/decoders/ipv4.o
MOD_ANALYZERS	:=

all: netmon

netmon: $(CORE_OBJ) $(MOD_OBJ) $(MOD_DECODERS) $(MOD_ANALYZERS)
	$(CC) -o netmon $(CORE_OBJ) $(MOD_OBJ) $(MOD_DECODERS) $(MOD_ANALYZERS) $(LDFLAGS)

.PHONY: clean
clean:
	-rm -rf netmon
	-rm -rf src/core/*.o
	-rm -rf src/decoders/*.o
	-rm -rf src/analyzers/*.o

.PHONY: upload
upload:
	-make clean
	-svn export --force . /tmp/netmon
	-scp -r /tmp/netmon/* eos@eos-india.net:~/www/abhisek/codez/misc/netmon
	-rm -rf /tmp/netmon

