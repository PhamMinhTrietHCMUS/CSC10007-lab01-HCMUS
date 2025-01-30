#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char* tokenizer(char* input, char delim)
{

    static char* keeper_ptr;
    char* iteratr;
    char* res;
    if(input){
        iteratr = input;
        res = input;
    }
    else{
        iteratr = keeper_ptr;
        res = keeper_ptr;
    }

    if(*iteratr == '\0')
        return 0x0;

    int boolean_delim_found = 0;
    while(1)
    {
        if(*iteratr == '\0')
        {
            keeper_ptr = iteratr;
            break;
        }
        else if(*iteratr != delim && !boolean_delim_found)
            iteratr++;
        else if(*iteratr == delim)
        {
            *iteratr = '\0';
            boolean_delim_found = 1;
            iteratr++;
        }
        else if((*iteratr != delim) && boolean_delim_found)
        {
            keeper_ptr = iteratr;
            break;
        }
    }
    return res;
}

// the overall idea here is xargs_line_handler will handle each line of input being piped in by whatever comes before the pipe.
// the 2d array that contains all the commands will be created within main, passed into xargs_line_handler to then be modified by tokenizing the line and affixing it in the second-to-last position.
// for each token it will call exec once
// the main function will have a main loop to get all the line and then pass them in to xargs_line_handler

void xargs_line_handler(int argc, char** xargs_args_list, char* line)
{
    char* tok = tokenizer(line, ' ');
    while(tok)
    {
        strcpy(xargs_args_list[argc - 1], tok);
        tok = tokenizer(0x0, ' ');
        int pid = fork();
        if(pid == 0)
        {
            exec(xargs_args_list[0], xargs_args_list);
            printf("exec xarg %s failed!\n", xargs_args_list[0]);
        }
        else if(pid > 0)
            wait(0);
        else
            printf("fork failed\n");
    }
}




int main(int argc, char** argv)
{
    char** xargs_arguments = malloc((argc + 1) * sizeof(char*));
    memset(xargs_arguments, 0x0, (argc + 1) * sizeof(char*));
    // this creates an array of char pointers, all set to 0x0

    for(int i = 0; i < argc - 1; i++)
    {
        xargs_arguments[i] = malloc(strlen(argv[i + 1]) * sizeof(char));
        strcpy(xargs_arguments[i], argv[i + 1]);
    }
    xargs_arguments[argc - 1] = malloc(65 * sizeof(char));
    // this builds the xargs_arguments with arguments passed into xargs. for example, xargs echo aa bb cc will result in
    // [echo] [aa] [bb] [cc] [<empty>] [ptr = 0x0]


    while(1)
    {
        char piped_in_line[65];
        gets(piped_in_line, 65);
        if(strlen(piped_in_line) == 0 || !strcmp(piped_in_line, "\n"))
            break;// if the line is just empty, break
        if(piped_in_line[strlen(piped_in_line) - 1] == '\n')
            piped_in_line[strlen(piped_in_line) - 1] = '\0'; // strips the line of any surplus newline character
        
        xargs_line_handler(argc, xargs_arguments, piped_in_line); // run xargs line handler if line isnt empty 
    }

    for(int i = 0; i < argc + 1; i++)
    {
        if(xargs_arguments[i])
            free(xargs_arguments[i]);
        else
            break;
    }

    free(xargs_arguments);
    // this frees allocated memory. need i say more?
}