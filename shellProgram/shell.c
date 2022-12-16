#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXSIZE 20
#define STDIN_FILENO 0 //표준입력
#define STDOUT_FILENO 1 //표준출력
int main(int argc, char** argv) {
	char* cmd_string[MAXSIZE] = { NULL };
	char cmd[MAXSIZE];
	char buff[MAXSIZE][MAXSIZE] = { NULL };
	char command[MAXSIZE] = { NULL }; //명령어
	char command2[MAXSIZE] = { NULL }; // pipe용 명령어
	char files[MAXSIZE][MAXSIZE] = { NULL };
	int filedes[2]; // 파이프용
	int i, j, z, num;
	int pid1, pid2, status;
	int file, file2;
	int amper = 0;
	int redirection;
	while (1) {
		printf("$ ");
		num = 0; //자식 프로세스 관리
		amper = 0; //백그라운드 변수
		redirection = 0; // redirection 검사
		fgets(cmd, sizeof(cmd), stdin); //문자열 입력받음
		cmd[strlen(cmd) - 1] = '\0'; //개행문자 제거
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
		pid1 = fork();
		z = 0;
		if (pid1 == 0) { //자식 프로세스의 경우
			strcpy(command, buff[0]); // 명령어1
			for (int j = 0; j < i; j++) { // Redirection 처리
				if ((strcmp(buff[j], ">") == 0) || (strcmp(buff[j], "<") == 0)) {
					redirection = 1;
					strcpy(files[z], buff[j + 1]); // 파일
					file = open(files[z], O_RDWR | O_CREAT, 0777); //파일
					오픈
						if (strcmp(buff[j], ">") == 0) {
							dup2(file, STDOUT_FILENO); //파일 디스크
							립터 복제
						}
						else {
							dup2(file, STDIN_FILENO); //파일 디스크립
							터 복제
						}
					close(file); //안 쓰는 디스크립터는 닫아줌
				}
			}
			for (int j = 0; j < i; j++) { // Pipe 처리
				if (strcmp(buff[j], "|") == 0) {
					strcpy(command2, buff[j + 1]); //명령어2
					if (pipe(filedes) == -1) {//파이프 생성
						printf("fail to call pipe()\n");
						exit(1);
					}
					if ((pid2 = fork()) < 0)
						perror("fork error");
					//파이프 연결
					if (pid2 > 0) {//child
						close(STDIN_FILENO);
						dup2(filedes[0], STDIN_FILENO);
						close(filedes[0]);
						close(filedes[1]);
						execlp(command, command, NULL);
					}
					else { // grandchild
						close(STDOUT_FILENO);
						dup2(filedes[1], STDOUT_FILENO);
						close(filedes[0]);
						close(filedes[1]);
						execlp(command2, command2, NULL);
					}
				}
			}
			if (redirection == 1)
				execlp(command, command, NULL);
			else {
				if (strlen(buff[1]) == 0) {
					execlp(command, command, NULL);
				}
				else if (strlen(buff[2]) == 0 && amper == 1) {
					execlp(command, command, NULL);
				}
				else {
					if (amper == 1)
						i--; //백그라운드 문자는 떼고 exec
					for (int l = 0; l < i; l++) {
						cmd_string[l] = buff[l];
					}
					execvp(command, &cmd_string);
				}
			}
			exit(0);
		}
		else { //부모 프로세스의 경우
			if (amper == 1) {
				waitpid(pid1, &status, 0);
			}
		}
	} //while 끝
	printf("쉘 프로그램 종료\n");
	exit(0);

}