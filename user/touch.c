#include <lib.h>

int main(int argc, char **argv) {
	if (argc == 2) {
		if (open(argv[1], O_CREAT) < 0) {
			debugf("Touch failed\n");
		} 
		else {
            debugf("Success touch!\n");
        }
	} 
	else {
		debugf("usage: touch [filename]\n");
	}	
}