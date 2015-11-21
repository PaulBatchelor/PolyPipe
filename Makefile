default: make_file sines str2poly

OBJ = poly.o 
CFLAGS += -g 
LIBS += -lsoundpipe -lm -lsndfile

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

make_file: main.c poly.o
	gcc $(CFLAGS) $(LIBS) $< -o $@ poly.o

sines: sines.c poly.o 
	gcc $< $(CFLAGS) $(LIBS) -o $@  poly.o 

str2poly: str2poly.go
	go build -o $@ $<

clean:
	rm -rf $(OBJ) make_file sines *.bin *.wav str2poly 
