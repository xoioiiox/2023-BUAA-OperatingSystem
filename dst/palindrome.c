#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);
	int i = 0;
	int flag = 0;
	char s[10];
	while(n != 0) {
		s[i] = n%10;
		i++;
		n/=10;
	}
	for(int j = 0; j < i; j++) {
		if (s[j] != s[i - j - 1]) {
			flag = 1;
			break;
		}
	}
	if (!flag) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
