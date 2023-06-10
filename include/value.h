int create_shellid(void);
int declare_value(char* name, int value, int shellid, int type);
int unset_value(char* name, int shellid);
int get_value(char* name, int shellid);
int print_value(char* name, int shellid);