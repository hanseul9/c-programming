#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// 색상 정의
#define BLACK   0
#define BLUE1   1
#define GREEN1   2
#define CYAN1   3
#define RED1   4
#define MAGENTA1 5
#define YELLOW1   6
#define GRAY1   7
#define GRAY2   8
#define BLUE2   9
#define GREEN2   10
#define CYAN2   11
#define RED2   12
#define MAGENTA2 13
#define YELLOW2   14
#define WHITE   15

#define STAR1 'O' // player1 표시
#define STAR2 'X' // player2 표시
#define MONSTER '&' //monster
#define BLANK ' ' // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define NUMBER1 '1'
#define NUMBER2 '2'
#define NUMBER3 '3'

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define UP2      'w'//player2 는 AWSD 로 방향키 대신
#define DOWN2   's'
#define LEFT2   'a'
#define RIGHT2   'd'

#define ENTER 13 //player1 점프
#define SPACE 0x20 //player2 점프

#define WIDTH 52
#define HEIGHT 26

int last_stage = 0;

int love; //생명 출력용

int dir[2] = { 0 }; //dir[0]은 playerA, dir[1]은 b, 1이면 왼 2이면 우를 바라보는 상태
int dir_count[2] = { 0 };


int map[HEIGHT][WIDTH] = { 0 };
int star[HEIGHT][WIDTH] = { 0 };

int starcount = 0;

int player1_score[HEIGHT][WIDTH] = { 0 };

int player2_score[HEIGHT][WIDTH] = { 0 };




int trap[HEIGHT][WIDTH] = { 0 };

int monster_location1[HEIGHT][WIDTH] = { 0 }; //몬스터 위치, 있는 위치에 1


int Delay = 10; // 10 msec delay, 이 값을 줄이면 속도가 빨라진다.
int keep_moving = 1; // 1:계속이동, 0:한칸씩이동.
int time_out = 60; // 제한시간
int score[2] = { 0 };

int called[5];
int frame_count = 0; // game 진행 frame count 로 속도 조절용으로 사용된다.

int p1_frame_sync = 9; // 처음 시작은 10 frame 마다 이동, 즉, 100msec 마다 이동
int p1_frame_sync_count = 0;

int p2_frame_sync = 9; // 처음 시작은 10 frame 마다 이동
int p2_frame_sync_count = 0;

int monster1_frame_sync = 6;
int monster2_frame_sync = 8;
int monster3_frame_sync = 10;




typedef struct player {
    int coin;        //coin하나당 하트 3개
    int heart[5];
    int jump_count;  //점프키를 얼마나 눌렀는지 측정
    int jump;

}player;


//plyaer 코인, 생명
player playerA = { 0,0,0,0, };
player playerB = { 0,0,0,0, };


void removeCursor(void) { // 커서를 안보이게 한다

    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}

void putstar(int x, int y, char ch)
{
    gotoxy(x, y);
    putchar(ch);
}
void erasestar(int x, int y)
{
    gotoxy(x, y);
    putchar(BLANK);
}

void textcolor(int fg_color, int bg_color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}
// 화면 지우기고 원하는 배경색으로 설정한다.
void cls(int bg_color, int text_color)
{
    char cmd[100];
    system("cls");
    sprintf(cmd, "COLOR %x%x", bg_color, text_color);
    system(cmd);

}

void player1(unsigned char ch)
{
    static int oldx = 17, oldy = 23, newx = 17, newy = 23;
    int move_flag = 0;
    static unsigned char last_ch = 0;


    if (called[0] == 0) { // 처음 또는 Restart
        oldx = 17, oldy = 23, newx = 17, newy = 23;
        putstar(oldx, oldy, STAR1);
        called[0] = 1; //한번 불려진것으로 표시
        last_ch = 0;
        ch = 0;
    }

    if (keep_moving && ch == 0)
        ch = last_ch;
    last_ch = ch; //가던 방향 기억해둠


    if (map[oldy][oldx] == 0 && dir_count[0] == 0) //떨어짐
    {

        newy = oldy + 1;
        move_flag = 1;


    }

    switch (ch) {
    case UP:
        if (oldy > 1 && map[oldy - 1][oldx] == 4) // 0 은 Status Line
            newy = oldy - 1;
        else if (oldy > 1 && map[oldy - 1][oldx] == 40)
            newy = oldy - 1;
        else if (oldy > 1 && map[oldy - 1][oldx] == 3)
            newy = oldy - 1;
        else { // 벽에 부딛치면 방향을 반대로 이동
           /*newy = oldy + 1;
           last_ch = DOWN;*/
        }
        move_flag = 1;
        dir[0] = 0;
        break;


    case DOWN:
        if (oldy < HEIGHT - 1 && map[oldy + 1][oldx] == 4)
            newy = oldy + 1;
        else if (oldy < HEIGHT - 1 && map[oldy + 1][oldx] == 40)
            newy = oldy + 1;
        else {
            /*newy = oldy - 1;
            last_ch = UP;*/
        }//map[y][x] == 4
        move_flag = 1;
        dir[0] = 0;
        break;
    case LEFT:
        if (oldx > 2 && map[oldy][oldx - 1] == 3 || map[oldy][oldx - 1] == 7)
            newx = oldx - 1;
        else if (oldx > 2 && map[oldy][oldx - 1] == 4)
            newx = oldx - 1;
        else {
            /*newx = oldx + 1;
            last_ch = RIGHT;*/
        }
        dir[0] = 1;//왼쪽을 바라봄
        move_flag = 1;
        break;
    case RIGHT:
        if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 3)
            newx = oldx + 1;
        else if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 4)
            newx = oldx + 1;
        else if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 40)
            newx = oldx + 1;
        else {
            /*newx = oldx - 1;
            last_ch = LEFT;*/
        }
        dir[0] = 2; //오른쪽을 바라봄
        move_flag = 1; //이동할 상태
        break;

    case ENTER: //점프

        if (playerA.jump == 1) { //1단점프

            if (dir[0] == 1 && oldx > 3) { //왼쪽을 바라볼때



                keep_moving = 1;

                if (dir_count[0] == 3)
                {
                    p1_frame_sync += 2;
                    newx = oldx - 1;
                    newy = oldy - 1;
                    dir_count[0]--;
                }
                else if (dir_count[0] == 2) {

                    newx = oldx - 1;
                    newy = oldy + 1;
                    keep_moving = 0;

                    p1_frame_sync -= 2;

                    dir_count[0] = 0;



                }

                move_flag = 1; //이동할 상태
                break;
            }
            else if (dir[0] == 2 && oldx < WIDTH - 2) { //오른쪽 바라볼때
                keep_moving = 1;
                if (dir_count[0] == 3)
                {
                    p1_frame_sync += 2;

                    newx = oldx + 1;
                    newy = oldy - 1;
                    dir_count[0]--;
                }
                else if (dir_count[0] == 2) {
                    newx = oldx + 1;
                    newy = oldy + 1;
                    dir_count[0]--;
                    keep_moving = 0;

                    p1_frame_sync -= 2;
                    dir_count[0] = 0;
                }

                move_flag = 1; //이동할 상태
                break;
            }

            dir_count[0]--;
        }

        else if (playerA.jump == 2) { //2단 점프
            {

                if (dir[0] == 1 && oldx > 3) { //왼쪽보고 있을 때



                    keep_moving = 1;
                    if (dir_count[0] == 3)
                    {
                        p1_frame_sync += 4;
                        newx = oldx - 1;
                        newy = oldy - 1;
                        dir_count[0]--;
                    }
                    else if (dir_count[0] == 2) {
                        newx = oldx - 1;
                        dir_count[0]--;
                    }
                    else if (dir_count[0] == 1) {
                        newx = oldx - 1;
                        newy = oldy + 1;
                        dir_count[0]--;
                        keep_moving = 0;

                        p1_frame_sync -= 4;
                    }
                    move_flag = 1; //이동할 상태
                    break;
                }
                else if (dir[0] == 2 && oldx < WIDTH - 2) { //오른쪽을 바라보고 있을때
                    keep_moving = 1;
                    if (dir_count[0] == 3)
                    {
                        p1_frame_sync += 4;
                        newx = oldx + 1;
                        newy = oldy - 1;
                        dir_count[0]--;
                    }
                    else if (dir_count[0] == 2) {
                        newx = oldx + 1;
                        dir_count[0]--;
                    }
                    else if (dir_count[0] == 1) {
                        newx = oldx + 1;
                        newy = oldy + 1;
                        dir_count[0]--;
                        keep_moving = 0;
                        p1_frame_sync -= 4;
                    }
                    move_flag = 1; //이동할 상태
                    break;
                }

                dir_count[0]--;
            }

        }




    }


    if (move_flag) { //움직였을때


        if (last_stage == 1 && newy != 23) { //보너스 스테이지




            if (map[newy][newx] == 3)
            {


                if (newx % 2 == 0 && player1_score[newy][newx] != 100) {

                    player1_score[newy][newx] = 100;
                    player1_score[newy][newx + 1] = 100;
                    score[0]++;


                    if (player2_score[newy][newx] == 100) {
                        player2_score[newy][newx] = 0;
                        player2_score[newy][newx + 1] = 0;
                        score[1]--;
                    }

                }

                else if (newx % 2 != 0 && player1_score[newy][newx] != 100) {
                    player1_score[newy][newx] = 100;
                    player1_score[newy][newx - 1] = 100;
                    score[0]++;


                    if (player2_score[newy][newx] == 100) {
                        player2_score[newy][newx] = 0;
                        player2_score[newy][newx - 1] = 0;
                        score[1]--;
                    }


                }


                //그리기




            }
            if (player1_score[oldy][oldx] == 100
                && newx % 2 == 0)
            {
                if (last_ch == DOWN || last_ch == UP) {
                    gotoxy(oldx, oldy);
                    printf("★");
                }

                else {
                    gotoxy(oldx - 1, oldy);
                    printf("★");



                }

            }
            else if (player1_score[oldy][oldx] == 100
                && newx % 2 != 0)
            {


                gotoxy(oldx, oldy);
                printf("★");

            }




            else if (map[oldy][oldx] == 4) {      //지나쳐온거 다시그려줌
                putstar(oldx, oldy, '#');
            }
            else if (map[oldy][oldx] == 40) {
                gotoxy(oldx, oldy);
                printf("□");
            }


            else
                erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고

            putstar(newx, newy, STAR1); // 새로운 위치에서 * 를 표시한다.
            oldx = newx; // 마지막 위치를 기억한다.
            oldy = newy;

        }

        else { //1,2 스테이지


            if (map[oldy][oldx] == 4) {      //지나쳐온거 다시그려줌
                putstar(oldx, oldy, '#');
            }
            else if (map[oldy][oldx] == 40) {
                gotoxy(oldx, oldy);
                printf("□");
            }

            else if (trap[oldy][oldx] == 88 && dir[0] == 1)
            {
                gotoxy(oldx, oldy);
                printf("▲");


            }
            else if (trap[oldy][oldx - 1] == 88 && dir[0] == 2) {
                gotoxy(oldx - 1, oldy);
                printf("▲");
            }


            else
                erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고

            putstar(newx, newy, STAR1); // 새로운 위치에서 * 를 표시한다.
            oldx = newx; // 마지막 위치를 기억한다.
            oldy = newy;

            if (star[oldy][oldx] == 7) //oldx%2!= 0&&
            {

                star[oldy][oldx] = 8;
                star[oldy][oldx + 1] = 8;
                starcount--;
            }
            else if (star[oldy][oldx - 1] == 7) {
                star[oldy][oldx] = 8;
                star[oldy][oldx - 1] = 8;
                starcount--;

            }

            if (trap[oldy][oldx] == 88) //함정에 닿음
            {

                if (playerA.heart[playerA.coin - 1] == 1) {
                    playerA.heart[playerA.coin - 1]--;
                    playerA.coin--;
                }

                else if (playerA.heart[playerA.coin - 1] != 0 && playerA.coin != 0) {
                    playerA.heart[playerA.coin - 1]--;

                }
            }
            else if (trap[oldy][oldx - 1] == 88) {
                if (playerA.heart[playerA.coin - 1] == 1) {
                    playerA.heart[playerA.coin - 1]--;
                    playerA.coin--;
                }

                else if (playerA.heart[playerA.coin - 1] != 0 && playerA.coin != 0) {
                    playerA.heart[playerA.coin - 1]--;

                }
            }

        }


    }
    else { //안움직였을때

        putstar(newx, newy, STAR1); // 새로운 위치에서 * 를 표시한다.

        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;




    }




    if (monster_location1[oldy][oldx] == 1) { //몬스터와 만남

        if (playerA.heart[playerA.coin - 1] == 1) {
            playerA.heart[playerA.coin - 1]--;
            playerA.coin--;
        }

        else if (playerA.heart[playerA.coin - 1] != 0 && playerA.coin != 0) {
            playerA.heart[playerA.coin - 1]--;

        }

    }





}
void player2(unsigned char ch)
{
    static int oldx = 38, oldy = 23, newx = 38, newy = 23;
    int move_flag = 0;
    static char last_ch = 0;

    if (called[1] == 0) { // 처음 또는 Restart
        oldx = 38, oldy = 23, newx = 38, newy = 23;
        putstar(oldx, oldy, STAR2);
        called[1] = 1; //한번 불려진것으로 표시
        last_ch = 0;
        ch = 0;
    }
    // 방향키가 눌려지지 않은 경우 예전에 이동하던 방향으로 계속 이동
    if (keep_moving && ch == 0)
        ch = last_ch;
    last_ch = ch;

    if (map[oldy][oldx] == 0 && dir_count[1] == 0)
    {

        newy = oldy + 1;
        move_flag = 1;


    }

    switch (ch) {
    case UP2:
        if (oldy > 1 && map[oldy - 1][oldx] == 4)
            newy = oldy - 1;
        else if (oldy > 1 && map[oldy - 1][oldx] == 40)
            newy = oldy - 1;
        else if (oldy > 1 && map[oldy - 1][oldx] == 3)
            newy = oldy - 1;
        else { // 벽에 부딛치면 방향을 반대로 이동
           /*newy = oldy + 1;
           last_ch = DOWN;*/
        }
        move_flag = 1;
        dir[1] = 0;
        break;

    case DOWN2:
        if (oldy < HEIGHT - 1 && map[oldy + 1][oldx] == 4)
            newy = oldy + 1;
        else if (oldy < HEIGHT - 1 && map[oldy + 1][oldx] == 40)
            newy = oldy + 1;
        else {
            /*newy = oldy - 1;
            last_ch = UP;*/
        }//map[y][x] == 4
        move_flag = 1;
        dir[1] = 0;
        break;
    case LEFT2:
        if (oldx > 2 && map[oldy][oldx - 1] == 3 || map[oldy][oldx - 1] == 7)
            newx = oldx - 1;
        else if (oldx > 2 && map[oldy][oldx - 1] == 4)
            newx = oldx - 1;
        else {
            /*newx = oldx + 1;
            last_ch = RIGHT;*/
        }
        dir[1] = 1;//왼쪽을 바라봄
        move_flag = 1;
        break;
    case RIGHT2:
        if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 3)
            newx = oldx + 1;
        else if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 4)
            newx = oldx + 1;
        else if (oldx < WIDTH - 1 && map[oldy][oldx + 1] == 40)
            newx = oldx + 1;
        else {
            /*newx = oldx - 1;
            last_ch = LEFT;*/
        }
        dir[1] = 2; //오른쪽을 바라봄
        move_flag = 1; //이동할 상태
        break;
    case SPACE: //점프


        if (playerB.jump == 1) { //1단점프

            if (dir[1] == 1 && oldx > 3) { //왼쪽을 바라볼때


                keep_moving = 1;

                if (dir_count[1] == 3)
                {
                    p2_frame_sync += 2;
                    newx = oldx - 1;
                    newy = oldy - 1;
                    dir_count[1]--;
                }
                else if (dir_count[1] == 2) {


                    newx = oldx - 1;
                    newy = oldy + 1;
                    keep_moving = 0;

                    p2_frame_sync -= 2;

                    dir_count[1] = 0;



                }

                move_flag = 1; //이동할 상태
                break;
            }
            else if (dir[1] == 2 && oldx < WIDTH - 2) { //오른쪽 바라볼때
                keep_moving = 1;
                if (dir_count[1] == 3)
                {
                    p2_frame_sync += 2;

                    newx = oldx + 1;
                    newy = oldy - 1;
                    dir_count[1]--;
                }
                else if (dir_count[1] == 2) {
                    newx = oldx + 1;
                    newy = oldy + 1;
                    dir_count[1]--;
                    keep_moving = 0;

                    p2_frame_sync -= 2;
                    dir_count[1] = 0;
                }

                move_flag = 1; //이동할 상태
                break;
            }

            dir_count[1]--;
        }

        else if (playerB.jump == 2) { //2단 점프
            {

                if (dir[1] == 1 && oldx > 3) { //왼쪽보고 있을 때



                    keep_moving = 1;
                    if (dir_count[1] == 3)
                    {
                        p2_frame_sync += 4;
                        newx = oldx - 1;
                        newy = oldy - 1;
                        dir_count[1]--;
                    }
                    else if (dir_count[1] == 2) {
                        newx = oldx - 1;
                        dir_count[1]--;
                    }
                    else if (dir_count[1] == 1) {
                        newx = oldx - 1;
                        newy = oldy + 1;
                        dir_count[1]--;
                        keep_moving = 0;

                        p2_frame_sync -= 4;
                    }
                    move_flag = 1; //이동할 상태
                    break;
                }
                else if (dir[1] == 2 && oldx < WIDTH - 2) { //오른쪽을 바라보고 있을때
                    keep_moving = 1;
                    if (dir_count[1] == 3)
                    {
                        p2_frame_sync += 4;
                        newx = oldx + 1;
                        newy = oldy - 1;
                        dir_count[1]--;
                    }
                    else if (dir_count[1] == 2) {
                        newx = oldx + 1;
                        dir_count[1]--;
                    }
                    else if (dir_count[1] == 1) {
                        newx = oldx + 1;
                        newy = oldy + 1;
                        dir_count[1]--;
                        keep_moving = 0;
                        p2_frame_sync -= 4;
                    }
                    move_flag = 1; //이동할 상태
                    break;
                }

                dir_count[1]--;
            }

        }





    }

    if (move_flag) { //움직였을때


        if (last_stage == 1 && newy != 23) { //보너스 스테이지

            if (map[newy][newx] == 3)
            {


                if (newx % 2 == 0 && player2_score[newy][newx] != 100) {

                    player2_score[newy][newx] = 100;
                    player2_score[newy][newx + 1] = 100;
                    score[1]++;


                    if (player1_score[newy][newx] == 100) {
                        player1_score[newy][newx] = 0;
                        player1_score[newy][newx + 1] = 0;
                        score[0]--;
                    }

                }

                else if (newx % 2 != 0 && player2_score[newy][newx] != 100) {
                    player2_score[newy][newx] = 100;
                    player2_score[newy][newx - 1] = 100;
                    score[1]++;


                    if (player1_score[newy][newx] == 100) {
                        player1_score[newy][newx] = 0;
                        player1_score[newy][newx - 1] = 0;
                        score[0]--;
                    }


                }




            } //그리기
            if (player2_score[oldy][oldx] == 100
                && newx % 2 == 0)
            {
                if (last_ch == DOWN2 || last_ch == UP2) {
                    gotoxy(oldx, oldy);
                    printf("♡");
                }
                /*else if () {

                }*/
                else {
                    gotoxy(oldx - 1, oldy);
                    printf("♡");



                }

            }
            else if (player2_score[oldy][oldx] == 100
                && newx % 2 != 0)
            {


                gotoxy(oldx, oldy);
                printf("♡");

            }




            else if (map[oldy][oldx] == 4) {      //지나쳐온거 다시그려줌
                putstar(oldx, oldy, '#');
            }
            else if (map[oldy][oldx] == 40) {
                gotoxy(oldx, oldy);
                printf("□");
            }


            else
                erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고

            putstar(newx, newy, STAR2); // 새로운 위치에서 * 를 표시한다.
            oldx = newx; // 마지막 위치를 기억한다.
            oldy = newy;

        }
        else { // 1,2 스테이지


            if (map[oldy][oldx] == 4) {      //지나쳐온거 다시그려줌
                putstar(oldx, oldy, '#');
            }
            else if (map[oldy][oldx] == 40) {
                gotoxy(oldx, oldy);
                printf("□");
            }


            else if (trap[oldy][oldx] == 88 && dir[1] == 1)
            {
                gotoxy(oldx, oldy);
                printf("▲");


            }
            else if (trap[oldy][oldx - 1] == 88 && dir[1] == 2) {
                gotoxy(oldx - 1, oldy);
                printf("▲");
            }


            else
                erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고

            putstar(newx, newy, STAR2); // 새로운 위치에서 * 를 표시한다.
            oldx = newx; // 마지막 위치를 기억한다.
            oldy = newy;

            if (star[oldy][oldx] == 7) //별을 만남
            {

                star[oldy][oldx] = 8;
                star[oldy][oldx + 1] = 8;
                starcount--;
            }
            else if (star[oldy][oldx - 1] == 7) {
                star[oldy][oldx] = 8;
                star[oldy][oldx - 1] = 8;
                starcount--;
            }


            if (trap[oldy][oldx] == 88) //함정에 닿음
            {

                if (playerB.heart[playerB.coin - 1] == 1) {
                    playerB.heart[playerB.coin - 1]--;
                    playerB.coin--;
                }

                else if (playerB.heart[playerB.coin - 1] != 0 && playerB.coin != 0) {
                    playerB.heart[playerB.coin - 1]--;

                }
            }
            else if (trap[oldy][oldx - 1] == 88) {
                if (playerB.heart[playerB.coin - 1] == 1) {
                    playerB.heart[playerB.coin - 1]--;
                    playerB.coin--;
                }

                else if (playerB.heart[playerB.coin - 1] != 0 && playerB.coin != 0) {
                    playerB.heart[playerB.coin - 1]--;

                }
            }





        }

    }
    else { //안움직였을때

        putstar(newx, newy, STAR2); // 새로운 위치에서 * 를 표시한다.

        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;



    }



    if (monster_location1[oldy][oldx] == 1) { //몬스터와 만남

        if (playerB.heart[playerB.coin - 1] == 1) {
            playerB.heart[playerB.coin - 1]--;
            playerB.coin--;
        }

        else if (playerB.heart[playerB.coin - 1] != 0 && playerB.coin != 0) {
            playerB.heart[playerB.coin - 1]--;

        }

    }

}

void monster1(unsigned char ch)
{
    static int oldx = 5, oldy = 8, newx = 5, newy = 8;
    int move_flag = 0;
    static unsigned char last_ch = 0;

    if (called[2] == 0) { // 처음 또는 Restart 
        oldx = 5, oldy = 8, newx = 5, newy = 8;
        putstar(oldx, oldy, MONSTER);
        called[2] = 1;
        last_ch = LEFT;
        ch = 1;

    }
    //if (last_ch == ch)
    //   return;
    if (ch == 1)
        ch = last_ch;
    last_ch = ch;



    switch (ch) {

    case LEFT:
        if (oldx > 2)
            newx = oldx - 1;
        else {
            newx = oldx + 1;
            last_ch = RIGHT;
        }
        move_flag = 1;
        break;
    case RIGHT:
        if (oldx < WIDTH - 1)
            newx = oldx + 1;
        else {
            newx = oldx - 1;
            last_ch = LEFT;
        }
        move_flag = 1;
        break;
    }
    if (move_flag) {


        erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
        monster_location1[oldy][oldx] = 0;

        putstar(newx, newy, MONSTER); // 새로운 위치에서 * 를 표시한다.
        monster_location1[newy][newx] = 1;




        /*gotoxy(12, 10);
        printf("   : %d  %d: ", newy, newx);*/

        if (star[oldy][oldx] == 7 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("☆");


        }
        else if (star[oldy][oldx - 1] == 7 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("☆");
        }



        else if (trap[oldy][oldx] == 88 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("▲");


        }
        else if (trap[oldy][oldx - 1] == 88 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("▲");
        }


        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;

    }


}

void monster2(unsigned char ch)
{
    static int oldx = 20, oldy = 13, newx = 20, newy = 13;
    int move_flag = 0;
    static unsigned char last_ch = 0;

    if (called[3] == 0) { // 처음 또는 Restart 
        oldx = 20, oldy = 13, newx = 20, newy = 13;
        putstar(oldx, oldy, MONSTER);
        called[3] = 1;
        last_ch = LEFT;
        ch = 1;

    }
    //if (last_ch == ch)
    //   return;
    if (ch == 1)
        ch = last_ch;
    last_ch = ch;



    switch (ch) {

    case LEFT:
        if (oldx > 2)
            newx = oldx - 1;
        else {
            newx = oldx + 1;
            last_ch = RIGHT;
        }
        move_flag = 1;
        break;
    case RIGHT:
        if (oldx < WIDTH - 1)
            newx = oldx + 1;
        else {
            newx = oldx - 1;
            last_ch = LEFT;
        }
        move_flag = 1;
        break;
    }
    if (move_flag) {


        erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
        monster_location1[oldy][oldx] = 0;

        putstar(newx, newy, MONSTER); // 새로운 위치에서 * 를 표시한다.
        monster_location1[newy][newx] = 1;

        if (map[oldy][oldx] == 4) {      //사다리 다시그려줌
            putstar(oldx, oldy, '#');
        }

        if (star[oldy][oldx] == 7 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("☆");


        }
        else if (star[oldy][oldx - 1] == 7 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("☆");
        }


        else if (trap[oldy][oldx] == 88 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("▲");


        }
        else if (trap[oldy][oldx - 1] == 88 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("▲");
        }



        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;

    }


}

void monster3(unsigned char ch)
{
    static int oldx = 30, oldy = 18, newx = 30, newy = 18;
    int move_flag = 0;
    static unsigned char last_ch = 0;

    if (called[4] == 0) { // 처음 또는 Restart 
        oldx = 30, oldy = 18, newx = 30, newy = 18;
        putstar(oldx, oldy, MONSTER);
        called[4] = 1;
        last_ch = LEFT;
        ch = 1;

    }
    //if (last_ch == ch)
    //   return;
    if (ch == 1)
        ch = last_ch;
    last_ch = ch;



    switch (ch) {

    case LEFT:
        if (oldx > 2)
            newx = oldx - 1;
        else {
            newx = oldx + 1;
            last_ch = RIGHT;
        }
        move_flag = 1;
        break;
    case RIGHT:
        if (oldx < WIDTH - 1)
            newx = oldx + 1;
        else {
            newx = oldx - 1;
            last_ch = LEFT;
        }
        move_flag = 1;
        break;
    }
    if (move_flag) {


        erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
        monster_location1[oldy][oldx] = 0;

        putstar(newx, newy, MONSTER); // 새로운 위치에서 * 를 표시한다.
        monster_location1[newy][newx] = 1;

        if (map[oldy][oldx] == 4) {      //사다리 다시그려줌
            putstar(oldx, oldy, '#');
        }



        if (star[oldy][oldx] == 7 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("☆");


        }
        else if (star[oldy][oldx - 1] == 7 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("☆");
        }


        else if (trap[oldy][oldx] == 88 && last_ch == LEFT)
        {
            gotoxy(oldx, oldy);
            printf("▲");


        }
        else if (trap[oldy][oldx - 1] == 88 && last_ch == RIGHT) {
            gotoxy(oldx - 1, oldy);
            printf("▲");
        }




        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;

    }


}


// box 그리기 함수, ch 문자로 (x1,y1) ~ (x2,y2) box를 그린다.
void draw_box(int x1, int y1, int x2, int y2, char ch)
{
    int x, y;
    for (x = x1; x <= x2; x += 1) {
        gotoxy(x, y1);
        printf("%c", ch);
        gotoxy(x, y2);
        printf("%c", ch);
    }
    for (y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        printf("%c", ch);
        gotoxy(x2, y);
        printf("%c", ch);
    }
}

void draw_box2(int x2, int y2)
{
    int x, y;
    //int len = strlen(ch);
    for (x = 0; x <= x2; x += 2) { // 한글은 2칸씩 차지한다.
        gotoxy(x, 0);
        printf("%s", "─"); // ㅂ 누르고 한자키 누르고 선택
        gotoxy(x, y2);
        printf("%s", "─");
    }
    for (y = 0; y <= y2; y++) {
        gotoxy(0, y);
        printf("%s", "│");
        gotoxy(x2, y);
        printf("%s", "│");
    }
    gotoxy(0, 0); printf("┌");
    gotoxy(0, y2); printf("└");
    gotoxy(x2, 0); printf("┐");
    gotoxy(x2, y2); printf("┘");
}
void draw_box2_fill(int x1, int y1, int x2, int y2, int color)
{
    int x, y;

    textcolor(BLACK, WHITE);
    for (x = x1; x <= x2; x += 2) { // 한글은 2칸씩 차지한다.
        gotoxy(x, y1);
        printf("%s", "─"); // ㅂ 누르고 한자키 누르고 선택
        gotoxy(x, y2);
        printf("%s", "─");
    }
    for (y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        printf("%s", "│");
        gotoxy(x2, y);
        printf("%s", "│");
    }
    gotoxy(x1, y1); printf("┌");
    gotoxy(x1, y2); printf("└");
    gotoxy(x2, y1); printf("┐");
    gotoxy(x2, y2); printf("┘");
    //getch();
    for (y = y1 + 1; y < y2; y++) {
        for (x = x1 + 2; x < x2; x++) {
            textcolor(color, color);
            gotoxy(x, y);
            printf(" ");
        }
    }

}



// 가로줄 
void draw_hline(int y, int x1, int x2, char ch)
{
    gotoxy(x1, y);
    for (; x1 <= x2; x1++)
        putchar(ch);
}

//게임 초기화, 처음 시작과 Restar때 호출
void init_game()
{

    int x, y;
    char cmd[100];



    srand(time(NULL));
    score[0] = score[1] = 0;
    called[0] = called[1] = called[2] = called[3] = called[4] = 0;


    time_out = 60;    //전체 게임시간
    keep_moving = 0;
    Delay = 10; //여길 바꾸면 속도가 달라짐

    cls(WHITE, BLACK);
    removeCursor();
    textcolor(BLACK, GRAY2);
    draw_hline(0, 0, 79, ' ');
    textcolor(BLACK, WHITE);
    sprintf(cmd, "mode con cols=%d lines=%d", WIDTH, HEIGHT);
    //system(cmd);
}

void show_time(int remain_time)
{
    textcolor(CYAN2, YELLOW2);
    gotoxy(18, 1);
    printf("  ※ TIME: %03d ※ ", remain_time);
    textcolor(BLACK, WHITE);
}

void show_coin(int player, int coin)
{
    /*gotoxy(30, 0);
    textcolor(YELLOW2, GRAY2);

    printf("coin : %d", acoin);
    textcolor(BLACK, WHITE);*/

    textcolor(GRAY2, YELLOW2);
    switch (player) {
    case 0: // player 1
        gotoxy(2, 1);
        printf("Player1 coin : %d", coin);
        break;
    case 1: // player 2
        gotoxy(36, 1);
        printf("Player2 coin : %d", coin);
        break;
    }
    textcolor(BLACK, WHITE);
}

void show_score(int player)
{

    textcolor(GRAY2, YELLOW2);
    switch (player) {
    case 0: // player 1
        gotoxy(2, 1);
        printf(" P.1 score : %2d", score[0]);
        break;
    case 1: // player 2
        gotoxy(37, 1);
        printf(" P.2 score : %2d", score[1]);
        break;
    }
    textcolor(BLACK, WHITE);
}

void show_heart(int player)
{
    int heart = 0;
    if (player == 1) {
        //printf("ddd");
        heart = playerA.heart[playerA.coin - 1];


    }
    else {

        heart = playerB.heart[playerB.coin - 1];
    }
    textcolor(GRAY2, WHITE);
    switch (heart) {
    case 0:
        printf("LIFE: X X X ");
        break;
    case 1:
        printf("LIFE: ♥ X X ");
        break;
    case 2:
        printf("LIFE: ♥ ♥ X ");
        break;
    case 3:
        //gotoxy(4, 3);
        printf("LIFE: ♥ ♥ ♥ ");
        break;
    }


    textcolor(BLACK, WHITE);
}


void show_map() {

    int i, x, y;


    for (y = 4; y < HEIGHT - 1; y++) {
        for (x = 2; x < WIDTH - 1; x += 2) {
            gotoxy(x, y);
            if (map[y][x] == 1)
                printf("□");

            else if (map[y][x] == 40)
                printf("□");
            else if (map[y][x] == 4)
                printf("#");
            else if (map[y][x] == 5) {
                printf("■");


            }
            else {
                printf("");
            }

        }
    }
}
void show_star() { //초기 별

    int i, x, y;


    for (y = 4; y < HEIGHT - 1; y++) {
        for (x = 2; x < WIDTH - 1; x += 2) {
            gotoxy(x, y);
            if (star[y][x] == 7)
                printf("☆");

        }
    }
}

void show_trap() { //함정

    int i, x, y;


    for (y = 4; y < HEIGHT - 1; y++) {
        for (x = 2; x < WIDTH - 1; x += 2) {
            gotoxy(x, y);
            if (trap[y][x] == 88)
                printf("▲");

        }
    }
}


void main()
{
FIRST: {

    for (int i = 4; i < HEIGHT - 3; i++) { // 맵 초기화
        for (int j = 2; j < WIDTH - 3; j += 2) {


            map[i][j] = 0;

        }
    }

    for (int i = 4; i < HEIGHT - 3; i++) { //별 초기화
        for (int j = 2; j < WIDTH - 3; j += 2) {

            star[i][j] = 0;

        }
    }

    }


srand(time(NULL));

//맵- 필드 
//블럭 : 1  /  움직일수 있는 범위 : 3  
for (int i = 0; i < 1; i++) {

    //맨 위 블럭
    for (int i = 2; i < 16; i++) {
        map[9][i] = 1;
        map[8][i] = 3;

    }
    for (int i = 18; i < 30; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }

    for (int i = 32; i < 50; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }
    map[4][2] = 2;
    /*map[4][18] = 2;
    map[4][33] = 2;*/



    //중간 블럭
    for (int i = 4; i < 20; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }

    for (int i = 22; i < 38; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }

    for (int i = 40; i < 51; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }
    map[9][2] = 2;



    //맨 아래 블럭
    for (int i = 2; i < 10; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }


    for (int i = 12; i < 26; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }

    for (int i = 28; i < 50; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }
    /*for (int i = 37; i < 50; i++) {
       map[19][i] = 1;
       map[18][i] = 3;
    }*/
    map[14][2] = 2;
    /*map[14][20] = 2;
    map[14][34] = 2;*/

    for (int i = 1; i < WIDTH - 1; i++) {
        map[23][i] = 3;
        map[24][i] = 5;
        map[25][i] = 5;
    }

}

//맵- 사다리 : 4,40
for (int i = 0; i < 1; i++) {

    //사다리와 이어진 ㅁ 부분 , 9, 14
    map[9][10] = 40;

    map[9][24] = 40;

    map[9][36] = 40;
    for (int i = 10; i < 14; i++) {
        map[i][10] = 4;
        map[i][24] = 4;
        map[i][36] = 4;
    }

    map[14][6] = 40;

    map[14][30] = 40;

    map[14][44] = 40;
    for (int i = 15; i < 19; i++) {
        map[i][6] = 4;
        map[i][30] = 4;
        map[i][44] = 4;
    }


    //맨밑 사다리는 위에만 
    map[19][4] = 40;

    map[19][16] = 40;

    map[19][40] = 40;
    for (int i = 20; i < 24; i++) {
        map[i][4] = 4;
        map[i][16] = 4;
        map[i][40] = 4;
    }
}

for (int i = 4; i < HEIGHT - 3; i++) { //별
    for (int j = 2; j < WIDTH - 3; j += 2) {
        //int a=1;
        if (rand() % 7 == 0 && map[i][j] == 3) { //난수생성
            if (j % 2 == 0)
                j + 1;
            star[i][j] = 7;

            starcount++;
        }
    }
}



//초기화면
int start = 0;

unsigned char ch = 0;
int run_time, start_time, remain_time, last_remain_time;
;

int acoin, bcoin;


removeCursor(); // 커서를 안보이게 한다


cls(WHITE, BLACK);
for (int i = 0; i <= 0; i++) {

    draw_box2_fill(i * 2, i, WIDTH - i * 2, HEIGHT - i, YELLOW2);

}

gotoxy(15, 10);
textcolor(BLACK, YELLOW2);
printf("★ player 1 coin : 1 ★");
gotoxy(15, 11);
printf("★ player 2 coin : 2 ★");
gotoxy(19, 12);
printf("★ start : 3 ★");

while (1) { //코인이 있어야 게임 시작 가능



    if (kbhit() == 1) {  // 키보드가 눌려져 있으면
        ch = getch();// key 값을 읽는다


        if (ch == NUMBER1) {
            if (playerA.coin == 5);
            else {
                playerA.coin++;

                playerA.heart[playerA.coin - 1] = 3;

                show_coin(0, playerA.coin);

            }
        }

        if (ch == NUMBER2) {
            if (playerB.coin == 5);

            else {
                playerB.coin++;
                playerB.heart[playerB.coin - 1] = 3;

                show_coin(1, playerB.coin);
            }
        }

        if (ch == NUMBER3 && playerA.coin + playerB.coin == 0)
        {
            gotoxy(16, 13);
            textcolor(BLACK, YELLOW2);
            printf("→동전을 넣어주세요←");
        }
    }

    if (ch == NUMBER3 && playerA.coin + playerB.coin >= 1)
        break;
}
//초기화면



START:
init_game(); //초기화

start_time = time(NULL);
last_remain_time = remain_time = time_out;

cls(WHITE, BLACK);
draw_box2(WIDTH, HEIGHT);

show_time(remain_time);




show_map();

show_star();


//stage 1
while (1) { //게임 메인 실행 파트



    if (starcount == 0)
        break;

    show_coin(0, playerA.coin); show_coin(1, playerB.coin);


    // 시간 check
    run_time = time(NULL) - start_time;

    remain_time = time_out - run_time;
    if (remain_time < last_remain_time) {
        show_time(remain_time); // 시간이 변할때만 출력
        last_remain_time = remain_time;
    }
    if (remain_time == 0) // 시간 종료
        break;


    char m1 = 0, m2 = 0; //동시이동
    if (GetAsyncKeyState(VK_UP) & 0x8000
        || GetAsyncKeyState(VK_DOWN) & 0x8000
        || GetAsyncKeyState(VK_LEFT) & 0x8000
        || GetAsyncKeyState(VK_RIGHT) & 0x8000
        || GetAsyncKeyState(0x57) & 0x8000 //W (UP)
        || GetAsyncKeyState(0x53) & 0x8000 //S (DOWN)
        || GetAsyncKeyState(0x41) & 0x8000 //A (LEFT)
        || GetAsyncKeyState(0x44) & 0x8000 //D (RIGHT)
        || GetAsyncKeyState(VK_RETURN) & 0x8000  //enter
        || GetAsyncKeyState(VK_SPACE) & 0x8000)  //space bar
    {


        if (GetAsyncKeyState(VK_UP))   //player 1
            m1 = UP;
        if (GetAsyncKeyState(VK_DOWN))
            m1 = DOWN;
        if (GetAsyncKeyState(VK_LEFT))
            m1 = LEFT;
        if (GetAsyncKeyState(VK_RIGHT))
            m1 = RIGHT;
        if (GetAsyncKeyState(VK_RETURN)) {
            m1 = 0;

            playerA.jump_count++;
        }


        if (GetAsyncKeyState(0x57))      //player 2
            m2 = UP2;
        if (GetAsyncKeyState(0x53))
            m2 = DOWN2;
        if (GetAsyncKeyState(0x41))
            m2 = LEFT2;
        if (GetAsyncKeyState(0x44))
            m2 = RIGHT2;
        if (GetAsyncKeyState(VK_SPACE)) {

            m2 = 0;
            playerB.jump_count++;

        }


        if (playerA.coin == 0)
            m1 = 0;

        if (playerB.coin == 0)
            m2 = 0;


        //player 1
        if (playerA.coin != 0) {

            if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련

                if (frame_count % p1_frame_sync == 0) {
                    player1(ENTER);
                    //dir_count[0]--;
                }
                m1 = 0;
            }
            else if (frame_count % p1_frame_sync == 0)
                player1(m1);


            //ch = 1;


            //점프조작

            if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) { //enter에서 손을 뗀 상태

                if (playerA.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드
                    playerA.jump = 1;

                else {
                    playerA.jump = 2;
                }

                playerA.jump_count = 0;


                dir_count[0] = 3;

                player1(ENTER);



            }
        }



        //player 2
        if (playerB.coin != 0) {

            if (dir_count[1] == 2 || dir_count[1] == 1) { //점프관련


                if (frame_count % p2_frame_sync == 0) {
                    player2(SPACE);
                    //dir_count[0]--;
                }
                m2 = 0;
            }
            else if (frame_count % p2_frame_sync == 0)
                player2(m2);


            //ch = 1;


            //점프조작

            if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태



                if (playerB.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드로하기
                    playerB.jump = 1;

                else {
                    playerB.jump = 2;
                }

                playerB.jump_count = 0;


                dir_count[1] = 3;

                player2(SPACE);


            }
        }





    }

    if (kbhit() == 1) {  // 키보드가 눌려져 있으면
        char c1;
        c1 = getch(); // key 값을 읽는다
        if (c1 == '\0') // VS 의 NULL 값은 Error이다.
            continue;
        else
            ch = c1;

        // ESC 누르면 프로그램 종료
        if (ch == ESC)
            exit(0);


        if (ch == NUMBER1) {      //코인추가
            if (playerA.coin == 5);
            else {
                playerA.coin++;
                playerA.heart[playerA.coin - 1] = 3;

                show_coin(0, playerA.coin);

            }
        }

        if (ch == NUMBER2) {  //코인추가
            if (playerB.coin == 5);

            else {
                playerB.coin++;
                playerB.heart[playerB.coin - 1] = 3;

                show_coin(1, playerB.coin);
            }
        }

    }
    else {  //keyboard 가 눌려지지 X 몬스터-움직임 , 플레이어 - 그자리 그대로


        //PLAYER 1
        if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련
            if (frame_count % p1_frame_sync == 0)
                player1(ENTER);
            //dir_count[0]--;


        }

        else if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) {

            if (playerA.jump_count < 25)
                playerA.jump = 1;

            else {
                playerA.jump = 2;
            }

            playerA.jump_count = 0;
            dir_count[0] = 3;
            player1(ENTER);
            //dir_count[0]--;
        }

        else if (frame_count % p1_frame_sync == 0)
            player1(0);


        //PLAYER 2
        if (dir_count[1] == 2 || dir_count[1] == 1) { //플레이어 2 점프관련
           //printf("dddd");
            if (frame_count % p2_frame_sync == 0)
                player2(SPACE);
            //dir_count[1]--;

        }

        else if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태

            if (playerB.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드로하기
                playerB.jump = 1;

            else {
                playerB.jump = 2;
            }

            playerB.jump_count = 0;


            dir_count[1] = 3;

            player2(SPACE);

            //dir_count[1]--;


        }

        else if (frame_count % p2_frame_sync == 0)
            player2(0);



    }





    if (frame_count % monster1_frame_sync == 0) //몬스터 이동
        monster1(1);

    if (frame_count % monster2_frame_sync == 0) //몬스터 이동
        monster2(1);

    if (frame_count % monster3_frame_sync == 0) //몬스터 이동
        monster3(1);

    gotoxy(4, 3); show_heart(1);
    gotoxy(37, 3); show_heart(2);



    Sleep(Delay); // Delay 값을 줄이고
    frame_count = frame_count++; // frame_count 값으로 속도 조절을 한다.

} //메인 실행 파트 - 엔진





//게임끝!
if (starcount == 0) { //별을 다 먹으면

    while (1) { //게임이 끝났을때
        int c1, c2;
        do { // 색을 변경하면서 Game Over 출력
            c1 = rand() % 16;
            c2 = rand() % 16;
        } while (c1 == c2);
        textcolor(c1, c2);
        gotoxy(20, 12);
        printf(" Stage Clear!! ");
        gotoxy(18, 14);
        textcolor(WHITE, BLACK);
        printf(" Hit (G) Next Stage ");



        Sleep(500);

        if (kbhit()) {
            ch = getch();
            if (ch == 'g')
                break;
        }




    }
}

else {
    while (1) { //게임이 끝났을때
        int c1, c2;
        do { // 색을 변경하면서 Game Over 출력
            c1 = rand() % 16;
            c2 = rand() % 16;
        } while (c1 == c2);
        textcolor(c1, c2);
        gotoxy(20, 11);
        printf("** Time Out **");
        gotoxy(12, 13);
        textcolor(WHITE, BLACK);
        printf(" Hit (R) to Restart (Q) to Quit ");
        Sleep(500);
        if (kbhit()) {
            ch = getch();
            if (ch == 'r' || ch == 'q')
                break;
        }
    }
    if (ch == 'r') {

        playerA.coin = 0;
        playerA.heart[5] = 0;

        playerB.coin = 0;
        playerB.heart[5] = 0;
        goto START; //시작부분으로 돌아가서 초기화하고 다시 시작

    }

    gotoxy(0, HEIGHT - 1);
}

//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
//stage 2




cls(WHITE, BLACK);


init_game(); //초기화
start_time = time(NULL);
last_remain_time = remain_time = time_out;

cls(WHITE, BLACK);
draw_box2(WIDTH, HEIGHT);


show_time(remain_time);


//맵, 별 초기화


starcount = 0;
srand(time(NULL));

//상자
draw_box2(WIDTH, HEIGHT);

for (int i = 4; i < HEIGHT - 3; i++) {
    for (int j = 2; j < WIDTH - 3; j += 2) {
        map[i][j] = 0;
        star[i][j] = 0;
        trap[i][j] = 0;
    }
}


start_time = time(NULL);

//맵- 필드 
//블럭 : 1  /  움직일수 있는 범위 : 3  
for (int i = 0; i < 1; i++) {

    //맨 위 블럭
    for (int i = 2; i < 16; i++) {
        map[9][i] = 1;
        map[8][i] = 3;

    }
    for (int i = 18; i < 36; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }

    for (int i = 38; i < 50; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }



    //중간 블럭
    for (int i = 2; i < 26; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }

    for (int i = 28; i < 42; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }

    for (int i = 44; i < 52; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }


    //맨 아래 블럭
    for (int i = 2; i < 10; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }
    //map[18][10] = 3;

    for (int i = 12; i < 30; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }

    for (int i = 32; i < 52; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }

    //map[14][2] = 2;

    for (int i = 1; i < WIDTH - 1; i++) {
        map[23][i] = 3;
        map[24][i] = 5;
        map[25][i] = 5;
    }

}

//맵- 사다리 : 4,40
for (int i = 0; i < 1; i++) {

    //사다리와 이어진 ㅁ 부분 , 9, 14
    map[9][10] = 40;

    map[9][24] = 40;

    map[9][40] = 40;
    for (int i = 10; i < 14; i++) {
        map[i][10] = 4;
        map[i][24] = 4;
        map[i][40] = 4;
    }

    map[14][18] = 40;

    map[14][34] = 40;

    map[14][46] = 40;
    for (int i = 15; i < 19; i++) {
        map[i][18] = 4;
        map[i][34] = 4;
        map[i][46] = 4;
    }


    //맨밑 사다리는 위에만 
    map[19][4] = 40;

    map[19][24] = 40;

    map[19][40] = 40;
    for (int i = 20; i < 24; i++) {
        map[i][4] = 4;
        map[i][24] = 4;
        map[i][40] = 4;
    }
}


for (int i = 4; i < HEIGHT - 3; i++) { //별
    for (int j = 2; j < WIDTH - 3; j += 2) {

        if (rand() % 6 == 0 && map[i][j] == 3) { //난수생성
            if (j % 2 == 0)
                j + 1;
            star[i][j] = 7;



            starcount++;
        }
    }
}



for (int i = 4; i < HEIGHT - 3; i++) { //함정   ▲
    for (int j = 2; j < WIDTH - 3; j += 2) {

        if (rand() % 15 == 0 && map[i][j] == 3) { //난수생성

            if (star[i][j] == 7) //별이랑 겹치면 스킵
                continue;

            if (j % 2 == 0)
                j + 1;
            trap[i][j] = 88;

        }
    }
}



show_map();
show_star();
show_trap();



while (1) { //게임 메인 실행 파트

    if (starcount == 0)
        break;

    show_coin(0, playerA.coin); show_coin(1, playerB.coin);


    // 시간 check
    run_time = time(NULL) - start_time;
    remain_time = time_out - run_time;
    if (remain_time < last_remain_time) {
        show_time(remain_time); // 시간이 변할때만 출력
        last_remain_time = remain_time;
    }
    if (remain_time == 0) // 시간 종료
        break;


    char m1 = 0, m2 = 0; //동시이동
    if (GetAsyncKeyState(VK_UP) & 0x8000
        || GetAsyncKeyState(VK_DOWN) & 0x8000
        || GetAsyncKeyState(VK_LEFT) & 0x8000
        || GetAsyncKeyState(VK_RIGHT) & 0x8000
        || GetAsyncKeyState(0x57) & 0x8000 //W (UP)
        || GetAsyncKeyState(0x53) & 0x8000 //S (DOWN)
        || GetAsyncKeyState(0x41) & 0x8000 //A (LEFT)
        || GetAsyncKeyState(0x44) & 0x8000 //D (RIGHT)
        || GetAsyncKeyState(VK_RETURN) & 0x8000  //enter
        || GetAsyncKeyState(VK_SPACE) & 0x8000)  //space bar
    {


        if (GetAsyncKeyState(VK_UP))   //player 1
            m1 = UP;
        if (GetAsyncKeyState(VK_DOWN))
            m1 = DOWN;
        if (GetAsyncKeyState(VK_LEFT))
            m1 = LEFT;
        if (GetAsyncKeyState(VK_RIGHT))
            m1 = RIGHT;
        if (GetAsyncKeyState(VK_RETURN)) {
            m1 = 0;
            gotoxy(15, 15);
            //("%d", playerA.jump_count);
            playerA.jump_count++;
        }


        if (GetAsyncKeyState(0x57))      //player 2
            m2 = UP2;
        if (GetAsyncKeyState(0x53))
            m2 = DOWN2;
        if (GetAsyncKeyState(0x41))
            m2 = LEFT2;
        if (GetAsyncKeyState(0x44))
            m2 = RIGHT2;
        if (GetAsyncKeyState(VK_SPACE)) {

            m2 = 0;
            playerB.jump_count++;

        }


        if (playerA.coin == 0)
            m1 = 0;

        if (playerB.coin == 0)
            m2 = 0;


        //player 1
        if (playerA.coin != 0) {

            if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련

                if (frame_count % p1_frame_sync == 0) {
                    player1(ENTER);
                    //dir_count[0]--;
                }
                m1 = 0;
            }
            else if (frame_count % p1_frame_sync == 0)
                player1(m1);






            //점프조작

            if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) { //enter에서 손을 뗀 상태

                if (playerA.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드
                    playerA.jump = 1;

                else {
                    playerA.jump = 2;
                }

                playerA.jump_count = 0;


                dir_count[0] = 3;

                player1(ENTER);


            }
        }



        //player 2
        if (playerB.coin != 0) {

            if (dir_count[1] == 2 || dir_count[1] == 1) { //점프관련


                if (frame_count % p2_frame_sync == 0) {
                    player2(SPACE);
                    //dir_count[0]--;
                }
                m2 = 0;
            }
            else if (frame_count % p2_frame_sync == 0)
                player2(m2);



            //점프조작

            if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태



                if (playerB.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드
                    playerB.jump = 1;

                else {
                    playerB.jump = 2;
                }

                playerB.jump_count = 0;


                dir_count[1] = 3;

                player2(SPACE);



            }
        }


    }

    if (kbhit() == 1) {  // 키보드가 눌려져 있으면
        char c1;
        c1 = getch(); // key 값을 읽는다
        if (c1 == '\0') // VS 의 NULL 값은 Error이다.
            continue;
        else
            ch = c1;

        // ESC 누르면 프로그램 종료
        if (ch == ESC)
            exit(0);


        if (ch == NUMBER1) {      //코인추가
            if (playerA.coin == 5);
            else {
                playerA.coin++;
                playerA.heart[playerA.coin - 1] = 3;

                show_coin(0, playerA.coin);

            }
        }

        if (ch == NUMBER2) {  //코인추가
            if (playerB.coin == 5);

            else {
                playerB.coin++;
                playerB.heart[playerB.coin - 1] = 3;

                show_coin(1, playerB.coin);
            }
        }

    }
    else {  //keyboard 가 눌려지지 X 몬스터-움직임 , 플레이어 - 그자리 그대로


       //PLAYER 1
        if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련
            if (frame_count % p1_frame_sync == 0)
                player1(ENTER);
            //dir_count[0]--;


        }

        else if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) {

            if (playerA.jump_count < 25)
                playerA.jump = 1;

            else {
                playerA.jump = 2;
            }

            playerA.jump_count = 0;
            dir_count[0] = 3;
            player1(ENTER);
            //dir_count[0]--;
        }

        else if (frame_count % p1_frame_sync == 0)
            player1(0);


        //PLAYER 2
        if (dir_count[1] == 2 || dir_count[1] == 1) { //플레이어 2 점프관련
           //printf("dddd");
            if (frame_count % p2_frame_sync == 0)
                player2(SPACE);
            //dir_count[1]--;

        }

        else if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태

            if (playerB.jump_count < 25)   //점프
                playerB.jump = 1;

            else {
                playerB.jump = 2;
            }

            playerB.jump_count = 0;


            dir_count[1] = 3;

            player2(SPACE);

            //dir_count[1]--;


        }

        else if (frame_count % p2_frame_sync == 0)
            player2(0);



    }





    if (frame_count % monster1_frame_sync == 0) //몬스터 이동
        monster1(1);

    if (frame_count % monster2_frame_sync == 0) //몬스터 이동
        monster2(1);

    if (frame_count % monster3_frame_sync == 0) //몬스터 이동
        monster3(1);


    gotoxy(4, 3); show_heart(1);
    gotoxy(37, 3); show_heart(2);

    Sleep(Delay);
    frame_count = frame_count++; // frame_count 값으로 속도 조절을 한다.

} //메인 실행 파트 - 엔진


if (starcount == 0) { //별을 다 먹으면

    while (1) { //게임이 끝났을때
        int c1, c2;
        do { // 색을 변경하면서 출력
            c1 = rand() % 16;
            c2 = rand() % 16;
        } while (c1 == c2);
        textcolor(c1, c2);
        gotoxy(20, 12);
        printf(" Stage Clear!! ");
        gotoxy(14, 14);
        textcolor(WHITE, BLACK);
        printf(" Hit (G) B Stage (Q) to Quit ");



        Sleep(500);

        if (kbhit()) {
            ch = getch();
            if (ch == 'Q' || ch == 'q')
                exit(0);
            else {
                if (ch == 'g' || ch == 'G')
                    break;
            }
        }



    }
}

else {
    while (1) { //게임이 끝났을때
        int c1, c2;
        do { // 색을 변경하면서 Game Over 출력
            c1 = rand() % 16;
            c2 = rand() % 16;
        } while (c1 == c2);
        textcolor(c1, c2);
        gotoxy(20, 11);
        printf("** Time Out **");
        gotoxy(12, 13);
        textcolor(WHITE, BLACK);
        printf(" Hit (R) to Restart (Q) to Quit ");
        Sleep(500);
        if (kbhit()) {
            ch = getch();
            if (ch == 'r' || ch == 'q' || ch == 'R' || ch == 'Q')
                break;
        }
    }
    if (ch == 'r' || ch == 'R') {

        playerA.coin = 0;
        playerA.heart[5] = 0;

        playerB.coin = 0;
        playerB.heart[5] = 0;
        goto START; //시작부분으로 돌아가서 초기화하고 다시 시작

    }

    gotoxy(0, HEIGHT - 1);
}

//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★ 
//Bonus Stage


last_stage = 1;

cls(WHITE, BLACK);

for (int i = 0; i < 5; i++) {
    called[i] = 0;
}

playerA.coin = 5;
playerB.coin = 5;

time_out = 30;


start_time = time(NULL);
last_remain_time = remain_time = time_out;




show_time(remain_time);


//맵, 별 초기화
for (int i = 4; i < HEIGHT - 1; i++) {
    for (int j = 2; j < WIDTH - 1; j += 2) {
        map[i][j] = 0;
        star[i][j] = 0;
        trap[i][j] = 0;
    }
}

starcount = 0;
srand(time(NULL));

//상자
draw_box2(WIDTH, HEIGHT);

start_time = time(NULL);

//맵- 필드 
//블럭 : 1  /  움직일수 있는 범위 : 3  
for (int i = 0; i < 1; i++) {

    //맨 위 블럭
    for (int i = 4; i < 16; i++) {
        map[9][i] = 1;
        map[8][i] = 3;

    }
    for (int i = 18; i < 36; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }

    for (int i = 38; i < 50; i++) {
        map[9][i] = 1;
        map[8][i] = 3;
    }



    //중간 블럭
    for (int i = 4; i < 50; i++) {
        map[14][i] = 1;
        map[13][i] = 3;
    }




    //맨 아래 블럭
    for (int i = 4; i < 16; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }
    //map[18][10] = 3;

    for (int i = 18; i < 36; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }

    for (int i = 38; i < 50; i++) {
        map[19][i] = 1;
        map[18][i] = 3;
    }


    for (int i = 1; i < WIDTH - 1; i++) {
        map[23][i] = 3;
        map[24][i] = 5;
        map[25][i] = 5;
    }

}

//맵- 사다리 : 4,40
for (int i = 0; i < 1; i++) {

    //사다리와 이어진 ㅁ 부분 , 9, 14
    map[9][10] = 40;

    map[9][26] = 40;

    map[9][44] = 40;
    for (int i = 10; i < 14; i++) {
        map[i][10] = 4;
        map[i][26] = 4;
        map[i][44] = 4;
    }

    map[14][6] = 40;

    map[14][20] = 40;

    map[14][32] = 40;

    map[14][46] = 40;

    for (int i = 15; i < 19; i++) {
        map[i][6] = 4;
        map[i][20] = 4;
        map[i][32] = 4;
        map[i][46] = 4;
    }




    map[19][26] = 40;


    for (int i = 20; i < 24; i++) {

        map[i][26] = 4;

    }
}





show_map();


while (1) { //게임 실행 파트




    show_score(0);
    show_score(1);


    // 시간 check
    run_time = time(NULL) - start_time;
    remain_time = time_out - run_time;
    if (remain_time < last_remain_time) {
        show_time(remain_time); // 시간이 변할때만 출력
        last_remain_time = remain_time;
    }
    if (remain_time == 0) // 시간 종료
        break;


    char m1 = 0, m2 = 0; //동시이동
    if (GetAsyncKeyState(VK_UP) & 0x8000
        || GetAsyncKeyState(VK_DOWN) & 0x8000
        || GetAsyncKeyState(VK_LEFT) & 0x8000
        || GetAsyncKeyState(VK_RIGHT) & 0x8000
        || GetAsyncKeyState(0x57) & 0x8000 //W (UP)
        || GetAsyncKeyState(0x53) & 0x8000 //S (DOWN)
        || GetAsyncKeyState(0x41) & 0x8000 //A (LEFT)
        || GetAsyncKeyState(0x44) & 0x8000 //D (RIGHT)
        || GetAsyncKeyState(VK_RETURN) & 0x8000  //enter
        || GetAsyncKeyState(VK_SPACE) & 0x8000)  //space bar
    {


        if (GetAsyncKeyState(VK_UP))   //player 1
            m1 = UP;
        if (GetAsyncKeyState(VK_DOWN))
            m1 = DOWN;
        if (GetAsyncKeyState(VK_LEFT))
            m1 = LEFT;
        if (GetAsyncKeyState(VK_RIGHT))
            m1 = RIGHT;
        if (GetAsyncKeyState(VK_RETURN)) {
            m1 = 0;
            gotoxy(15, 15);
            playerA.jump_count++;
        }


        if (GetAsyncKeyState(0x57))      //player 2
            m2 = UP2;
        if (GetAsyncKeyState(0x53))
            m2 = DOWN2;
        if (GetAsyncKeyState(0x41))
            m2 = LEFT2;
        if (GetAsyncKeyState(0x44))
            m2 = RIGHT2;
        if (GetAsyncKeyState(VK_SPACE)) {

            m2 = 0;
            playerB.jump_count++;

        }


        if (playerA.coin == 0)
            m1 = 0;

        if (playerB.coin == 0)
            m2 = 0;


        //player 1
        if (playerA.coin != 0) {

            if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련

                if (frame_count % p1_frame_sync == 0) {
                    player1(ENTER);
                    //dir_count[0]--;
                }
                m1 = 0;
            }
            else if (frame_count % p1_frame_sync == 0)
                player1(m1);



            ch = 1;//밑에서 한번 더 움직이는거 방지


            //점프조작

            if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) { //enter에서 손을 뗀 상태

                if (playerA.jump_count < 25)   //점프 길게들어오면 길게점프 아니면 짧게 점프하는 코드
                    playerA.jump = 1;

                else {
                    playerA.jump = 2;
                }
                playerA.jump_count = 0;


                dir_count[0] = 3;

                player1(ENTER);



            }
        }



        //player 2
        if (playerB.coin != 0) {

            if (dir_count[1] == 2 || dir_count[1] == 1) { //점프관련


                if (frame_count % p2_frame_sync == 0) {
                    player2(SPACE);
                    //dir_count[0]--;
                }
                m2 = 0;
            }
            else if (frame_count % p2_frame_sync == 0)
                player2(m2);



            //점프조작

            if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태

               //printf("%d", playerB.jump_count);

                if (playerB.jump_count < 25)   //점프 
                    playerB.jump = 1;

                else {
                    playerB.jump = 2;
                }

                playerB.jump_count = 0;


                dir_count[1] = 3;

                player2(SPACE);



            }
        }

    }

    if (kbhit() == 1) {  // 키보드가 눌려져 있으면
        char c1;
        c1 = getch(); // key 값을 읽는다
        if (c1 == '\0') // VS 의 NULL 값은 Error이다.
            continue;
        else
            ch = c1;

        // ESC 누르면 프로그램 종료
        if (ch == ESC)
            exit(0);


        if (ch == NUMBER1) {      //코인추가
            if (playerA.coin == 5);
            else {
                playerA.coin++;
                playerA.heart[playerA.coin - 1] = 3;

                show_coin(0, playerA.coin);

            }
        }

        if (ch == NUMBER2) {  //코인추가
            if (playerB.coin == 5);

            else {
                playerB.coin++;
                playerB.heart[playerB.coin - 1] = 3;

                show_coin(1, playerB.coin);
            }
        }

    }
    else {  //keyboard 가 눌려지지 X 몬스터-움직임 , 플레이어 - 그자리 그대로


       //PLAYER 1
        if (dir_count[0] == 2 || dir_count[0] == 1) { //점프관련
            if (frame_count % p1_frame_sync == 0)
                player1(ENTER);
            //dir_count[0]--;


        }

        else if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && playerA.jump_count != 0) {

            if (playerA.jump_count < 25)
                playerA.jump = 1;

            else {
                playerA.jump = 2;
            }

            playerA.jump_count = 0;
            dir_count[0] = 3;
            player1(ENTER);
            //dir_count[0]--;
        }

        else if (frame_count % p1_frame_sync == 0)
            player1(0);


        //PLAYER 2
        if (dir_count[1] == 2 || dir_count[1] == 1) { //플레이어 2 점프관련

            if (frame_count % p2_frame_sync == 0)
                player2(SPACE);


        }

        else if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && playerB.jump_count != 0) { //enter에서 손을 뗀 상태

            if (playerB.jump_count < 25)   //점프
                playerB.jump = 1;

            else {
                playerB.jump = 2;
            }

            playerB.jump_count = 0;


            dir_count[1] = 3;

            player2(SPACE);




        }

        else if (frame_count % p2_frame_sync == 0)
            player2(0);



    }



    Sleep(Delay);
    frame_count = frame_count++; // frame_count 값으로 속도 조절을 한다.

} //메인 실행 파트 - 엔진


//게임 끝!



while (1) {
    int c1, c2;
    do { // 색을 변경하면서 출력
        c1 = rand() % 16;
        c2 = rand() % 16;
    } while (c1 == c2);
    textcolor(c1, c2);

    if (score[0] > score[1]) {

        gotoxy(16, 10);
        printf(" WINNER IS PLAYER1!! ");
        gotoxy(12, 13);
        textcolor(WHITE, BLACK);
        printf(" Hit (R) to Restart (Q) to Quit");
    }

    else if (score[0] < score[1]) {

        gotoxy(16, 10);
        printf(" WINNER IS PLAYER2!! ");
        gotoxy(12, 13);
        textcolor(WHITE, BLACK);
        printf(" Hit (R) to Restart (Q) to Quit");
    }
    else {
        gotoxy(16, 10);
        printf("       DRAW!!        ");
        gotoxy(12, 13);
        textcolor(WHITE, BLACK);
        printf(" Hit (R) to Restart (Q) to Quit");
    }


    Sleep(300);
    if (kbhit()) {
        ch = getch();
        if (ch == 'Q' || ch == 'q')
            exit(1);
        else if (ch == 'r' || ch == 'R') {


            init_game();
            last_stage == 0;


            playerA.coin = 0;
            playerA.heart[5] = 0;

            playerB.coin = 0;
            playerB.heart[5] = 0;
            goto FIRST; //시작부분으로 돌아가서 초기화하고 다시 시작

        }

        gotoxy(0, HEIGHT - 1);
    }
}



}