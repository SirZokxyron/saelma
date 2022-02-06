#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

// ============================= //
// === SAELMA Implementation === //
// ============================= //

#define CMD_MAX_LEN     1024
#define CMD_MAX_TOK     64
#define NICK_MAX_LEN    64

typedef char * string_t;

static char user_nick[NICK_MAX_LEN] = "User";

// === Display === //

void print_logo(void);
void print_prompt(void);
void print_saelma(const string_t format, ...);
void print_debug(const string_t format, ...);
void print_error(const string_t format, ...);

// === Processus management === //

pid_t Fork(void);
void trim_whitespace(string_t * cmd);
int sep_tokens(string_t * tokens, char sep, string_t cmd);
string_t find_redirect(string_t * tokens, string_t delimiter);
int set_redirect(string_t * tokens);
string_t * find_pipe(string_t * tokens, string_t delimiter);

// === Custom Functions === //

void load_nick();
void saelma_nick(string_t * args);
void saelma_exit(void);
void saelma_hello(string_t * args);
void saelma_sudo();

