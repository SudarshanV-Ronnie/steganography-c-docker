OBJ:=$(patsubst %.c,%.o,$(wildcard *.c))
stego.exe:$(OBJ)
	gcc -o $@ $^
%.o:%.c
	gcc -c $<
clean:
	rm *.exe *.o