#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
char command[MAX_COMMAND_LENGTH];

int main(int argc, char *argv[]) {
    // Pipe for communication between C and E
    int pipe_fd[2],pipe_bd[2];
    int stout,stin;

    // Fork the first child (C mode)
    if(argc==1&&strcmp(argv[0],"./CSE")==0)
    {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            if (pipe(pipe_bd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            char fd0[3],fd1[3],bd0[3],bd1[3];
            sprintf(fd0,"%d",pipe_fd[0]);
            sprintf(fd1,"%d",pipe_fd[1]);
            sprintf(bd0,"%d",pipe_bd[0]);
            sprintf(bd1,"%d",pipe_bd[1]);
            pid_t child_C_pid = fork();
            if (child_C_pid == 0) {
                // Child process (C mode)
                    execlp("xterm", "xterm", "-T", "\"Child C\"","-e","./CSE","C",fd0,fd1,bd0,bd1, NULL);
               
            } else {
                // Fork the second child (E mode)
                pid_t child_E_pid = fork();
                if (child_E_pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }

                if (child_E_pid == 0) {
                    // Child process (E mode)
                        execlp("xterm", "xterm", "-T", "\"Child E\"","-e","./CSE","E",fd0,fd1,bd0,bd1, NULL);
                    
                } else {
                    // Parent process (Supervisor)
                    if(argc==1&&strcmp(argv[0],"./CSE")==0){
                    printf("+++ CSE in supervisor mode: Started\n");
                    printf("+++ CSE in supervisor mode: Forking first child in command-input mode\n");
                    }

                    // Close unused ends of the pipe
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);

                    // Wait for both child processes to terminate
                    waitpid(child_C_pid, NULL, 0);
                    waitpid(child_E_pid, NULL, 0);
                    printf("Supervisor process: Both children terminated.\n");

                }
            }
    }else{
        if(strcmp(argv[1],"C")==0)
        {
            while(1){
                    // stout=dup(STDOUT_FILENO);
                    sscanf(argv[2],"%d",&pipe_fd[0]);
                    sscanf(argv[3],"%d",&pipe_fd[1]);
                    sscanf(argv[4],"%d",&pipe_bd[0]);
                    sscanf(argv[5],"%d",&pipe_bd[1]);
                    while (1) {
                        fputs("Enter command: ", stderr);
                        for(int i=0;i<MAX_COMMAND_LENGTH;i++)command[i]='\0';
                        fgets(command,sizeof(command),stdin);
                        write(pipe_fd[1], command, strlen(command));
                        if(strcmp(command,"swaprole\n")==0)break;
                        else if(strcmp(command,"exit\n")==0)exit(EXIT_SUCCESS);
                    }
 

                    while (1) {
                       
                        for(int i=0;i<MAX_COMMAND_LENGTH;i++)command[i]='\0';
                        read(pipe_bd[0], command, MAX_COMMAND_LENGTH);
                        if(strcmp(command,"swaprole\n")==0)break;
                        else if(strcmp(command,"exit\n")==0)exit(EXIT_SUCCESS);
                        system(command);

                    }
                   
            }

                exit(EXIT_SUCCESS);
        }
        else if(strcmp(argv[1],"E")==0)
        {
            while(1){
                // stin=dup(STDIN_FILENO);
                sscanf(argv[2],"%d",&pipe_fd[0]);
                sscanf(argv[3],"%d",&pipe_fd[1]);
                sscanf(argv[4],"%d",&pipe_bd[0]);
                sscanf(argv[5],"%d",&pipe_bd[1]);
                while (1) {
                   
                    for(int i=0;i<MAX_COMMAND_LENGTH;i++)command[i]='\0';
                    read(pipe_fd[0], command, MAX_COMMAND_LENGTH);
                    if(strcmp(command,"swaprole\n")==0)break;
                    else if(strcmp(command,"exit\n")==0)exit(EXIT_SUCCESS);
                    system(command);
                    
                }
                while (1) {
                        for(int i=0;i<MAX_COMMAND_LENGTH;i++)command[i]='\0';
                        fputs("Enter command: ", stderr);
                        fgets(command,sizeof(command),stdin);
                        write(pipe_bd[1], command, strlen(command));
                        if(strcmp(command,"swaprole\n")==0)break;
                        else if(strcmp(command,"exit\n")==0)exit(EXIT_SUCCESS);
                }
                // close(stout);
            }
            
        }
    }

    return 0;
}
