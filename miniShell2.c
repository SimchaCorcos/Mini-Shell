#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
int allDigits(char* command);
char* readCommand(FILE* file, int numCommand, char b1[]);
void freeArray(char** array);

int numCommands = 0, numWords = 0;

int main()
{
    char pwd[SIZE]; //get pwd
    char command[SIZE]; //get command from user
    char b1[SIZE];//for readCommand
    char* command2, *buff;
    char** array, *copy_cmd = NULL;
    int status, num, flag = 0;
    pid_t pid_1, pid_2;
    FILE* file = NULL;
    fileExist(file);

    while (1)
    {
        getcwd(pwd,SIZE);
        printf("%s>",pwd);
        fgets(command, SIZE, stdin);
        command[strlen(command)-1] = '\0';//End with '\n'
        copy_cmd = (char*)malloc(sizeof(char)*strlen(command)+1);
        checkAlloc(copy_cmd);
        strcpy(copy_cmd,command);//Keep command before cutting 

        if(countWords(command) == 0)//If there is no command - continue
        {
            continue; 
        }

        if(strcmp(command, "cd") == 0)//If cd - print and continue
        {
            numCommands++; //count commands
            numWords++;  //count words in commands
            printf("command not supported (Yet)\n");
            continue;
        }

        if(strcmp(command, "done") == 0) // if done - print num commands and words in commands, and exit.
        {
            numCommands++;
            printf("Num of commands: %d\n",numCommands);
            printf("Total number of words in all commands: %d !\n", numWords);
            exit(0);//finish
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
       
        num = allDigits(command);//get the number after '!'.

        buff = readCommand(file, num, b1); //get the command from file.

        if((command[0] == '!') && (num != -1))
        {  
            command2 = (char*)malloc(sizeof(char)*strlen(buff)+1);
            checkAlloc(command2);
            strcpy(command2, buff);
            pid_1 = fork();
            array = buildArray(command2);//cut the string by " " - private method
            free(command2);
            if(pid_1 == 0)//son
            {
                if(execvp(array[0], array) == -1) // if execvp failed - print and exit
                {
                    freeArray(array);
                    flag = 1;
                    perror("command not found\n");
                    exit(1);
                }
            }
            //parent
            wait(&status);// wait the son to finish
            if(flag == 0)
            {
                freeArray(array);
            }
            continue;
         }

        pid_2 = fork();
        array = buildArray(command);//cut the string by " " - private method
        if(pid_2 == 0)//son
        {
            if(execvp(array[0], array) == -1)//if execvp failed - print and exit
            {
                freeArray(array);
                flag = 1;
                perror("command not found\n");
                exit(1);
            }
        }
        //parent
        wait(&status);// wait the son to finish
        if(flag == 0)
        {
            freeArray(array);
        }
        if(WEXITSTATUS(status)==0)//if son succeeded 
        {
            numCommands++; // count commands
            numWords+=countWords(copy_cmd); //count words in commands
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
    if (stat("file.txt", &buffer) == 0) //if exist open to append
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

void checkFileOpen(FILE* file)//check if open good
{
    if(file == NULL)
    {
        printf("Error: can not open file.\n"); //fail message to user
        fclose(file);
        exit(1);//error
    }
}

char** buildArray(char* command)//get command and return Two-dimensional array
{
    char *token;
    char** array;
    int numWords = countWords(command), i = 0;

    array = (char**)malloc(sizeof(char*)*numWords+1);
    checkAlloc(array);
    token = strtok(command, " "); //cut by " "
    
    while (token != NULL && i < numWords)
    {
        array[i] = (char*)malloc(sizeof(char)*strlen(token)+1);
        checkAlloc(array[i]);  
        strcpy(array[i],token);//put token in array
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
    new_sen = (char*)malloc(sizeof(char)* strlen(command)+1);

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

void readHistory(FILE* file)
{
    char buffer[SIZE];//for read a line from file.
    int i = 0;
    file = fopen("file.txt", "r"); //open file for read
    checkFileOpen(file);
    while(fgets(buffer, SIZE, file) != NULL) // read line from file
    {
        printf(" %d  %s", i, buffer);
        i++;
    }
    fclose(file);
}

int allDigits(char* command)
{
    int i, num;
    char* digits = (char*)malloc(sizeof(char)*strlen(command));//no +1 becouse we don't need !.
    for(i = 1; i < (int)strlen(command); i++)
    {
        if(isdigit(command[i]) == 0)
        {
            return -1;
        }
        digits[i-1] = command[i];
    }
    num = (int)atol(digits);
    free(digits);
    return num;
}

char* readCommand(FILE* file, int numCommand, char buffer[])
{
    int i = 0, flag = 0;
    file = fopen("file.txt", "r");//open file for read
    checkFileOpen(file);

    while(fgets(buffer, SIZE, file) != NULL)//run all file
    {
        if(i == numCommand)
        {
            fclose(file);
            flag = 1;
            buffer[strlen(buffer)-1] = '\0';
            return buffer;//return specific command
        }
        i++;
    }
    if(flag == 0)//to know if file closed
    {
        fclose(file);
    }
    return NULL;
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
