#include <blib.h>

size_t strlen(const char *s) {
	int len = 0;
	while (*s != '\0') {
		len++;
		s++;
	}
	return len;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while (*src != '\0') {
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while (*dst) {
		dst++;
	}
	while (*src) {
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 != '\0') {
		if (*s2 =='\0') {
			return 1;
		}
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		s1++;
		s2++;
	}
	if (*s2 != '\0') {
		return -1;
	}
	else {
		return 0;
	}
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	char *res = (char *)s;
	while (n--) {
		*res = c;
		res++;
	}
	return s;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	char *cs1 = (char *)s1;
	char *cs2 = (char *)s2;
	while (n--) {
		if (*cs1 != *cs2) {
			return *cs1 - *cs2;
		}
		cs1++;
		cs2++;
	}
	return 0;
}
