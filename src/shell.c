#include "shell.h"

// ============================ //
// === SÆLMA Implementation === //
// ============================ //

// === Display === //   

void print_logo(void) {
    printf("\033[0;35m███████\033[0;32m╗  \033[0;35m█████\033[0;32m╗\033[0;35m█████\033[0;32m╗ \033[0;35m██\033[0;32m╗      \033[0;35m███\033[0;32m╗   \033[0;35m███\033[0;32m╗  \033[0;35m█████\033[0;32m╗ \n");
    printf("\033[0;35m██\033[0;32m╔════╝ \033[0;35m██\033[0;32m╔══\033[0;35m██\033[0;32m╔════╝ \033[0;35m██\033[0;32m║      \033[0;35m████\033[0;32m╗ \033[0;35m████\033[0;32m║ \033[0;35m██\033[0;32m╔══\033[0;35m██\033[0;32m╗\n");
    printf("\033[0;35m███████\033[0;32m╗ \033[0;35m██████████\033[0;32m╗   \033[0;35m██\033[0;32m║      \033[0;35m██\033[0;32m╔\033[0;35m████\033[0;32m╔\033[0;35m██\033[0;32m║ \033[0;35m███████\033[0;32m║\n");
    printf("╚════\033[0;35m██\033[0;32m║ \033[0;35m██\033[0;32m╔══\033[0;35m██\033[0;32m╔══╝   \033[0;35m██\033[0;32m║      \033[0;35m██\033[0;32m║╚\033[0;35m██\033[0;32m╔╝\033[0;35m██\033[0;32m║ \033[0;35m██\033[0;32m╔══\033[0;35m██\033[0;32m║\n");
    printf("\033[0;35m███████\033[0;32m║ \033[0;35m██\033[0;32m║  \033[0;35m███████\033[0;32m╗ \033[0;35m███████\033[0;32m╗ \033[0;35m██\033[0;32m║ ╚═╝ \033[0;35m██\033[0;32m║ \033[0;35m██\033[0;32m║  \033[0;35m██\033[0;32m║\n");
    printf("\033[0;32m╚══════╝ ╚═╝  ╚══════╝ ╚══════╝ ╚═╝     ╚═╝ ╚═╝  ╚═╝\033[0;0m\n\n");
}
void print_prompt(void) {
    char buf[128];
    getcwd(buf, 128);
    fprintf(stdout, "\033[0;35m\033[1m%s\033[0;0m@\033[0;32m\033[1mSÆLMA\033[0;0m:\033[32m%s\033[0m\033[0;35m>\033[0;0m ", user_nick, buf);
}
void print_saelma(const string_t format, ...) {
    fprintf(stderr, "\033[0;32m[SÆLMA]\033[0;0m ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
void print_debug(const string_t format, ...) {
    fprintf(stderr, "\033[0;33m[SÆLMA Debug]\033[0;0m ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
void print_error(const string_t format, ...) {
    fprintf(stderr, "\033[0;31m[SÆLMA Error]\033[0;0m ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

// === Processus management === //

pid_t Fork(void) {
    pid_t pid;
    if ((pid = fork()) == -1) {
        print_error("Unsuccessful fork() attempt, ignoring the command.");
    }
    return pid;
}
void trim_whitespace(string_t * cmd) {
    // Trimming front whitespace
    int n = strlen(*cmd);
    if (!n) return;
    while((*cmd)[0] == ' ') {
        (*cmd)++; n--;
    }
    if (!n) return;
    // Trimming end whitespaces
    int i = n-1;
    while((*cmd)[i] == ' ') {
        (*cmd)[i--] = '\0'; n--;
    }
    if (n<2) return;
    // Trimming multiples middle whitespaces
    i = n-2;
    while(i != 0) {
        if((*cmd)[i-1] == ' ' && (*cmd)[i] ==  ' ') {
            for(int j = i; j < n; j++) (*cmd)[j] = (*cmd)[j+1];
        } else {
            i--;
        }
    }
}
int sep_tokens(string_t * tokens, char sep, string_t cmd) {
    int tok = 0;
    tokens[tok++] = cmd;
    int n = strlen(cmd);
    for (int i = 0; i < n; i++) {
        if (cmd[i] == sep) {
            cmd[i] = '\0';
            tokens[tok++] = &cmd[i+1];
        }
        if (tok > CMD_MAX_TOK) return -1;
    }
    tokens[tok] = NULL;
    return tok;
}
string_t find_redirect(string_t * tokens, string_t delimiter) {
    string_t str = NULL;
    for(int i = 0; tokens[i] != NULL; i++)
    if(!strcmp(tokens[i], delimiter)) {
        str = (string_t)malloc(sizeof(char)*strlen(tokens[i+1]) + 1);
        strcpy(str, tokens[i+1]);
        for(int j = i; tokens[j-1] != NULL; j++)
        tokens[j] = tokens[j+2]; 
        return str;
    }
    return str;
}
int set_redirect(string_t * tokens) {
    string_t redir[] = {">", "0>", "1>", "2>", ">>", "0>>", "1>>", "2>>", "<", "<<", NULL};
    
    string_t file;
    int target;
    int replaced;
    for (int i = 0; redir[i]; i++) {
        while ((file = find_redirect(tokens, redir[i]))) {
            if (i <= 3) {
                target = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                if (i == 0) replaced = 1;
                else replaced = redir[i][0] - '0';
            } else if (i <= 7) {
                target = open(file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                if (i == 4) replaced = 1;
                else replaced = redir[i][0] - '0';
            } else if (i == 8) {
                target = open(file, O_RDONLY);
                replaced = 0;
            } else if (i == 9) {
                print_error("Redirection using << is not implemented.\n");
                //! To be implemented in later versions... If there's any...
                exit(-1);
            }
            if (target == -1) {
                print_error("File \"%s\" doesn't exist or hold the right permissions.\n", file);
                exit(-1);
            }
            dup2(target, replaced);
        }
    }
    return 0;
}
string_t * find_pipe(string_t * tokens, string_t delimiter) {
    for(int i = 0; tokens[i] != NULL; i++)
    if(!strcmp(tokens[i], delimiter)) {
        tokens[i] = NULL;
        return (tokens+i+1);
    }
    return NULL;
}

// === Custom Functions === //

/* Get username from host */
void get_username(string_t nick) {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        strcpy(nick, pw->pw_name);
        return;
    }
    strcpy(nick, "user"); // No username found. User = default string
}
void load_nick() {
    FILE * f = fopen(config_path, "r");
    if (!f) {
        // Handle host username instead
        get_username(user_nick);
        return;
    }
    fgets(user_nick, NICK_MAX_LEN, f);
    fclose(f);
}
void load_config() {
    char name[NICK_MAX_LEN];
    get_username(name);
    strcat(config_path, name);
    strcat(config_path, "/.config/saelma");
}
void saelma_nick(string_t * args) {
    if (!args[1]) {
        print_error("Missing argument for command: nick <username>\n");
        return;
    }
    strcpy(user_nick, args[1]);
    FILE * f = fopen(config_path, "w");
    if (!f) {
        print_error("\033[0;31mCRITICAL\033[0;0m Cannot write to .config/nick file.\n");
        exit(-1);
    }
    fputs(user_nick, f);
    fclose(f);
}
void saelma_cd(string_t * args) {
    if (!args[1]) {
        print_error("Missing argument for command: cd <dir>\n");
        return;
    }
    int rvalue;
    char path[1024];
    strcpy(path, args[1]);

    char cwd[1024];
    if(args[1][0] != '/')
    {
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/");
        strcat(cwd, path);
        rvalue = chdir(cwd);
    } else {
        rvalue = chdir(args[1]);
    }
    if (rvalue != 0) print_error("%s: No such directory.\n", args[1]);
}
void saelma_exit(void) {
    print_saelma("Exiting...\n");
    exit(0);
}
void saelma_hello(string_t * args) {
    if (!args[1]) {
        print_saelma("hello there\n");
    } else if (!strcmp(args[1], "there")) {
        print_saelma("\033[0;33m\033[1mGeneral Kenobi!?\033[0;0m\n");
    } else {
        print_saelma("hello user\n");
    }
    exit(0);
}
void saelma_sudo() {
    print_saelma("You're \033[0;31mNOT\033[0;0m sudo.\n");
}
