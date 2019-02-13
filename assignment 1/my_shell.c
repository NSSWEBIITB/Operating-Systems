/*What to replace here?????
 *questions to be asked in class have '???'
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
int numberOfParallel = 0;
pid_t pidarr[64];
// int numberOfForeground = 0;
// pid_t foregroundArr[64];
int numberOfBackground = 0;
pid_t backgroundArr[64];

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
char **tokenize(char *line, int *parallel, int *background)
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
				if (strcmp(token, "&&&") == 0)
				{
					*parallel = 1;
				}
				if (strcmp(token, "&") == 0)
				{
					*background = 1;
				}
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

void killAll(pid_t[], int);

char *processecho(char **, int, int *);
void pwd();
int exec(int, int, char *, char **, int);

void my_sleep(int);
void sighandler()
{
	killAll(pidarr, numberOfParallel);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sighandler);

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
	int exit = 0;
	while (!exit)
	{
		int parallel = 0;
		int background = 0;
		numberOfParallel = 0;

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
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line, &parallel, &background);

		//do whatever you want with the commands, here we just print them

		for (i = 0; tokens[i] != NULL; i++)
		{
			// printf("found token %s\n", tokens[i]);
			if (strcmp(tokens[i], "&") == 0)
			{
				background = 1;
			}
			else if (strcmp(tokens[i], "&&") == 0)
			{
				continue;
			}
			else if (strcmp(tokens[i], "&&&") == 0)
			{
				parallel = 1;
			}

			else if (strcmp(tokens[i], "cd") == 0)
			{ // done
				chdir(tokens[++i]);
			}

			else if (strcmp(tokens[i], "exit") == 0)
			{ //done
				killAll(pidarr, numberOfParallel);
				killAll(backgroundArr, numberOfBackground);
				exit = 1;
				break;
			}

			else if (strcmp(tokens[i], "sleep") == 0)
			{ //done
				i++;
				char *array[1] = {tokens[i]};
				exec(parallel, background, "sleep", array, 1);
			}

			else if (strcmp(tokens[i], "pwd") == 0)
			{ // done
				exec(parallel, background, "pwd", NULL, 0);
			}

			else if (strcmp(tokens[i], "date") == 0)
			{ // done
				exec(parallel, background, "date", NULL, 0);
			}

			else if (strcmp(tokens[i], "ps") == 0)
			{ // done
				exec(parallel, background, "ps", NULL, 0);
			}

			else if (strcmp(tokens[i], "ls") == 0)
			{ //done
				if (tokens[i++] != NULL)
				{
					char *array[1] = {tokens[i]};
					i += exec(parallel, background, "ls", array, 1);
				}
				else
				{
					exec(parallel, background, "ls", NULL, 0);
				}
			}

			else if (strcmp(tokens[i], "cat") == 0)
			{ //done
				if (tokens[i++] != NULL)
				{
					char *array[1] = {tokens[i]};
					i += exec(parallel, background, "cat", array, 1);
				}
			}

			else if (strcmp(tokens[i], "echo") == 0)
			{ //done
				int length = 0;
				char *string = processecho(tokens, i, &length);
				char *array[1] = {string};
				exec(parallel, background, "echo", array, 1);
				i += length;
			}

			else
			{
				printf("Shell: Incorrect command: %s\n", tokens[i]);
			}
		}

		// Freeing the allocated memory
		for (i = 0; tokens[i] != NULL; i++)
		{
			free(tokens[i]);
		}
		free(tokens);

		for (size_t i = 0; i < numberOfParallel; i++)
		{
			pid_t wpid = waitpid(pidarr[i], 0, 0);
			numberOfParallel--;
			pidarr[i] = 0;
		}
	}
	return 0;
}

char *processecho(char **tokens, int tokenNo, int *number)
{
	int start = 0;
	int stop = 0;
	static char word[1024];
	int pointer = 0;
	int length = 0;

	// tokenNo contains the word echo;
	for (int i = tokenNo + 1; tokens[i] != NULL; i++)
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
					word[pointer] = '\0';
					*number = length + 1;
					return word;
					// last bit is the number of words
				}
			}
			// if immediately, we dont find ", give error
			else if (start == 0)
			{
				return NULL;
				// error for did not find "
			}
			else
			{
				word[pointer++] = readchar;
			}
		}
		word[pointer++] = ' ';
		length++;
	}
	word[pointer] = '\0';
	*number = length + 1;
	return word;
	// 0 for loop did not end
}

int exec(int parallel, int background, char *command, char **args, int size)
{
	pid_t pid, wpid;
	int status;
	int i;
	char *new_arg[size + 2];

	new_arg[0] = command;
	for (i = 0; i < size; i++)
	{
		new_arg[i + 1] = args[i];
	}
	new_arg[i + 1] = NULL;

	pid = fork();
	if (pid == 0)
	{
		execvp(command, new_arg);
	}
	else if (pid)
	{
		if (parallel == 1)
		{ // if parallel
			pidarr[numberOfParallel] = pid;
			numberOfParallel++;

			setpgid(pid, getpid());
		}
		else if (background == 1)
		{ // if background
			backgroundArr[numberOfBackground] = pid;
			numberOfBackground++;
		}
		if (parallel != 1 && background != 1)
		{ // if blocking, reap it before going ahead
			wpid = waitpid(pid, &status, 0);
		}
	}
	return size;
}

void killAll(pid_t processes[], int number)
{
	for (int i = 0; i < number; i++)
	{
		kill(processes[i], SIGKILL);
		printf("killed %d \n", processes[i]);
	}
}