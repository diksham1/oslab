#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>


struct chat {
	char data[100];
	int ready;
};

struct chat *sharedObj;
int id;
int pid = 0;

void printRecievedData(int signum) {
	if (signum == SIGUSR1) {
		if (sharedObj -> ready) {
			printf("\t\t\t\t%s",sharedObj -> data);
		}
	}
}

void chatEnd(int signum) {
	if (signum == SIGUSR2) {
		printf("User %d left the chat\n", pid);
		exit(0);
	}
}

void destroy(int signum) {
	if (signum == SIGINT) {	
		shmdt((void *)sharedObj);
		shmctl(id, IPC_RMID, NULL);
		kill(pid, SIGUSR2);
		exit(0);
	}
}

int main () {
	int key = 19876;
	id = shmget(key, sizeof(struct chat), IPC_CREAT | 0666);
	sharedObj = (struct chat *) shmat(id, NULL, 0);

	signal(SIGUSR2, chatEnd);
	signal(SIGUSR1, printRecievedData);
	signal(SIGINT, destroy);
	assert((int)sharedObj != -1);

	printf("Hello, My PID is %d \n", getpid());
	printf("Enter the pid of the other user: ");
	scanf("%d\n", &pid);
	fflush(stdin);

	while (1) {
//		printf("%d: ", getpid());
		fgets(sharedObj -> data, sizeof(sharedObj -> data), stdin);
		fflush(stdin);
		sharedObj -> ready = 1;
		kill(pid, SIGUSR1);
	}
		
	return 0;
}
