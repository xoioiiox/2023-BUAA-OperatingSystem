all: first
	gcc ./code/fibo.c ./code/main.c -o fibo -I./include/
first: 
	make -C ./code

clean: 
	rm -f ./code/*.o
