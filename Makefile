all: test.c
	gcc test.c -o test
run: 
	./test
clean:
	rm test
