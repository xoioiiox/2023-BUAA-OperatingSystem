#include <lib.h>

void tree(char *path, int type) {
    if (type) {
		debugf(".\n");
	}
	else {
		debugf("%s\n", path);
	}
    buildtree(path, 0);
}

void buildtree(char *path, int depth) {
    int fd;
    struct File f;
    struct File *dir;
    char newpath[55] = {0};
    if ((fd = open(path, O_RDONLY)) < 0) {
		debugf("Open error\n");
		return;
	}
    while (readn(fd, &f, sizeof(f)) == sizeof(f)) {
        if (f.f_name[0]) {
            dir = &f;
            int i;
            for (i = 0; i < depth; i++) {
				debugf("    ");
			}
            debugf("|-- ");
            if (dir->f_type == FTYPE_REG) {
				debugf("\033[32m%s\033[m\n", dir->f_name); //green
			}
            else {
				debugf("\033[34m%s\033[m\n", dir->f_name); //blue
			}
            if (dir->f_type == FTYPE_DIR) {
                strcpy(newpath, path);
				int len = strlen(newpath);
				newpath[len] = '/';
				int j;
				for (j = 0; j < strlen(f.f_name); j++) {
					newpath[len + 1 + j] = f.f_name[j];
				}
                newpath[len + 1 + j] = '\0';
                buildtree(newpath, depth + 1);
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        tree("/", 1);
    } 
	else if (argc == 2) {
        tree(argv[0], 0);  
    } 
	else {
        debugf("Num of arg error\n");
    }
}