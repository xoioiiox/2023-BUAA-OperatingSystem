#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int hangup = 0;
int shellid = 0;
/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if(*s == '"') {
		*s++ = 0;
		*p1 = s;
		while (*s && *s != '"') {
			s++;
		}
		if (*s == 0) {
			debugf("\"dismatch\n");
			return 0;
		}
		*s = 0;
		*p2 = s;
		return 'w';
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

int is_name_equal(char *s1, char *s2) {
	int i;
 	for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++) {
		if (s1[i] != s2[i]) {
			return 0;
		}
	}
	if (s1[i] != '\0' || s2[i] != '\0') {
		return 0;
	}
	return 1;
}

int is_value_cmd(char *name) {
	char *dec = "declare.b";
	char *uns = "unset.b";
	char *ech = "echo.b";
	char *dec1 = "declare";
	char *uns1 = "unset";
	char *ech1 = "echo";
	if (is_name_equal(dec, name) || is_name_equal(uns, name) || is_name_equal(ech, name)) {
		return 1;
	}
	if (is_name_equal(dec1, name) || is_name_equal(uns1, name) || is_name_equal(ech1, name)) {
		return 1;
	}
	return 0;
}

#define MAXARGS 128
int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int tmp, tmp1;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original fd.
			/* Exercise 6.5: Your code here. (1/3) */
			r = open(t, O_RDONLY);
			fd = r;
			dup(fd, 0);
			close(fd);
			//user_panic("< redirection not implemented");

			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, dup it onto fd 1, and then close the original fd.
			/* Exercise 6.5: Your code here. (2/3) */
			r = open(t, O_WRONLY | O_CREAT);
			fd = r;
			dup(fd, 1);
			close(fd);
			//user_panic("> redirection not implemented");

			break;
		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of the
			 *   command line.
			 * The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			pipe(p);
			if ((*rightpipe = fork()) == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe); 
			}
			else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				return argc;
			}
			//user_panic("| not implemented");
			break;
		case ';':;
			tmp = fork();
			if (tmp) {
				wait(tmp);
				return parsecmd(argv, rightpipe); 
			} 
			else {
				return argc;
			}
			break;
		case '&':;
			tmp1 = fork();
			if (tmp1) {
				return parsecmd(argv, rightpipe); 
			} 
			else {
				hangup = 1;
				return argc;
			}
			break;
		}
	}

	return argc;
}


void runcmd(char *s) {
	gettoken(s, 0);
	hangup = 0;
	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	if (is_value_cmd(argv[0])) {
		char id[10];
		//debugf("cid:%d\n", shellid);
		int tmp = shellid;
		int pos = 0;
		//memset(id, 0, sizeof(id));
		while (tmp != 0) { //turn int into char[]
			id[pos++] = '0' + (tmp % 10);
			tmp /= 10;
		}
		id[pos] = '\0';
		argv[argc] = id;
		argc++;
		argv[argc] = NULL;
	}

	char name[64];
	int child = spawn(argv[0], argv);
	if (child < 0) {
		int len = strlen(argv[0]);
		strcpy(name, argv[0]);
		name[len] = '.';
		name[len + 1] = 'b';
		name[len + 2] = 0;
		child = spawn(name, argv);
	}

	close_all();
	if (child >= 0) {
		if (!hangup) {
			debugf("\033[31mwaiting\033[m\n");
			wait(child);
			debugf("\033[31mwaiting end\033[m\n");
		}
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit();
}

char insert[1024];
void readline(char *buf, u_int n) {
	int r, pos = 0;
	for (int i = 0; i < n; i++) {
		if ((r = read(0, buf + i, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		//debugf("buf:%s\n", buf);
		again:;
		if (buf[i] == '\b' || buf[i] == 0x7f) {
			buf[i] = '\0';
			int l = strlen(buf);
			for (int i = l - pos - 1; i < l - 1; i++) {
				buf[i] = buf[i + 1];
			}
			buf[l - 1] = '\0';
			i-=2;
			if (buf[i] != '\b') {
				printf("\b");
			}
		}
		if (buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
			return;
		}
		if (buf[i] == 27) {
			char c1, c2;
			read(0, &c1, 1);
			read(0, &c2, 1);
			if (c1 == 91 && c2 == 65) { //up
				debugf("\n");
                i = history_get(buf, 0);
				debugf("%s\n", buf);
			} 
			else if (c1 == 91 && c2 == 66) { //down
				debugf("\n");
				i = history_get(buf, 1);
				debugf("%s\n", buf);
			}
			else if (c1 == 91 && c2 == 67) { //Right
				pos--;
				read(0, buf + i, 1);
				if (buf[i] == 27 || buf[i] == 0x7f) {
					goto again;
				}
				else {
					insert[0] = buf[i];
					buf[i] = '\0';
					str_insert(buf, i - pos);
					for (int j = 0; j + i < n; j++) {
						read(0, insert + j, 1);
						if (insert[j] == 27 || insert[j] == 0x7f) {
							char t = insert[j];
							insert[j] = '\0';
							str_insert(buf, i - pos);
							i++;
							buf[i] = t;
							goto again;
						}
						if (insert[j] == '\r' || insert[j] == '\n') {
							insert[j] = '\0';
							str_insert(buf, i - pos);
							i = strlen(buf) - 1;
							debugf("%s\n", buf);
							return;
						}
					}
				}
			}
			else if (c1 == 91 && c2 == 68) { //Left
				pos++; //相对末尾距离
				read(0, buf + i, 1);
				if (buf[i] == 27 || buf[i] == 0x7f) {
					goto again;
				}
				else {
					insert[0] = buf[i];
					buf[i] = '\0';
					str_insert(buf, i - pos);
					for (int j = 0; j + i < n; j++) {
						read(0, insert + j, 1);
						if (insert[j] == 27 || insert[j] == 0x7f) {
							char t = insert[j];
							insert[j] = '\0';
							str_insert(buf, i - pos);
							i++;
							buf[i] = t;
							goto again;
						}
						if (insert[j] == '\r' || insert[j] == '\n') {
							insert[j] = '\0';
							str_insert(buf, i - pos);
							i = strlen(buf) - 1;
							debugf("%s\n", buf);
							return;
						}
					}
				}
			}
		}
	}
	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

int offset;	//-1 : last cmd
int history_get(char *buf, int type) {	//type 0 : up
	if (type == 0) {
		offset--;
	} 
	else if (type == 1 && offset < 0) {
		offset++;
	}
	int fdnum = open(".history", O_RDONLY);
	if (fdnum < 0) {
		debugf("Error : open .history failed\n");
		return 0;
	}
	struct Fd *fd = num2fd(fdnum);
	char *c;
	char *begin = fd2data(fd);
	char *end = begin + ((struct Filefd*)fd)->f_file.f_size;
	c = end - 1;
	while((*c == '\n' || *c == 0) && c > begin) {
		c--;
	}
	if (c == begin) {	//no history cmd
		buf[0] = '\0';
		return 0;
	}
	c++;	//last \n or \0
	int i;
	for (i = 0; i > offset; i--) {
		while(*c != '\n' && *c != '\0') { //get target line
			c--;
			if (c <= begin) {
				break;
			}
		}
		c--; //remove the '\n'
		if (c <= begin) {
			offset = i;
			break;
		}
	} //now c is the end of target line
	if (c > begin) {
		while(c > begin && *c != '\n') {
			c--;
		}
		if (*c == '\n') {
			c++;
		}
	} 
	else { //out of range
		c = begin;
	}
	int p = 0;
    while(buf[p] != '\0') {
        buf[p] = '\0';
        p++;
    }
    p = 0;
	while (*c != '\n' && *c != '\0' && *c < end) {
		buf[p] = *c;
		p++;
		c++;
	}
	return p - 1;
}

void str_insert(char *buf, int pos) {
	int i = 0;
	int len = strlen(buf);
	int l1 = strlen(insert);
	for (i = len - 1 + l1; i >= pos + l1; i--) {
		buf[i] = buf[i - l1];
	}
	for (i = pos; i < pos + l1; i++) {
		buf[i] = insert[i - pos];
		insert[i - pos] = '\0';
	}
	len = strlen(buf);
	buf[len] = '\0';
}

char buf[1024];

void usage(void) {
	debugf("usage: sh [-dix] [command-file]\n");
	exit();
}

void history_save(char* cmd) {
	int r = open(".history", O_CREAT | O_WRONLY | O_APPEND);
	if (r < 0) {
		debugf("open .history failed! in save\n");
		return r;
	}
	write(r, cmd, strlen(cmd));
	write(r, "\n", 1);
	return 0;
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	debugf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	debugf("::                                                         ::\n");
	debugf("::                     MOS Shell 2023                      ::\n");
	debugf("::                                                         ::\n");
	debugf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

	shellid = syscall_create_shellid();
	debugf("--------This is shell %d--------\n", shellid);

	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[1], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			printf("\n$ ");
		}
		memset(buf, 0, sizeof(buf));
		readline(buf, sizeof buf);
		history_save(buf);

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit();
		} else {
			wait(r);
		}
	}
	return 0;
}
