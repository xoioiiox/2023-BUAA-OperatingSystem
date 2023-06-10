#include <lib.h>

int main(int argc, char **argv) {
	int i, nflag;

	nflag = 0;
	if (argc > 1 && strcmp(argv[1], "-n") == 0) {
		nflag = 1;
		argc--;
		argv++;
	}

	int shellid = 0;
	int len = strlen(argv[argc-1]);
	for (i = len - 1; i >= 0; i--) {
		shellid = shellid * 10 + argv[argc-1][i] - '0';
	}

	for (i = 1; i < argc - 1; i++) {
		if (i > 1) {
			printf(" ");
		}
		if (argv[i][0] == '$') {
			int value = syscall_get_value(&argv[i][1], shellid);
		    printf("[VALUE] %s : %d ", &argv[i][1], value);
		}
		else {
			printf("%s", argv[i]);
		}
	}
	if (!nflag) {
		printf("\n");
	}
	return 0;
}
