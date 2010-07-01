CC		=	gcc
CFLAGS	=	-g -std=gnu99 -DHAVE_CONFIG_H -c
INCS	=	#-I../ -I../confparser/
LDS		=	#-L../ -L../confparser/ #-lserver -lconfparser

OBJ		=	mod.o

OBJ:
	#$(CC) $(CFLAGS) $(INCS) $(LDS) -o $(OBJ) mod.c 
	$(CC) $(CFLAGS) -o $(OBJ) mod.c


clean:
	rm -f *.o
	rm -f $(OBJ)

