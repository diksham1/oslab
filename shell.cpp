#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <sys/wait.h>
#include <cstring>
#include <unistd.h>

//#define MAX_BUFFER_SIZE 100

using namespace std;

#define MAX_NUM_TOKENS 100
#define MAX_TOKEN_SIZE 100
#define MAX_COMMAND_SIZE 100


/* ......................................................................................
*
*  Breaks the input string into tokens and adds a NULL at the end
*  e.g echo hello world will be broken down into {"echo", "hello world", NULL}
*  (Strings are NULL terminated) 
*
*  ......................................................................................
*/


char ** nullTerminatedTokenize(char* command) {
	// One extra token for holding NULL pointer at the end
	char **args = (char **) malloc((MAX_NUM_TOKENS+1) * sizeof(char *));
	char *token = (char *) malloc(MAX_TOKEN_SIZE * sizeof(char));
		
	int tokenIndex = 0;
	int tokenNum = 0;
	
	for (int i = 0; command[i] != '\0'; i++) {
		char readChar = command[i];
		if(tokenNum == 0 && (readChar == ' ' || readChar == '\t' || readChar == '\n')) {
			token[tokenIndex] = '\0';
			args[tokenNum++] =	token;
			token = (char *) malloc(MAX_TOKEN_SIZE * sizeof (char)); 
			tokenIndex = 0;
		} else {
			token[tokenIndex++] = readChar;
		}
	}
 
	token[tokenIndex] = '\0';
	args[tokenNum++] = token;
	token = NULL;
	args[tokenNum++] = NULL;

	return args;
	
}

/* .....................................................................................
*
*  Processes the command recieved as input from the user
*  INPUT: string with command and arguments, 
*  e.g "echo hello world" to display "hello world" (without quotes) on the screen
*
*  .....................................................................................
*/

void processCommand(char* command) {
	char **args = nullTerminatedTokenize(command);
	if (strcmp(args[0], "cd") == 0) {
		chdir(args[1]);
		return;
	}
	pid_t childPID = fork();
	assert(childPID != -1);
	
	if (childPID == 0) {
		int exitCode = 0;
		exitCode = execvp(args[0], args);
		exit(exitCode);
	} else {
		pid_t childExitCode = wait(NULL);
		if (childExitCode == -1) {
			cout <<"Command not found\n";
		}
	}
}

/* .........................................................................................
*
*  The main function 
*  Supports: Interactive mode and Batch Mode
*  Interactive Mode: User gives command through shell prompt
*  Batch Mode: Batch file containing commands is fed as command line argument to main
*  Once done with processing the batch file commands, shells drops back to interactive mode
*
*  .........................................................................................
*/

int main(int argc, char *argv[]) {
	ifstream file;

	if (argc == 2) {
		string fname = argv[1];
		file.open(fname);
	}
	
	if (argc < 2 || !file) {
interact:	while (1) {
			char* cmd = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char));
			cout <<"Diksha@Tux:~" << get_current_dir_name() <<"$ ";
			cin.getline(cmd, MAX_COMMAND_SIZE);
			if (cmd == "\n") {
				cout << '\n';
				continue;
			}
			processCommand(cmd);	
		}
	}

	else {
		while (1) {	
			char* cmd = (char *)malloc(MAX_COMMAND_SIZE*sizeof(char));
			file.getline(cmd,MAX_COMMAND_SIZE);
			if (file.eof())	break;
			processCommand(cmd);
		}
		goto interact;
	}

	return 0;
} 
