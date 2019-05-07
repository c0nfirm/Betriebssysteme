#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "array.h"

#define SHELL_KEY_BUFF_SIZE 64
#define SHELL_KEY_DELIM "\t\r\n\a"

/*function declrations for shell commands*/
int shell_cd(char **args);
int shell_wait(char **args);
int shell_exit(char **args);

/*list of aviable commands*/
char *tools[] = {
    "cd",
    "wait",
    "exit"
};
/*corresponding functions to commands above*/
int (*tools_func[]) (char **) = {
    &shell_cd,
    &shell_wait,
    &shell_exit
};/*returns the nmber of tools*/
int shell_num_tools(){
    return sizeof(tools) / sizeof(char *);
}

/*command Function implementation*/

/*         Change Directory Command
    args[0] =  "cd", args[1] = directory
    returns 1 to continue execution
*/
int shell_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "Shell: expected argument to \"cd\"\n");
    }else{
        if(chdir(args[1]) != 0){
            perror("Shell");
        }
    }

    return 1;
}

/*              WAIT Command
        returns 1 to continue execution
*/
int shell_wait(char **args){
    return 1;
}

/*
            exit command
    allways returns 0 to terminate execution
*/
int shell_exit(char **args){
    return 0;
}

/*Reading INPUT Line*/
/*
    Launch a programm and wait for its termination
    returns 1 to continue execution
*/
int launch(char **args){
    pid_t pid,wpid;
    int state;

    pid = fork();
    if(pid == 0){
        /*Child Process*/
        if(execvp(args[0], args) == -1){
            perror("Shell");
        }
        exit(EXIT_FAILURE);
    }else{
        if(pid < 0){
            /*Forking Error*/
            perror("Shell");
        }else{
            /*Parent Process*/
            do{
                wpid = waitpid(pid, &state, WUNTRACED);
            }   while(!WIFEXITED(state) && !WIFSIGNALED(state));
         }
     }
     return 1;
}
/*
    executes shell command or launch a programm
    returns 1 if shell should continue or 0 if it should be terminated
*/
int execute(char **args){
    int i;

    if(args[0] == NULL){
        /*empty command was entered*/
        return 1;
    }

    /*execites a specified shell command*/
    for(i = 0; i < shell_num_tools(); i++){
        if(strcmp(args[0], tools[i]) == 0){
            return (*tools_func[i])(args);
        }
    }

    /*launches a specified programm*/
    return launch(args);
}

#define SHELL_BUFF_SIZE 1024
/*reads input line from stdin and returns it*/
char *read_line(void){
    int buff_size = SHELL_BUFF_SIZE;
    int posi = 0;
    char *buff = malloc(sizeof(char) * buff_size);
    int c;

    if(!buff){
        fprintf(stderr, "Shell: Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        /*read a character*/
        c = getchar();

        /*when EOL, replace it with null character and return*/
        if(c == EOF || c == '\n'){
            buff[posi] = '\0';
            return buff;
        }else{
            buff[posi] = c;
        }

        posi++;

        /*reallocate it buffer is exceeded*/
        if(posi >= buff_size){
            buff_size += SHELL_BUFF_SIZE;
            buff = realloc(buff, buff_size);
            if(!buff){
                fprintf(stderr, "Shell: Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
/*  Splits the input line into tokens
    returns a null terminated array of tokens
*/
char **split_line(char *line){
    int buff_size = SHELL_KEY_BUFF_SIZE, posi = 0;
    char **keys = malloc(buff_size * sizeof(char*));
    char *key;

    key = strtok(line, SHELL_KEY_DELIM);
    while(key != NULL){
        keys[posi] = key;
        posi++;

        if(posi >= buff_size){
            buff_size += SHELL_KEY_BUFF_SIZE;
            keys = realloc(keys, buff_size * sizeof(char*));
            if(!keys){
                fprintf(stderr, "Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
        }

        key = strtok(NULL, SHELL_KEY_DELIM);
    }

    keys[posi] = NULL;
    return keys;
}
/*      Shell LOOP
    getting user input and executes it
*/
void loop(){

/*      Main function
    argc = argument count and
    argv = argument vector
    returns a status code
*/

    char *line;
    char **args;
    int state;

    do{
        printf("./>");
        line = read_line();
        args = split_line(line);
        state = execute(args);

        free(line);
        free(args);
    } while(state);
}

int main(void){
	/*shell command loop*/
	loop();
	
	return EXIT_SUCCESS;
}
