#include <lib.h>

int main(int argc, char **argv) {
	if (argc > 5) {
		debugf("Declare arg error");
		return;
	}

	int i, shellid = 0;
	int len = strlen(argv[argc-1]);
	for (i = len - 1; i >= 0; i--) {
		shellid = shellid * 10 + argv[argc-1][i] - '0';
	}
	//debugf("shellid:%d\n", shellid);

	if (argc == 2) {
		syscall_print_value(NULL, shellid);
		return;
	}

	int value = 0, xr = 0, rdonly = 0;
	if (argc > 3 && argv[1][0] == '-') {
		xr = 1;
		if (argv[1][1] == 'x' || argv[1][2] == 'x') {
			shellid = 0;
		} 
		if (argv[1][1] == 'r' || argv[1][2] == 'r') {
			rdonly = 1;
		}
	}

	i = 0;
	char name[32] = {'\0'};
	while(argv[xr + 1][i] != '\0') {
		name[i] = argv[xr + 1][i];
		i++;
	}
	name[i] = '\0';

	if (argc == xr + 4) { //has =value
		i = 1;
		while(argv[xr + 2][i] != '\0') {
			value = value * 10 + argv[xr + 2][i] - '0';
			i++;
		}
	}
    //debugf("name: %s", name);
    //debugf("rd: %d\n", rdonly);
	syscall_declare_value(name, value, shellid, rdonly);
}