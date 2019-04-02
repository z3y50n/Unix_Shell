#Operating Systems
##My Shell
###Papageorgiou Dimitrios 8884

\---------------------------------------------

####Installation
    `>cd ~/Download/8884
    `make

####How to use

####Special characters

; - run 2 commands in a row

&& - run 2 commands only if the first is succesfull

\> or < - Redirect output in a file

| - run the second command with input from the output of the first command

###\*\*\_Interactive Mode_**

In the installation folder type:

    >/bin/myshell
The shell will start and you can press commands.

example:

    >ls | wc > file ; echo hello world

It supports 1 redirection and 1 pipe in row

    >ls | head -3 | tail -2 > file > file2
    >ERROR

Type `>quit` to exit 

### \*\*\_Batch Mode_**

In the installation folder type:

    >/bin/myshell "code.sh" 

The terminal will execute all the commands that are inside the file and exit