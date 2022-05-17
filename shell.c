#include<stdio.h>
#include <unistd.h>
#include<string.h>
#include<stdlib.h>

#define MAX_LINE 80
#define MAX_HISTORY 10

int input(char* str);
int str_to_args(char* str,char* args[],int str_num);
void display_history(int history_front,int history_rear);
void add_history(char* str,int* front,int* rear);

char history[10][MAX_LINE];//history queue

int input(char* str){
	char c;
	int i = 0;
	while((c = getchar())!='\n' && i<MAX_LINE){
		str[i] = c;
		i++;
	}
	if(i == MAX_LINE && c != '\n'){//when error
		printf("too long!");
		return 0;
	}	
	else{
		str[i] = 0;
		return i;
	}
}

int str_to_args(char* str,char* args[],int str_num){
	const char s[2] = " ";
	int i = 0;
	char* temp;

	temp = strtok(str,s);
	while(temp != NULL){
		args[i] = (char*)malloc(strlen(temp));
		strcpy(args[i],temp);
		i++;
		temp = strtok(NULL,s);
	}
	args[i] = 0;
	return i;
}
void display_history(int history_front,int history_rear){
	int i;
	for(i = history_front;i < history_rear;i++){
		printf("%d\t%s\n",history_rear - i,history[i%10]);
	}
}
void add_history(char* str,int* front,int* rear){
	strcpy(history[*rear % 10],str);//add
	*rear = *rear+1;
	if(*rear - *front > 10)
		*front++;
}

int main(void){
	char* args[MAX_LINE/2+1];//execvp's argslist
	int should_run = 1;
	int args_num = 0;

	char str[MAX_LINE];// user input
	int str_num;

	int history_front = 0;
	int history_rear = 0;
	
	int i = 0;
	int background = 0;
	int pid_status;

	while(should_run){
		printf("osh>");
		fflush(stdout);
		
		str_num = input(str);//get user input

		if(str_num == 0){//no input
			continue;
		}
		if(strcmp(str,"exit")== 0){//exit
			should_run = 0;
			continue;
		}
		if(strcmp(str,"history") == 0){//display history
			display_history(history_front,history_rear);
		}
		if(strcmp(str,"!!") == 0){//use recent command
			if(history_rear != 0){
				strcpy(str,history[(history_rear -1) % 10]);
				str_num = strlen(str);
			}
			else{
				printf("No commands in history.\n");
			}
		}
		else if(str[0] == '!'){//use history command
			if(str[1] <= '0' || str[1] > '9' || (str[1]-'0') > history_rear-history_front){
				printf("No such command in history.\n");
			}
			else if(str[1] == '1'){
				if(str[2] == '0')
					strcpy(str,history[history_front %10]);
				else if(str[2] == 0)
					strcpy(str,history[(history_rear-1) %10]);
				else
					printf("No such command in history.\n");
			}
			else
				strcpy(str,history[history_rear - str[1] + '0' -1]);
		}

		add_history(str,&history_front,&history_rear);//add to history

		args_num = str_to_args(str,args,str_num);

		if(strcmp(args[args_num-1],"&") == 0){
			background = 1;
			args_num--;
			args[args_num] = NULL;
		}

		pid_t pid = fork();
		if(pid == 0){
			pid_status = execvp(args[0], args);
		}
		else{
			if(background == 1){
				printf("%d %s \n",pid,str);
			}
			else{
				wait(&pid_status);
			}
		}
		background  = 0;
	}
	return 0;
}
