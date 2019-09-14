#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>


//#define MAX_BUFFER_SIZE 100

using namespace std;

void processCommand(string command) {
	pid_t childPID = fork();
	assert(childPID != -1);

	if (childPID == 0) {
		string cmd = "/bin/" + command;
		char cstr[cmd.size() + 1];
		copy(cmd.begin(), cmd.end(), cstr);
		cstr[cmd.size()] = '\0';
		char *args[] = {cstr, NULL};
		execv(args[0], args);
	}
}

int main(int argc, char *argv[]) {
	ifstream file;

	if (argc == 2) {
		string fname = argv[1];
		file.open(fname);
	}
	
	if (argc < 2 || !file) {		
interact:	while (1) {
			string cmd;
			cout <<"$ ";
			getline(cin, cmd);
			if (cmd == "\n")	continue;
			processCommand(cmd);	
		}
	}

	else {
		while (1) {	
			string cmd;
			getline(file, cmd, '\n');
			if (file.eof())	break;
			processCommand(cmd);
			cout << endl;
		}
		goto interact;
	}

	return 0;
} 
