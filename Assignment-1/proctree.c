#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<string.h>

int main(int argc,char *argv[])
{    
    //input tree root;
    char buff[100];
    pid_t mainpid,cpid,pid;
    //opening file
    FILE *file=fopen("treeinfo.txt","r");
    if(argc<=1)
    {
        printf("Run with a node name");
    }
    else if(argc>=2){
        mainpid=getpid();
        int gap=0,f=0;
        if(argc==3){
            gap=atoi(argv[2]);
        }
        for(int i=0; i<gap; i++){
                for(int j=0; j<4; j++){
                    printf(" ");
                }
        }
        while(fgets(buff,sizeof(buff),file))
        {
            // printf("%s\n",buff);
            char *tok=strtok(buff," ");

            if(strcmp(tok,argv[1])==0)
            {
                f=1;
               printf("%s(%d)\n",argv[1],mainpid);
                tok=strtok(NULL," ");
               int num=atoi(tok);

            for(int i=0;i<num;i++)
            {
                
                    int status;
                   pid=fork();
                   tok=strtok(NULL," ");
                   if(pid==0)
                   {
                    if(i==num-1)
                    {
                            tok[strlen(tok)-1]='\0';}
                        char s[3];
                        sprintf(s,"%d",gap+1);
                        execl("./proctree","proctree",tok, s,NULL);
                        exit(i);
                    
        
                   }
                   else{
            
                    waitpid(pid,&status,0);
                   }
                
            
            }
            }
            
            
        }
        if(f==0)
        {
            printf("City %s not found",argv[1]);
        }
       
    }    
    
    fclose(file); 
    return 0;
}