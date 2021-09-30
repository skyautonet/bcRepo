CC=gcc
CFLAGS=-g -Wall -lm
OBJS=mesh.o cross.o putting.o
TARGET=putting

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

mesh.o: mesh.c

compress:
	tar -cvzf budycaddy.tar.gz mesh.c putting.c cross.c budycaddy.h Makefile attr_0_0_x_387_442_y_194_251_100mm.csv height_0_0_x_387_442_y_194_251_100mm.csv

mesh: mesh.o
	$(CC) -o $@ mesh.c -DRUN_MESH $(CFLAGS)

cross: cross.o
	$(CC) -o $@ cross.c -DRUN_CROSS $(CFLAGS)

clean:
	rm mesh mesh.o putting putting.o cross cross.o budycaddy.tar.gz
   
