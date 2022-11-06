#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include <sys/stat.h> //For the test of file existence

#define SIZE 512

void fileExist(FILE* file);
char** buildArray(char* command);
int countWords(char* command);
void checkAlloc(void* sen);
void checkFileOpen(FILE* file);
void readHistory(FILE* file);
void freeArray(char** array);

int main()
{
    char pwd[SIZE]; //get pwd
    char command[SIZE]; //get command from user
    pid_t pid;
    char** array, *copy_cmd;
    int numCommands = 0, numWords = 0, status;
    FILE* file = NULL;
    fileExist(file);

    while (1)
    {
        getcwd(pwd,SIZE);//Get the path
        printf("%s>",pwd);
        fgets(command, SIZE, stdin);//scan command from user
        command[strlen(command)-1] = '\0';//end with '\0' instead of '\n'

        if(countWords(command) == 0)//if spaces or '\n'
        {
            continue;
        }

        if(strcmp(command, "cd") == 0)//if cd - count commands,and +1 for words in commands
        {
            numCommands++;
            numWords++;
            printf("command not supported (Yet)\n");
            continue;
        }

        if(strcmp(command, "done") == 0)//if done - commands +1, print commands and words in commands and exit
        {
            numCommands++;
            printf("Num of commands: %d\n",numCommands);
            printf("Total number of words in all commands: %d\n", numWords);
            exit(0);
        }

        if(strcmp(command,"history") == 0)// if history - read history from file
        {
            readHistory(file);
            file = fopen("file.txt", "a+");//open file for append
            checkFileOpen(file);//check if worked
            fprintf(file, "%s\n", command);//write to file the command
            fclose(file);
            continue;
        }
        
        copy_cmd = (char*)malloc(sizeof(char)*strlen(command)+1);//Save an original string before cuting it
        strcpy(copy_cmd,command);
        array = buildArray(command);//2D dinamic array

        pid = fork();//creating a son process
        if(pid == -1)//if failed - exit 1 and message for user
        {
            perror("fork failed\n");
//            freeArray(array);
            exit(1);
        }
        if(pid == 0)//son
        {
            if(execvp(array[0], array) == -1)//do the command like in terminal
            {
                perror("command not found\n");//if failed - exit 1 and message for parent process
                exit(1);
            }
        }
        //parent
        wait(&status);//Wait for the son's graduation status to know if he succeeded or failed
//            freeArray(array);
        if(WEXITSTATUS(status)==0)//if succeeded count commands and words in command
        {
            numCommands++;
            numWords+=countWords(copy_cmd);
            file = fopen("file.txt", "a+");//open file for append
            checkFileOpen(file);//check if worked
            fprintf(file, "%s\n", copy_cmd);//write to file the command
            fclose(file);
        }
        free(copy_cmd);
    }
    return 0;
}

void fileExist(FILE* file)//check if file exist and open, than close.
{
    struct stat buffer;
    if (stat("file.txt", &buffer) == 0)//if exist open to append
    {
        file = fopen("file.txt", "a+");
        checkFileOpen(file);//check if open good
    }
    else
    {
        file = fopen("file.txt", "w+");//if does'nt exist open to write - create file.
        checkFileOpen(file);//check if open good
    }
    fclose(file);
}

char** buildArray(char* command)//get command and return Two-dimensional array
{
    char *token;
    char** array;
    int numWords = countWords(command), i = 0;

    array = (char**)malloc(sizeof(char*)*numWords+1);
    checkAlloc(array);
    token = strtok(command, " ");//cut by " "

    while (token != NULL && i < numWords)
    {
        array[i] = (char*)malloc(sizeof(char)*strlen(token)+1);
        checkAlloc(array[i]);//put token in array
        strcpy(array[i],token);
        token = strtok(NULL, " ");
        i++;
    }
    array[numWords] = NULL;//last place = NULL
    return array;
}

int countWords(char* command)//count words in a string
{
    int counter = 0;
    char* token, *new_sen;
    new_sen = (char*)malloc(sizeof(char)*strlen(command)+1);

    strcpy(new_sen, command);

    token = strtok(new_sen, " ");

    while( token != NULL )// cut the sentence by " ".
    {
        if( (strcmp(token, "\n")) != 0) //count words
        {
            counter++;
        }
        token = strtok(NULL, " ");//cut next time
    }
    free(new_sen);
    return counter; //return num of words.
}

void checkAlloc(void* sen)//check if malloc succeeded
{
    if(sen == NULL)
    {
        perror("Malloc failed\n");//fail message to user
        exit(1);
    }
}

void checkFileOpen(FILE* file)//check if open good
{
    if(file == NULL)
    {
        printf("Error: can not open file.\n"); //fail message to user
        fclose(file);
        exit(1);//error
    }
}

void readHistory(FILE* file)
{ 
    char buffer[SIZE];//for read a line from file.
    int i = 0;
    file = fopen("file.txt", "r"); //open file for read
    checkFileOpen(file);
    while(fgets(buffer, SIZE, file) != NULL) // read line from file
    {
        printf("%d: %s", i, buffer);
        i++;
    }
    fclose(file);
}
void freeArray(char** array)
{
    int i = 0;

    while(array[i] != NULL)
    {
        free(array[i]);
        i++;
    }
    free(array);
}