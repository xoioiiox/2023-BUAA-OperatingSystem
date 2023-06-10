#include <lib.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		debugf("Arguments num Error\n");
		return;
	}

	int shellid = 0, i = 0;
	int len = strlen(argv[argc-1]);
	for (i = len - 1; i >= 0; i--) {
		shellid = shellid * 10 + argv[argc-1][i] - '0';
	}
	
	char name[32] = {0};
	i = 0;
	while (argv[1][i] != '\0') {
		name[i] = argv[1][i];
		i++;
	}
	name[i] = '\0';
	syscall_unset_value(name, shellid);
}
