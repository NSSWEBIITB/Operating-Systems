/*What to replace here?????
 *questions to be asked in class have '???'
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
/*Can we use dirent.h ????*/

#include <fcntl.h>
/* Again.... Can we use fcntl.h ???? */
// cat not implemented

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

// Helper functions
unsigned int stoi(char *token)
{
	unsigned int unsInt = 0;
	for (int i = 0; i < strlen(token); i++)
	{
		unsInt = unsInt * 10 + (token[i] - '0');
	}
	return unsInt;
}

/* Splits the string by space and returns the array of tokens */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

//

int echo(char **tokens, int tokenNo);
int pwd();
void ls();
int cd();
void my_sleep(char **tokens, int i);

int main(int argc, char *argv[])
{
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;

	FILE *fp;
	if (argc >= 2)
	{
		fp = fopen(argv[1], "r");
		if (fp < 0)
		{
			printf("File doesn't exists.");
			return -1;
		}
	}
	int count = 8;
	while (count--)
	{
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		if (argc == 2)
		{ // batch mode
			if (fgets(line, sizeof(line), fp) == NULL)
			{ // file reading finished
				break;
			}
			line[strlen(line) - 1] = '\0';
		}
		else
		{ // interactive mode
			printf("$ ");
			scanf("%[^\n]", line);
			getchar();
		}
		printf("Command entered: %s\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);

		//do whatever you want with the commands, here we just print them

		for (i = 0; tokens[i] != NULL; i++)
		{
			printf("found token %s\n", tokens[i]);

			if (strcmp(tokens[i], "cd") == 0)
			{
				i = cd(tokens[i], i);
			}
			else if (strcmp(tokens[i], "echo") == 0)
			{
				i = echo(tokens, i);
			}
			if (strcmp(tokens[i], "sleep") == 0)
			{
				i++;
				// my_sleep(tokens, i);
				int sleepNo = stoi(tokens[i]);
				if (sleepNo > MAX_NUM_TOKENS)
				{
					printf("Shell: Incorrect command \n");
					// perror("Sleeping cannot be done for such long period \n");
					/*What to replace here?????*/
				}
				sleep(sleepNo);
			}
			else
			{
				printf("Shell: Incorrect command \n");
			}
		}

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);
	}
	return 0;
}

int echo(char **tokens, int tokenNo)
{
	pid_t pid, wpid;
	int status;
	int i;

	pid = fork();
	if (pid == 0)
	{
		int start = 0;
		int stop = 0;
		// tokenNo contains the word echo;
		for (i = tokenNo + 1; tokens[i] != NULL; i++)
		{
			char *currentWord = tokens[i];
			for (int j = 0; j < strlen(currentWord); j++)
			{
				char readchar = currentWord[j];
				// Handling encounter with "
				if (readchar == '"')
				{
					// first encounter
					if (start == 0)
					{
						start = 1;
						continue;
					}
					// last encounter
					else
					{
						stop = 1;
						printf("\n");
						exit(EXIT_SUCCESS);
						// i returns the word index last echoed
					}
				}
				// if immediately, we dont find ", give error
				else if (start == 0)
				{
					exit(EXIT_FAILURE);
					// error for did not find "
				}
				else
				{
					printf("%c", readchar);
				}
			}
			printf(" ");
		}
		printf("\n");
		// did not end with "
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("error forking");
	}
	else
	{
		printf("%d \n", pid);
		// do
		// {
		// 	wpid = waitpid(pid, &status, WUNTRACED);
		// } while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return i;
}

// function to check the current working directory
int pwd()
{
	char cwd[MAX_INPUT_SIZE];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
	}
	else
	{
		perror("getcwd() error");
		return -1;
	}
	return 0;
}

void ls()
{
	struct dirent *de; // Pointer for directory entry

	// opendir() returns a pointer of DIR type.
	DIR *dr = opendir(".");

	if (dr == NULL) // opendir returns NULL if couldn't open directory
	{

		printf("Shell: Incorrect command \n");
		// printf("Could not open current directory");
		return;
	}

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
	// for readdir()
	while ((de = readdir(dr)) != NULL)
		printf("%s\n", de->d_name);

	closedir(dr);
	printf("is using direct.h header okay??? \n");
	return;
}

int cd(char **tokens, int i)
{

	if (chdir(tokens[i]) == -1)
	{
		printf("Shell: Incorrect command \n");
		// perror("no such directory \n");
		/*What to replace here????? */
	}
	else
	{
		// success
		pwd();
		/*What to replace here????? */
	}
	return i + 2;
}

// void my_sleep(char **tokens, int i)
// {
// 	pid_t pid, wpid;
// 	int status;

// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		int sleepNo = stoi(tokens[i + 1]);
// 		if (sleepNo > MAX_NUM_TOKENS)
// 		{
// 			printf("Shell: Incorrect command \n");
// 			// perror("Sleeping cannot be done for such long period \n");
// 			/*What to replace here?????*/
// 		}
// 		sleep(sleepNo);
// 		exit(EXIT_FAILURE);
// 	}
// 	else if (pid)
// 	{
// 		wpid = waitpid(pid, &status, 0);
// 		printf("%d reaped \n", wpid);
// 	}

// 	else if (pid < 0)
// 	{
// 		perror("error forking");
// 	}
// }