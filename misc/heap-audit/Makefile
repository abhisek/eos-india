ROOT	:= $(PWD)

CC			:= gcc
CFLAGS	:= -DDEBUG -I$(ROOT) -ggdb -O2 -pipe -fPIC
LDFLAGS	:= -shared -ldl
MODS		:= heaplib.o hash.o text_log.o audit.o dfree_audit.o

all: $(MODS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o heaplib.so $(MODS)

.PHONY: clean
clean:	
	-rm *.o
	-rm heaplib.so
