CC 	=	gcc
CFLAGS	=	-Wextra -g -Wall -L/usr/local/lib

LINK	=	-lev -lpthread

OBJS	=	main.o

.c.o:
	$(CC) -c $*.c $(CFLAGS)

main: $(OBJS)
	$(CC) -o main $(OBJS) $(LINK)

clean:
	rm -f *.o *.swp main