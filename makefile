RAVEN2DB = raven2db.o parse.o dbglog.o sql.o

ALLOBJ = (RAVEN2DB)


ALLPGM =  raven2db


CC = gcc

IFLAGS = -I /usr/include/mysql -I /usr/include 
CFLAGS = -g -c $(IFLAGS)

LFLAGS = -L /usr/lib64/mysql -L /usr/lib64 
SQLLIBS = -lmysqlclient -lz 

all : $(ALLPGM)


raven2db : $(RAVEN2DB)
	gcc $(LFLAGS) -g -o $@ $(RAVEN2DB) $(SQLLIBS)

%.o:%.c makefile
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean

clean:
	rm -f *.o $(ALLPGM)
