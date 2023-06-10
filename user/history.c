#include <lib.h>

int main(int argc, char** argv) {
	if (argc != 1) {
		debugf("num of arg error\n");
	}
	int fdnum = open(".history", O_RDONLY);
	if (fdnum < 0) {
		debugf("open .history failed\n");
		return;
	}
	
	char buf;
	int r, cnt = 0, newline = 1;
	while (r = read(fdnum, &buf, 1)) {
		if (newline) {
            debugf("History %d : %c", cnt, buf);
			cnt++;
			newline = 0;
		} 
		else {
			debugf("%c", buf);
		}
		if (buf == '\n') {
			newline = 1;
		}
	}
}