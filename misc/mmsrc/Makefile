BIN		:= memory
WARN		:= 
OBJS		:= main.o infodump.o shared.o pam.o pfh.o arh.o sysmem.o
CFLAGS	:= -g $(WARN) -DDEBUG -I. 
CC       := gcc

all 		: $(BIN)

$(BIN) 	: $(OBJS)
			$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(MODS) -lpthread

.PHONY 	: clean
clean 	: 
			rm -f $(BIN) $(OBJS) *~

backup	:
			scp * eos@eos-india.net:~/www/abhisek/codez/misc/mmsimu/
