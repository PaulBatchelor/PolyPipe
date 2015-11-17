default: make_file sines

OBJ = poly.o 
CFLAGS += -g 
LIBS += -lsoundpipe -lm -lsndfile

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

make_file: main.c poly.o
	gcc $(CFLAGS) $(LIBS) $< -o $@ poly.o

sines: basic.c poly.o 
	gcc $< $(CFLAGS) $(LIBS) -o $@  poly.o 

clean:
	rm -rf $(OBJ) make_file sines *.bin *.wav
