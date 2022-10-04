#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

Node* head = NULL;

void checkStatus(){ //Function idea from A1 Q&A 6)
  int status;
  pid_t pid = waitpid(-1, &status, WNOHANG|WCONTINUED|WUNTRACED);
  
  if (pid > 0) {
    if (WIFEXITED(status)) {
     		printf("exited by signal %d\n", WEXITSTATUS(status));  // Display the status code of child process
     		deleteNode(head,pid);
    	} else if (WIFSIGNALED(status)) {
    		printf("killed by signal %d\n", WTERMSIG(status));
    		addFlag(head,pid,"killed");	
    	} else if (WIFSTOPPED(status)) {
    		printf("stopped by signal %d\n", WSTOPSIG(status));
    		addFlag(head,pid,"stopped");
    	} else if (WIFCONTINUED(status)){
    	  printf("process %d, has been continued\n",pid);
    		addFlag(head,pid,"continued");
    	}


  }
}

void func_BG(char **cmd){
  	char *file = cmd[1];

  	if (cmd[1]==NULL){
  	  printf("Error: missing parameters\n");
  	  return;
  	}
    
    pid_t pid = fork();

  	if (pid == 0) { // Child Process
  	  //printf("Child process started \n");
  	  if (execvp(file,&cmd[1]) < 0) { //From A1 Q&A 8a)
  	      printf("Error: Execution of %s failed \n", file);
  	      exit(-1);
  	      }
  	  printf("Error finding command\n");
  	  exit(1);

  	} else if (pid > 0) { // Parent Process
  	  printf("Process: %d, has started in the background \n",pid);
  	  if (head == NULL) {
  	    head = add_newNode(NULL,pid,file);
  	  }
  	  else{
  	    add_newNode(head, pid, file);
  	  }
  	} else {
  	  printf("Error forking\n");
  	}
  	checkStatus();
}


void func_BGlist(char **cmd){
	printList(head);
	checkStatus();
}


void func_BGkill(char* str_pid){
      if (str_pid == NULL){
     	printf("Error: missing parameters\n");
	return;
      }
  pid_t pid = atoi(str_pid);
	int boolVal = PifExist(head, pid);
	if (boolVal != 0){
	  int eVal = kill(pid, SIGKILL); //Signal to kill process
	  if (eVal != 0){
	    printf("bgstop failure\n");
	  }
	}else {
    printf("Error: pid is not a running process\n");
	}
	checkStatus(); 
}


void func_BGstop(char  *str_pid){
      if (str_pid == NULL){
     	printf("Error: missing parameters\n");
	return;
      }
  pid_t pid = atoi(str_pid);
	int boolVal = PifExist(head, pid);
	if (boolVal != 0){
	  int eVal = kill(pid, SIGSTOP); //Signal to stop process
	  if (eVal != 0){
	    printf("bgstop failure\n");
	  }
	}else {
    printf("Error: pid is not a running process\n");
	}
	checkStatus(); 
}


void func_BGstart(char *str_pid){
      if (str_pid == NULL){
     	printf("Error: missing parameters\n");
	return;
      }
  pid_t pid = atoi(str_pid);
	int boolVal = PifExist(head, pid);
	if (boolVal != 0){
	  int eVal = kill(pid, SIGCONT); //Signal to continue process
	  if (eVal != 0){
	    printf("bgstart failure\n");
	  }
	}else {
    printf("Error: pid is not a running process\n");
	}
	checkStatus(); 
}


void func_pstat(char *  str_pid){
      if (str_pid == NULL){
     	printf("Error: missing parameters\n");
	return;
      }
	pid_t pid = atoi(str_pid);
	int boolVal = PifExist(head,pid);
	char path1[128]; //Path to stat fie
	char path2[128]; //Path to status file
	char *statArr[128]; //Array to save contents from stat file
	char statusMatr[64][128]; //64x128 Matrix to save the Name and Data from status file strcute [name][data] 
	int i = 0; //while loop increment

	if (boolVal != 0){
    sprintf(path1, "/proc/%d/stat", pid); 
    sprintf(path2, "/proc/%d/status", pid); 
   
    //read stat file (Method borrowed from stack overflow)
    char statBuffer[1024];
    FILE *fp = fopen(path1,"r");
    if (fp != NULL){
      while (fgets(statBuffer, sizeof(statBuffer),fp) != NULL ){
        char *tok = strtok(statBuffer," ");
        statArr[i] = tok;
        while (tok != NULL){
          statArr[i] = tok;
          tok = strtok(NULL, " ");
          i++;
        }
      }
      fclose(fp);
    }

    //read status file
    char line[128];
    fp = fopen(path2,"r");
    int j = 0;
    if (fp != NULL){
      while (fgets(statusMatr[j],sizeof(line),fp) != NULL){
        j++;
      }
      fclose(fp);
    }
      printf("comm: %s\n",statArr[1]);
      printf("state: %s\n",statArr[2]);
      printf("utime :%lf\n",((float)*statArr[13] / sysconf(_SC_CLK_TCK)));
      printf("stime :%lf\n",((float)*statArr[14] / sysconf(_SC_CLK_TCK))); //from a1 q&a
      printf("voluntary_ctxt_switches: %d\n", atoi(statusMatr[54]));
      printf("nonvoluntary_ctxt_switches: %d\n", atoi(statusMatr[55]));
	} else {
	  printf("Error: pid is not a running process\n");
	}
}

 
int main(){
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (lst[0] == NULL){
     	printf("Error: missing parameters\n");
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
    }

  return 0;
}

