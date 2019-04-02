#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <errno.h>
#include <fcntl.h>

#define BUFSIZE 512


void execute(int *fd,char **input);
void batch_mode(char **argv);
void interactive(int status);
int parse(char **commands, char *input, char *delim);
int checkInput(char *input, int *delims);
void delimsFind(int *delims, char *input, int *status);
int printCommand(int pd[], int delims[], int index);
void redirection(int pd[], int type, char *filename);
void swap(char *str1, char *str2);
void pipeExecute(char *command1,char *command2,int *pd);
void changeRedirection(int delims[], char **commands, int sizeofCommands);
void main_function(char *input, int *delims);

int main(int argc, char *argv[])
{

	printf("\t\t\t---WELCOME TO MY SHELL---\t\n");
	if(argc==1)
	{
		int status=1;
		interactive(status);
	}
	else if(argc==2)
	{
		batch_mode(argv);
	}
	else
	{
		printf("Too many arguments!\n");
		printf("EXITING...\n");
	}
	return 0;
}

void interactive(int status)
{
	while(status)
	{
		int *delims = malloc(20 * sizeof(int));
		char input[BUFSIZE];
		printf("papageorgiou_8884> ");
		fgets(input, BUFSIZE, stdin);
		status = checkInput(input,delims);
		delimsFind(delims,input, &status);

		if(status == 0)
		{
			break;
		}
		else if(status == 2)
		{
			status = 1;
			continue;
		}
		main_function(input, delims);
		
		delims = calloc(20,sizeof(delims));
		free(delims);

	}
}

void batch_mode(char **argv)
{
	char *lineInput = malloc(BUFSIZE * sizeof(char));
	char **batchInput = malloc(100 * sizeof *batchInput);
	size_t len = BUFSIZE;
	int numofLines = 0;
	int status;
	FILE *fp;
	fp = fopen(argv[1],"r");

	if(fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(0);
	}

	while(getline(&lineInput,&len,fp) != -1)
	{
		for(int j=0;j<strlen(lineInput);j++)
		{
			if(lineInput[j]=='#')
			{
				while(j<strlen(lineInput))
				{
					lineInput[j] = ' ';
					j++;
				}
				break;
			}

		}
		batchInput[numofLines++] = strdup(lineInput);
	}
	for(int i=0;i<numofLines;i++)
	{
		int *delims = malloc(20 * sizeof(int));
		status = checkInput(batchInput[i],delims);
		delimsFind(delims,batchInput[i], &status);

		if(status == 0)
		{
			break;
		}
		else if(status == 2)
		{
			status = 1;
			continue;
		}
		main_function(batchInput[i], delims);
		delims = calloc(20,sizeof(delims));
		free(delims);
	}
	printf("EXIT\n");
	fclose(fp);

}

void main_function(char *input, int *delims)
{
	char **commands = malloc(10 * sizeof *commands);
	
	size_t sizeofCmd = 0;
	size_t sizeofCommands = 0;
	int commandStatus=0;


	sizeofCommands = parse(commands,input,";&><|\n");
	
	changeRedirection(delims,commands,sizeofCommands);


	for(int i=0;i<sizeofCommands;i++)
	{
		
		char **cmd = malloc(30* sizeof *cmd);
		
		int pd[2];
		if ( pipe(pd) < 0 ){ printf("can’t open pipe"); break;}
		
		if(delims[i]==5)
		{
			if(delims[i+1]==5)
			{

				printf("Too many pipes in order\n");
				break;
			}
			pipeExecute(commands[i],commands[i+1],pd);
			i++;
		}
		else
		{
			sizeofCmd = parse(cmd,commands[i]," \t\n");
			cmd[sizeofCmd]=NULL;
			
			if(!fork())
			{
				execute(pd,cmd);
				exit(0);
			} 
			wait(NULL);
		}
		
		close(pd[1]);
		
		if(delims[i]==3)
		{
			if(delims[i+1]==3 || delims[i+1]==4)
			{
				printf("Too many redirections in order\n");
				break;
			}
			redirection(pd, delims[i],commands[i+1]);
			i++;
			close(pd[0]);
			continue;
		}

		commandStatus = printCommand(pd,delims,i);
		if(commandStatus)
		{
			i++;
			while(delims[i] != 2 && delims[i]!=0)
			{
				i++;

			}
			continue;
		}
		close(pd[0]);
		cmd = calloc(sizeofCmd,sizeof(cmd));
		free(cmd);
		sizeofCmd=0;
	}
	
	commands = calloc(20,sizeof(commands));
	free(commands);
}

void changeRedirection(int delims[], char **commands, int sizeofCommands)
{
	for(int i=0;i<sizeofCommands;i++)
	{
		if(delims[i]==4)
		{
			if(delims[i+1]==5)
			{
				swap(commands[i],commands[i+1]);
				int temp = delims[i];
				delims[i]=delims[i+1];
				delims[i+1] = temp;
			}
			else
			{
				swap(commands[i],commands[i+1]);
				delims[i] = 3;
			}
		}

	}
}
int checkInput(char *input, int *delims)
{
	char *temp = malloc(BUFSIZE * sizeof(input));
	char **toTest = malloc(20 * sizeof (toTest));
	strcpy(temp,input);
	if(strcmp(input,"\n")==0)
	{
		return 2;
	}
	int size;
	size = parse(toTest,temp," \t\n");
	if(size ==1)
	{
		if(strcmp(toTest[0],"quit")==0)
		{
			return 0;
		}
	}
	return 1;
}

void delimsFind(int *delims ,char *input, int *status)
{
	int size = 0;
	for(int i =0; i< strlen(input)-1;i++)
	{
		if(input[i]=='&')
		{
			if(input[i+1]=='&')
			{
				delims[size] = 1;
				i++;
				size++;
			}
			else 
			{
				printf("ERROR: Wrong & usage!\n");
				*status = 2;
				break;
			}
		}
		else if (input[i] == ';')
		{
			if(input[i+1]==';')
			{
				printf("ERROR: Wrong ; usage!\n");
				*status = 2;
				break;
			}
			delims[size] = 2;
			size++;
		}
		else if(input[i] == '>')
		{
			delims[size] = 3;
			size++;
		}
		else if(input[i] == '<')
		{
			delims[size] = 4;
			size++;
		}
		else if(input[i] == '|')
		{
			delims[size]=5;
			size++;
		}
	}
	delims[size]=0;
}

int parse(char **commands, char *input,char *delim)
{
	char *token;
	int size = 0;
	token = strtok(input,delim);
	
	while(token !=NULL)
	{
		//commands = realloc(commands, ++size * sizeof(*commands));
		size++;
		*(commands+size-1) = malloc(strlen(token)+1);
		
		strcpy(*(commands+size-1),token);
		token = strtok(NULL,delim);
	}

	return size;
}

void execute(int *fd,char **input)
{
	close(fd[0]);
	dup2(fd[1],1);
	close(fd[1]);
	execvp(input[0],input);
	dup2(1,STDERR_FILENO);
	printf("ERROR\n");

}

void pipeExecute(char *command1,char *command2, int *pd)
{
	int fd[2];
	char **cmd1 = malloc(30* sizeof *cmd1);
	char **cmd2 = malloc(30* sizeof *cmd2);
	//int size1,size2;
	parse(cmd1,command1," \t\n");
	parse(cmd2,command2," \t\n");
	if ( pipe(fd) < 0 ){ printf("can’t open pipe"); return;}
	if(!fork())
	{
		
		close(pd[0]);
		close(pd[1]);
		execute(fd,cmd1);
		exit(0);
	}
	else
	{
		
		if(!fork())
		{
			dup2(fd[0],0);
			dup2(pd[1],1);
			close(fd[1]);
			close(pd[0]);
			close(fd[0]);
			close(pd[1]);
			execvp(cmd2[0],cmd2);
			dup2(1,STDERR_FILENO);
			printf("ERROR\n");
			exit(0);
		}
		
		close(fd[0]);
		close(fd[1]);
		wait(NULL);
		wait(NULL);
	}

}

int printCommand(int pd[], int delims[], int index)
{
	char *buff = malloc(BUFSIZE * sizeof(char));
	int k=read(pd[0], buff, BUFSIZE);
	write(1, buff, k);
	
	if(delims[index] == 1 && strcmp(buff,"ERROR\n")==0)
	{
		return 1;
	}
	return 0;
}

void redirection(int pd[],int type,char *filename)
{
	FILE *filePointer;
	char *buff = malloc(BUFSIZE * sizeof(char));
	char *newfilename = malloc(sizeof(filename));
	close(pd[1]);
	int index1;
	int index2;
	read(pd[0], buff, BUFSIZE);
	
	for(int i=0;i<strlen(filename);i++)
	{
		if(filename[i] != ' ' && filename[i]!='\t')
		{
			index1 = i;
			break;
		}
	}
	for(int i=strlen(filename)-1;i>=0;i--)
	{
		if(filename[i] != ' ' && filename[i]!='\t')
		{
			index2 = i;
			break;
		}
	}
	for(int i=index1;i<=index2;i++)
	{
		newfilename[i-index1] = filename[i];
	}
	filePointer = fopen(newfilename,"w");
	fputs(buff,filePointer);
	fclose(filePointer);
}

void swap(char *str1, char *str2)
{
	char *temp = malloc(sizeof(str1));
	strcpy(temp,str1);
	str1 = realloc(str1,sizeof (str2));
	strcpy(str1,str2);
	str2 = realloc(str2,sizeof(temp));
	strcpy(str2,temp);
}
