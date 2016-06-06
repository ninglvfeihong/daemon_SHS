/*
 ============================================================================
 Name        : daemon_SHS.c
 Author      : Xiao Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
//asynchronized recycle zombie process to avoid zombie process
void handler(int num){
	int status;
	//wait all the dead processes, clean them
	while(waitpid(-1,&status,WNOHANG)>0);
}

int main(int argc, char **argv)
{
    int ret, i, status;
    char *child_argv[100] = {0};
    pid_t pid;
    pid = fork();
    if(pid<0){
    	printf("daemon_smart fork failed\n");
    	exit(EXIT_FAILURE);//exit(1)
   	}else if(pid>0){
   		//parent process
   		exit(EXIT_SUCCESS);//exit(0)
   	}else{
   		//this is the daemon process
   		printf("daemon_SHS fork success\n");
   		close(0);//std input
   		close(1);//std output
   		open ("/dev/null", O_RDWR);
   		/*dup stdout */
   		dup (0);
   		//avoid zombie process
   		signal(SIGCHLD,handler);
   		//clear the log file
   		FILE* file =fopen("/etc/SHS/shs.log","w");
   		if(file){
			fclose(file);
   		}
   	}

    child_argv[0] = "./SHS_cpp";
    child_argv[1] =NULL;
    while(1){

        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "fork() error.errno:%d error:%s\n", errno, strerror(errno));
        }
        if (pid == 0) {
            ret = execv(child_argv[0], (char **)child_argv);
            if (ret < 0) {
                fprintf(stderr, "execv ret:%d errno:%d error:%s\n", ret, errno, strerror(errno));
                continue;
            }
            exit(0);
        }
        if (pid > 0) {
            pid = wait(&status);

            fprintf(stdout, "wait return");
            sleep(15);
        }

    }
    return 0;
}
