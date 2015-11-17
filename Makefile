default: poly basic

OBJ = poly.o
CFLAGS += -g 
LIBS += -lsoundpipe -lm -lsndfile

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

poly: main.c poly.o
	gcc $(CFLAGS) $(LIBS) $< -o $@ poly.o

basic: basic.c poly.o
	gcc $< $(CFLAGS) $(LIBS) -o $@  poly.o

clean:
	rm -rf $(OBJ) poly basic
