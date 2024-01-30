#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_JOBS 10

struct Job {
    pid_t pid;
    pid_t pgid;
    int status;
    char argument;
};

struct Job PT[MAX_JOBS + 1]; // Process table with one extra entry for the manager
int currentJobIndex = -1;    // Index of the current job in the process table

void handle_sigint(int signum) {
        int i;
        for (i = 0; i <= currentJobIndex;i++) {
        if (PT[i].status==0) {
            kill(PT[i].pid, SIGINT);
            return;
        }
    }
    printf("\nmgr> ");
    fflush(stdout);
}

void handle_sigtstp(int signum) {
        int i;
        for (i = 0; i <= currentJobIndex;i++) {
        if (PT[i].status==0) {
            kill(PT[i].pid, SIGTSTP);
            return;
        }
    }
    printf("\nmgr> ");
    fflush(stdout);
}

void help()
{
    printf("Command : Action\n");
        printf("c : Continue a suspended job\n");
        printf("h : Print this help message\n");
        printf("k : Kill a suspended job\n");
        printf("p : Print the process table\n");
        printf("q : Quit\n");
        printf("r : Run a new job\n");
}

int main() {
    // Set up signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);

    // Main loop for user input
    char input[10];
    while (1) {
        printf("\nmgr> ");
        fgets(input, sizeof(input), stdin);

        if (strcmp(input, "p\n") == 0) {
            // Print process table
            printf("Printing process table...\n");
            printf("NO\t\tPID\t\tPGID\t\tSTATUS\t\tNAME\n");
            printf("0\t\t%d\t\t%d\t\tSELF\t\tmgr\n",getpid(),getpid());
            int i;
            for (i = 0; i <= currentJobIndex; i++) {
                char* st="";
                if(PT[i].status==4)st="FINISHED";
                else if(PT[i].status==3)st="TERMINATED";
                else if(PT[i].status==2)st="SUSPENDED";
                else if(PT[i].status==1)st="KILLED";
                printf("%d\t\t%d\t\t%d\t\t%s\t\tjob %c\n",
                       i+1, PT[i].pid, PT[i].pgid,st , PT[i].argument);
            }
        } else if (strcmp(input, "r\n") == 0) {
            // Start a new job
            if(currentJobIndex>=MAX_JOBS)
            {
                printf("Cant anymore");
                exit(0);
            }
            char array[2];
            array[0]='A'+rand()%26;
            array[1]='\0';
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                setpgid(0,getpid());
                
                execl("./job", "job", array,NULL);
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                // Parent process
                setpgid(pid, pid); // Set child's process group ID to its PID
                currentJobIndex++;

                PT[currentJobIndex].pid = pid;
                PT[currentJobIndex].pgid = pid;
                PT[currentJobIndex].status = 0;
                PT[currentJobIndex].argument = array[0];
                waitpid(pid, &(PT[currentJobIndex].status),WUNTRACED);
                // Check if the child process terminated normally
                if (WIFEXITED(PT[currentJobIndex].status)) {
                PT[currentJobIndex].status=4;
                } 
                // Check if the child process was stopped
                else if (WIFSTOPPED(PT[currentJobIndex].status)) {
                    PT[currentJobIndex].status=2;
                    // Handle the stopped state as needed
                } 
                    // Check if the child process terminated due to a signal
                else if (WIFSIGNALED(PT[currentJobIndex].status)) {
                    PT[currentJobIndex].status=3;
                    // Handle the terminated state as needed
                }
                
            } else {
                // Error
                perror("fork");
            }
        } else if (strcmp(input, "c\n") == 0) {
            printf("Suspended jobs: ");
            int i;
            for (i = 0; i <= currentJobIndex; i++) {
                if(PT[i].status==2)
                {
                    printf("%d ",i+1);
                    printf(",");
                }                
            }
            
            printf(" (Pick one):");
            int x;
            scanf("%d",&x);
            PT[x-1].status=0;
            kill(PT[x-1].pid, SIGCONT);
            waitpid(PT[x-1].pid, &(PT[x-1].status),WUNTRACED);
            // Check if the child process terminated normally
            if (WIFEXITED(PT[x-1].status)) {
                PT[x-1].status=4;
            } 
            // Check if the child process was stopped
            else if (WIFSTOPPED(PT[x-1].status)) {
                PT[x-1].status=2;
                // Handle the stopped state as needed
            } 
            // Check if the child process terminated due to a signal
            else if (WIFSIGNALED(PT[x-1].status)) {
                PT[x-1].status=3;
                // Handle the terminated state as needed
            }

        } else if (strcmp(input, "k\n") == 0) {
            printf("Suspended jobs: ");
            int i;
            for (i = 0; i <= currentJobIndex; i++) {
                if(PT[i].status==2)
                {
                    printf("%d ",i+1);
                    printf(",");
                }                
            }
            
            printf(" (Pick one):");
            int x;
            scanf("%d",&x);
            PT[x-1].status=1;
            kill(PT[x-1].pid, SIGINT);

        } else if (strcmp(input, "h\n") == 0) {
            printf("Help message...\n");
            help();
        } else if (strcmp(input, "q\n") == 0) {
            printf("Exiting...\n");
            exit(0);
            break;        
        } 
        else if (strcmp(input, "q\n") == 0) {
            printf("Exiting...\n");
            exit(0);
            break;
        
        }else {
            printf("Invalid command\n");
        }
    }

    return 0;
}

