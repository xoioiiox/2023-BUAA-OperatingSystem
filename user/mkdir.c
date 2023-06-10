#include <lib.h>

int main(int argc, char **argv) {
	if (argc == 2) {
		if (open(argv[1], O_MKDIR) < 0) {
			debugf("mkdir failed\n");
		} 
		else {
            debugf("Success mkdir!\n");
        }
	}
	else {
		debugf("Usage: mkdir [dirname]\n");
	}
}