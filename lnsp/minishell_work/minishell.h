
char** command_parse(char*);
int run_command(char** arglist);
void command_freelist(char **arglist);
int is_builtin(char** arg);
