# Mini-Shell
The program is a simulation of a simple shell in C, under the Linux operating system.

MINI AHELL
Authored by Simcha Corcos
212328199


==Description==

The program is a simulation of a simple shell in C, under the Linux operating system.


==Program DATABASE==

1.array = A two-dimensional array (assigned in a dynamic assignment) that accepts the shell command when there is NULL at the end of the array.

2.file.txt = A file that contains the history of the commands that were executed, and in fact the history is read from it as well as the commands  !(Num).


==functions==

two main functions:
1. buildArray - This function that receives a string and returns the string in the form of a two-dimensional array that ends in NULL. This array is sent to execvp to execute the shell command.

2. readHistory - This method reads and prints the file to stdin.


==Program Files==

ex2a.c - contain the main of first part.

ex2b.c - contain the main of second part.

file.txt -to read history and !(num)


==How to compile?==

Part #1:
compile: gcc ex2a.c -o ex2a
run: ./ex2a


Part #2:
compile: gcc ex2b.c -o ex2b
run: ./ex2b


==Input:==

no input


==Output:==

History, when user entered history.And all shell commands that user enter.


