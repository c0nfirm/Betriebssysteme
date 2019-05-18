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
#define SHELL_KEY_DELIM "\t,\r,\n,\a, "

/*function declrations for shell commands*/
int shell_cd(char **args);
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
    &shell_exit
};/*returns the nmber of tools*/
int shell_num_tools(){
    return sizeof(tools) / sizeof(char *);
}
int stopwait = 0; // if 0 ctrl+c stops shell,if 1 stops wait by setting it to 0

void sig_handler(int signr)
{
    if (stopwait == 1)
    {
        stopwait = 0;
    }
    else
    {
        exit(-1);
    }
}
/*command Function implementation*/

/*         Change Directory Command
    args[0] =  "cd", args[1] = directory
    returns 1 to continue execution
*/
int shell_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "Shell: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("Shell");
        }
    }

    return 1;
}

/*              WAIT Command
        returns 1 to continue execution
*/
int shell_wait(char **args, int argcount)
{
    if (argcount == 1)
    {
        fprintf(stderr, "Shell: expected argument for \"wait\"\n");
    }
    else
    {
        int state;
        int finished_count = 0;
        stopwait = 1;
        while (stopwait == 1)
        {
            pid_t cpid = waitpid(-1, &state, 0);
            for (int i = 0; i < argcount; i++)
            {
                if (atoi(args[i]) == (int)cpid)
                {
                    if (WIFEXITED(state) != 0)
                    {
                        printf("[%d] TERMINATED\n", cpid);
                    }
                    else if (WIFSIGNALED(state) != 0)
                    {
                        printf("[%d] SIGNALED\n", cpid);
                    }
                    else if (WCOREDUMP(state) != 0)
                    {
                        printf("[%d] COREDUMP\n", cpid);
                    }
                    else if (WSTOPSIG(state) != 0)
                    {
                        printf("[%d] STOPSIG\n", cpid);
                    }
                    printf("[%d] EXIT STATUS: %d\n", cpid, WEXITSTATUS(state));
                    finished_count++;
                }
            }
            if (finished_count == argcount-1)
                return 1;
        }
        printf("WAIT interrupted!\n");
    }
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
    pid_t pid;
    int state;

    printf("command is %s \n", args[0]);
    pid = fork();
    if (pid == 0)
    {
        /*Child Process*/
        if (execvp(args[0], args) == -1) //path dir
        {
            if (execv(args[0], args) == -1) //current dir
            {
                perror("Shellchild");
            }
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        if (pid < 0)
        {
            /*Forking Error*/
            perror("Shell");
        }
        else
        {
            /*Parent Process*/
            do
            {
                waitpid(pid, &state, WUNTRACED);
            } while (!WIFEXITED(state) && !WIFSIGNALED(state));
        }
    }
    return 1;
}
int launch_background(char **args){
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        /*Child Process*/
        if (execvp(args[0], args) == -1) //path dir
        {
            if (execv(args[0], args) == -1) //current dir
            {
                perror("Shellchild");
            }
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        if (pid < 0)
        {
            /*Forking Error*/
            perror("Shell");
        }
        else
        {
            /*Parent Process*/
            printf("[%d]\n", (int) pid);
        }
    }
    return 1;

}
/*
    executes shell command or launch a programm
    returns 1 if shell should continue or 0 if it should be terminated
*/
int execute(char **args,int argcount){
    int i;
    if(args[0] == NULL){
        /*empty command was entered*/
        return 1;
    }

    /*execites a specified shell command*/
    if(strcmp(args[0], "wait") == 0){
        return shell_wait(args,argcount);
    }
    for(i = 0; i < shell_num_tools(); i++){
        if(strcmp(args[0], tools[i]) == 0){
            return (*tools_func[i])(args);
        }
    }
    for (int i = 0; i < argcount; i++)
    {
        if (strchr(args[i],'|') != 0)
        {
            printf("pipemode\n");
            return 1;
        }
        if (strchr(args[i],'&') != 0) //background mode
        {
            return launch_background(args);
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
char **split_line(char *line,int* argcount){
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
    *argcount = posi;
    return keys;
}
/*      Shell LOOP
    getting user input and executes it
*/
char *rel_wd(char *current, char *start)
{
    char *ret = calloc(1000, sizeof(char));
    if (strcmp(current, start) == 0) //wir sind im geleichen verzeichniss wo wir gestartet sind
    {
        strcpy(ret, "./");
        return ret;
    }
    if (strstr(current, start) != 0) //current > start, also zb. wir sind in /dir
    {
        strcpy(ret, current + strlen(start) + 1);
        strcat(ret, "/");
        return ret;
    }
    int i = 0;
    while (memcmp(start, current, i + 1) == 0) //abziehen von gleichen substring am anfang
    {
        i++;
    }
    while (strchr(start + i, '/') != 0) //ersetzen von ebenen nach oben mit "../"
    {
        start = strchr(start + i, '/');
        strcat(ret, "../");
    }

    strcat(ret, current + i); //andere pfade anhaengen
    return ret;
}

int  main(int argc, char *argv[]){
	/*shell command loop*/
	//loop();
    signal(SIGINT, sig_handler);
    char *line;
    char **args;
    int state;
    int argcount;
    char start[1000];
    char current_wd[1000];
    getcwd(start,1000);

    do{

        char * pointer_current_wd;
        getcwd(current_wd,1000);
        pointer_current_wd = rel_wd(current_wd,start);
        printf(strcat(pointer_current_wd,">"));


        line = read_line();
        args = split_line(line,&argcount);
        state = execute(args,argcount);

        free(line);
        free(args);
        free(pointer_current_wd);
        argcount = 0;
    } while(state);

	return EXIT_SUCCESS;
}
