#include <value.h>
#include <printk.h>

struct valueset {
	char name[32];
	int value;
	int shellid;	//0:global
	int rdonly;		//1:read_only
	int unset;	//0:unset
} values[256];

int cur_valuesum = 0;
int cur_id = 1;

int create_shellid() {
	int cur = cur_id;
	cur_id++;
	return cur;
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

int declare_value(char *name, int value, int shellid, int rdonly) {
	int i;
	for (i = 0; i < cur_valuesum; i++) { //check deassign first
		if (values[i].unset) {
			continue;
		}
		//printk("%d %d\n", shellid, values[i].shellid);
		if (values[i].shellid == shellid || values[i].shellid == 0) {
			if (is_name_equal(values[i].name, name)) {
				if (values[i].rdonly == 1) { //check read_only
					printk("Error : Can't deassign read_only value\n");
					return 0;
				} 
				else if (values[i].shellid == shellid) {
					values[i].value = value;
					values[i].rdonly = rdonly;
					printk("Success deassign!\n");
					return 0;
				}
			}
		}
	}
	//printk("in-name : %s\n", name);
	for (i = 0; name[i] != '\0'; i++) {
		values[cur_valuesum].name[i] = name[i];
	}
	values[cur_valuesum].name[i] = '\0';
	values[cur_valuesum].value = value;
	values[cur_valuesum].shellid = shellid;
	values[cur_valuesum].rdonly = rdonly;
	values[cur_valuesum].unset = 0;
	cur_valuesum++;
	printk("Success create!\n");
	return 0;
}

int unset_value(char *name, int shellid) {
	for (int i = 0; i < cur_valuesum; i++) {
		if (values[i].unset) {
			continue;
		}
		if (values[i].shellid == shellid || values[i].shellid == 0) {
			if (is_name_equal(values[i].name, name)) {
				if (values[i].rdonly == 1) {
					printk("Error : Can't unset readonly value\n");
					return 0;
				}
				values[i].unset = 1;
				printk("Success unset!\n");
			}
		}
	} 
	return 0;
}

int get_value(char *name, int shellid) {
	for (int i = 0; i < cur_valuesum; i++) {
		if (!values[i].unset && (values[i].shellid == shellid || values[i].shellid == 0)) {
			if (is_name_equal(name, values[i].name)) {
				return values[i].value;
			}
		}
	}
	return -1;
}

int print_value (char *name, int shellid) {
	for (int i = 0; i < cur_valuesum; i++) {
		if (!values[i].unset && (values[i].shellid == shellid || values[i].shellid == 0)) {
			printk("[VALUE] Name : %s | ", values[i].name);
			printk("Value : %d | ", values[i].value);
			if (values[i].shellid) {
				printk("Visibility : LOCAL | ");
			} 
			else {
				printk("Visibility : GLOBAL | ");
			}
			if (values[i].rdonly) {
				printk("ReadOnly : TRUE\n");
			} 
			else {
				printk("ReadOnly : FALSE\n");
			}
		}
	}
	return 0;
}

