.PHONY: clean

out: calc case_all
	./calc < case_all > out

# Your code here.
casegen: casegen.c
	gcc -o casegen casegen.c
calc: calc.c
	gcc -o calc calc.c

case_add: casegen
	./casegen 100 add 	
case_sub: casegen
	./casegen 100 sub
case_mul: casegen
	./casegen 100 mul
case_div: casegen
	./casegen 100 div
case_all: 
	cat case_add case_sub case_mul case_div > case_all


clean:
	rm -f out calc casegen case_*
