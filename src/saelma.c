#include "shell.h"

int child(string_t * tokens) {

    string_t * new_tokens = find_pipe(tokens, "|");

    while (new_tokens) {
        int com[2];

        if (pipe(com) == -1) {
            print_error("Pipe creation failed.\n");
            exit(-1);
        }
        if (Fork() == 0) {
            // Child behavior
            close(com[0]);
            dup2(com[1], 1);
            child(tokens);
        } else {
            // Parent behavior
            close(com[1]);
            dup2(com[0], 0);
            wait(NULL);
        }

        tokens = new_tokens;
        new_tokens = find_pipe(tokens, "|");
    }

    // for(int i = 0; tokens[i] != NULL; i++)
    // print_debug("token[%d]: \"%s\"\n", i, tokens[i]);

    int flag = set_redirect(tokens);
    if (flag == -1) exit(-1);

    // Checking for "sudo" keyword
    if (!strcmp(tokens[0], "sudo")) { saelma_sudo(); exit(0); }

    // printf("new_tokens[%d]: %s\n", 2, new_tokens[2]); fflush(stdout);
    execvp(tokens[0], tokens); // Going throught PATH env first
    if (!strcmp(tokens[0], "hello")) { // If it fails, then checking for custom commands
        saelma_hello(tokens);
    }
    print_error("Unknown command: \"%s\"\n", tokens[0]); // Otherwise, print error message
    exit(-1);
}

int main(int argc, string_t argv[]) {

    // Checking for consistent number of arguments, the debug flag
    if (argc != 2) {
        print_error("Incorrect number of arguments, expected 1 got %d.\n", argc-1);
        exit(-1);
    }

    // Displays the logo
    print_logo();

    // Loads the user nick
    load_nick();

    // Setting up the main while loop
    int debug = atoi(argv[1]); // Retrieving the debug flag from the user
    char cmd_arr[CMD_MAX_LEN]; 
    string_t cmd = cmd_arr; // Better use stack allocation to avoid memory leaks in childs
    int n;
    string_t tokens[CMD_MAX_TOK];
    pid_t pid;

    while (1) {
        // Clearing the command buffer and token buffer
        memset(cmd, 0, CMD_MAX_LEN);
        memset(tokens, 0, CMD_MAX_TOK);

        print_prompt(); fflush(stdout); // Displaying the prompt //? Add CWD and CD ??

        n = read(0, cmd, CMD_MAX_LEN); // Reading the user input
        if (!n) { printf("\n"); saelma_exit(); } // Capturing CTRL-D's EOF

        cmd[strlen(cmd)-1] = '\0'; // Remove the '\n' character at the end of the input

        if (debug) print_debug("Received: \"%s\"\n", cmd);
        trim_whitespace(&cmd); // Removing the whitespace duplicates
        if (debug) print_debug("Trimmed:  \"%s\"\n", cmd);

        int flag = sep_tokens(tokens, ' ', cmd); // Separating the command into tokens

        if (flag == -1) { // Checking if too many arguments in command
            print_error("Too many arguments in command line.\n");
            continue;
        }

        if (debug) for(int i = 0; tokens[i] != NULL; i++)
        print_debug("token[%d]: \"%s\"\n", i, tokens[i]);

        if (!strcmp(tokens[0], "exit")) { // Checking for the keyword "exit"
            saelma_exit();
        }

        // Creating a child process to execute the command
        if (!strcmp(tokens[0], "nick")) {
            saelma_nick(tokens);
        } else if (!strcmp(tokens[0], "cd")) {
            saelma_cd(tokens);
        } else if (!(pid = Fork())) {
            // Child behavior
            if (pid != -1) child(tokens);
            print_error("Failure during subprocess creation.\n");
            exit(-1); // Return in case of fork failure
        } else {
            // Parent behavior
            wait(NULL); // Parent always wait for child process
        }
    }
    return 0; // Shouldn't ever happen but who knows?
}
