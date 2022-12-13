#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXSIZE 50
#define STDIN_FILENO 0  //표준입력
#define STDOUT_FILENO 1 //표준출력


int main(int argc, char** argv) {

	char cmd[MAXSIZE];
	char *cmd_string[2];
	char buff[MAXSIZE][MAXSIZE] = { NULL };
	char command[MAXSIZE] = { NULL }; //명령어
	char files[MAXSIZE][MAXSIZE] = { NULL };
	int filedes[2]; // 파이프용
	int i,j,z,num;
	int pid[MAXSIZE], status;
	int file, file2;
	int amper=0;
	
	int redirection = 0;

	//while (1) {
		
	//}


	while (1) {
	
		num = 0; //자식 프로세스 관리
		amper = 0; //백그라운드 변수
		redirection = 0; // redirection 검사
	
		printf("$ ");

		//scanf("%s", &cmd); getchar();

		fgets(cmd, sizeof(cmd), stdin); //문자열 입력받음
		cmd[strlen(cmd) - 1] = '\0'; //개행문자 제거
		
		//strcpy(cmd_string, cmd);

		//printf("%s", cmd);

		i = 0;
		char* ptr = strtok(cmd, " ");
		while (ptr != NULL) {
			strcpy(buff[i], ptr);

			if (strcmp(buff[i], "&") == 0) //백그라운드 검사
				amper = 1;

			ptr = strtok(NULL, " ");
			i++;
		}
		
		
		

		if (strcmp(buff[0], "exit") == 0 || strcmp(buff[0], "logout") == 0) //로그아웃
			break;
			
	
		
			
		printf("프로세스 생성\n");
		pid[num] = fork(); 
		
		z=0;
		if(pid[num] == 0){ //자식 프로세스의 경우
		
		strcpy(command, buff[0]); // 명령어
		
			for(int j=0;j<=i;j++){ // Redirection 처리
				if ( (strcmp(buff[j], ">") == 0) || (strcmp(buff[j], "<") == 0) ){
					
					strcpy(files[z], buff[j + 1]); // 파일
					file = open(files[z], O_RDWR | O_CREAT, 0777); //파일 오픈

					if(strcmp(buff[j], ">") == 0){
						dup2(file, STDOUT_FILENO); //파일 디스크립터 복제
					}
					else{
						dup2(file, STDIN_FILENO); //파일 디스크립터 복제
					}
						
					close(file); //안 쓰는 디스크립터는 닫아줌
						
				}
			} 
			
			if(redirection == 1)
				execlp(command, command, NULL); 
			else{
				printf(">>%d<<\n", strlen(buff[1]));
				
				//cmd_string[0] = buff[0];
				//cmd_string[1] = buff[1];
				
				//printf("%s", cmd_string[0]);
				//printf("%d<", strlen(cmd_string[1]));
				
				if( strlen(buff[1]) == 0 ){
					execlp(command, command, NULL); 
				}
				else{
					cmd_string[0] = buff[0];
					cmd_string[1] = buff[1];
					execvp(command, &cmd_string);
				}
				
				
				//if(buff[1]){
					//printf("test");
				//}
				
				//printf("%daa", strcmp(buff[1], NULL));
					
				//if(strcmp(cmd_string[0], NULL) == 0){
					//printf("THIS");
				//}
				//else
					//printf("test2");
					
					
				
				//if(strcmp(buff[1], NULL) == 0)
					//execlp(command, command, NULL); 
				
				
				
				
				//execvp(command, &cmd_string);
			}
					
					 
				
			exit(0);

		}
		else{ //부모 프로세스의 경우
			sleep(1);
			if(amper==1)
				wait(&status);
		
		}


					

	} //while 끝
				
	printf("쉘 프로그램 종료\n");
	exit(0);
	

}
