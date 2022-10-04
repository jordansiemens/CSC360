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

 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	Node* n = (Node*)malloc(sizeof(Node));

	n->pid = new_pid;
	n->path = strdup(new_path);
	n->next = NULL;
	n->flag = " ";

	if (head == NULL) {
		return n;
	} else {
				Node* temp = head;
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = n;
	}
	return n; // Change this, DO NOT return NULL !
}


Node * deleteNode(Node* head, pid_t pid){
	
	Node* temp = head;
	Node* prev = NULL;

	while (temp->pid != pid) {
		if (temp->next == NULL) {
			printf("pid does not exist");
			return NULL;
		}
		prev = temp;
		temp = temp->next;
	}

	if (head == temp) {
		head = head->next;
		free(temp);
	} else {
		prev->next = temp->next;
		free(temp);
	}

	return head;
}

void addFlag(Node *node, int pid, char *flag){
	Node* temp = node;
	while (temp != NULL){
		if (temp->pid == pid){
			temp->flag = flag;
		}
		temp = temp->next;
	}
}

void printList(Node *node){
	Node *temp = node;
	int count = 0;
	
	while (temp != NULL){
		count++;
		printf("%d: %s %s\n",temp->pid,temp->path,temp->flag);
		temp = temp->next;
	}
	printf("Total background processes: %d\n", count);
}


int PifExist(Node *node, pid_t pid){
	Node* temp = node;
	while (temp != NULL){
		if (temp->pid == pid){
			return 1;
		}
		temp = temp->next;
	}
  	return 0;
}

