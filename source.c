#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <conio.h>

#include <Windows.h>

#include <time.h>

#pragma warning(disable:4996)

 

/* 출력 블록 모양 정의 */

#define EMPTY_SHAPE "　"

#define WALL_SHAPE "▩"

#define FOOD_SHAPE "■"

#define SNAKE_HEAD_SHAPE "▣"

#define SNAKE_BODY_SHAPE "□"

 

/* 출력용 좌표 변환 함수 */

#define CONVERT_X(x) ((x) << 1) + BOARD_START_POS_X

#define CONVERT_Y(y) (y) + BOARD_START_POS_Y

 

#define GAME_THREAD while (true)

 

/* ASCII code 값 */

typedef enum ASCII_code_of_key_used {

    UP = 72,

    DOWN = 80,

    LEFT = 75,

    RIGHT = 77,

    ENTER = 13,

    PAUSE = 32,

    X = 120

} ASCII;

 

/* 출력 블럭 종류 */

enum _type_of_output {

    EMPTY,

    WALL,

    FOOD,

    SNAKE_HEAD,

    SNAKE_BODY

};

 

/* 난이도 종류 */

typedef enum _difficulty {

    EASY,

    NORMAL,

    HARD,

    DIFFICULTY_COUNT

} difficulty;

 

/* 게임판 설정 (수치를 바꿔도 되지만 콘솔창 크기도 재설정 해야함) */

enum _setting_board {

    BOARD_START_POS_X = 4,

    BOARD_START_POS_Y = 2,

    BOARD_WIDTH = 23,

    BOARD_HEIGHT = 19

};

 

/* 초기 뱀 상태 설정 */

enum _setting_snake {

    SNAKE_START_POS_X = BOARD_WIDTH >> 1,    //시작 위치 변경 가능

    SNAKE_START_POS_Y = BOARD_HEIGHT >> 1,

    SNAKE_START_LEVEL = 1,

    SNAKE_MAX_LEVEL = 10,

    SNAKE_BASIC_LENGTH = 4,        //기준 길이 변경 가능(3이상)

    SNAKE_BASIC_SPEED = 300,    //기준 속도 변경 가능

    SNAKE_SPEED_FLUCTUATION_VALUE = SNAKE_BASIC_SPEED / SNAKE_MAX_LEVEL

};

 

/* snake를 이루는 블럭 */

typedef struct _SNAKE_BLOCK {

    struct _SNAKE_BLOCK* next;    //다음 블럭

    struct _SNAKE_BLOCK* prev;    //이전 블럭

    COORD pos;                    //블럭의 좌표

    char shape;                    //블럭 모양

} SNAKE_BLOCK, * P_SNAKE_BLOCK;

 

/* snank 구조체 */

typedef struct _SNAKE {

    P_SNAKE_BLOCK head;    //머리 블럭 포인터

    P_SNAKE_BLOCK tail;    //꼬리 블럭 포인터

    ASCII direction;    //진행 방향

    short food_count;    //먹은 food 개수

    int level;            //현재 레벨

    int length;            //현재 길이

    int power_count;        //남은 power개수

    clock_t extension_time;    //느려지는 clock수치

    clock_t speed;                //속도

    clock_t speed_fluctuation;    //속도 변화량

    difficulty difficulty;    //난이도

} SNAKE, * P_SNAKE;

 

typedef char block_t;

 

/****************************************************************************************************

 *                                                                                                    *

 *                                        FUNCTION DECLARATION                                        *

 *                                                                                                    *

 ****************************************************************************************************/

 

 /* game board 관련 함수 */

void draw_game_board(const block_t(*board)[BOARD_WIDTH]);  //데이터를 기반으로 게임판을 콘솔창에 그림

void set_board_border(block_t(*board)[BOARD_WIDTH]);  //게임 테두리 데이터 구축

/* snake 구현 관련 함수 */

void set_snake(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);  //뱀 데이터를 기반으로 board에 데이터 구축

void create_new_snake(P_SNAKE p_snake);  //뱀 데이터 생성

void setting_snake_according_to_difficulty(P_SNAKE p_snake);  //난이도에 따른 뱀 설정 (속도, power개수 등)

void new_head(P_SNAKE p_snake);  //head node 생성

void new_tail(P_SNAKE p_snake);  //taile node 생성

void insert_body_block(P_SNAKE p_snake);    //head와 첫 번째 노드 사이에 새 노드 추가, 진행 방향에 따라 head뒤의 좌표값을 가짐

void set_node_coord_behind_head(P_SNAKE p_snake, P_SNAKE_BLOCK block);  //특정 노드를 head node 뒤의 좌표로 설정

void set_node_location_behind_head(P_SNAKE p_snake, P_SNAKE_BLOCK block);  //특정 노드를 head node 뒤의 위치로 설정

void delete_snake(P_SNAKE p_snake);  //뱀을 이루는 연결리스트 삭제

void delete_snake_all_body(P_SNAKE p_snake);  //뱀 몸통을 이루는 연결리스트 모두 삭제

/* 이동 관련 함수 */

void move_forward(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);

void move_head(P_SNAKE p_snake);    //진행 방향에 따라 head의 좌표값 1증가

void change_tailnode(P_SNAKE p_snake);    //tail node를 head node 바로 뒤의 노드와 바꿈

void redraw_snake(const P_SNAKE p_snake);    //이동하면서 바뀐 부분만 재 출력

/* 행동 관련 함수 */

void user_input_time(P_SNAKE p_snake);  //game thread 에서 키를 입력하는 부분 (입력이 없으면 넘어감)

ASCII get_key();  //입력한 키의 ASCII code 반환

void game_pause();

void management_extension_time(clock_t* p_extension_time);  //느려지는 시간 관리 함수

void decide_action(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);  //game thread 에서 행동을 결정하는 부분

void create_food(block_t(*board)[BOARD_WIDTH]);  //food 생성

/* 조건 확인 관련 함수 */

bool is_extended_key(const ASCII ASCII_code);

block_t check_front_block(const P_SNAKE p_snake, const block_t(*board)[BOARD_WIDTH], bool is_checking_last_block);

bool is_last_block_of_snake(const P_SNAKE p_snake);  //앞의 블럭이 뱀의 마지막 블럭인지 확인

bool is_empty_to_coord(COORD pos, block_t(*board)[BOARD_WIDTH]);  //좌표로 빈공간인지 확인

void check_level_up(P_SNAKE p_snake);  //먹은 food 개수로 레벨업 체크

/* 게임 시작, 종료 관련 함수 */

void draw_title(P_SNAKE p_snake);

void draw_game_title();

void choose_difficulty(P_SNAKE p_snake);

void draw_difficulty_selection_menu(COORD start_pos);  //난이도 선택 메뉴 출력

void display_selected_difficulty(difficulty difficulty_type, COORD start_pos);  //선택한 난이도 화살표로 표시

void game_start(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);  //게임 시작 루틴

void game_over(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);  //게임 오버 루틴

void save_top_score(const P_SNAKE p_snake);  //최고 점수 저장

bool open_score_file(FILE** file_pointer, const char* _Mode);  //데이터 파일 열기 시도, 성공 = 1, 실패 = 0

void close_score_file(FILE** file_pointer);  //파일포인터가 NULL이 아닐때 파일 닫음

void compare_scores(const P_SNAKE p_snake, int score_array[]);  //점수 비교

void store_score_in_array(FILE* fpr, int score_array[]);  //파일에서 점수를 가져와 배열에 저장

void store_score_to_file(FILE* fpw, int score_array[]);  //배열에 있는 점수를 파일에 저장

void score_encryption_and_decryption(int score_array[]);  //점수 숫자 암호화, 복호화

void decide_retry(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]);

/* 정보 출력 관련 함수 */

void show_info(const P_SNAKE p_snake);

void show_difficulty(const P_SNAKE p_snake);

void draw_snake_info_border();

void show_level(const P_SNAKE p_snake);

void show_length(const P_SNAKE p_snake);

void show_power_count(const P_SNAKE p_snake);

void show_move_key_info(COORD pos);

void show_pause_key_info(COORD pos);

void show_power_key_info(COORD pos);

void show_recommended_font_size(COORD pos);

void show_top_score(const P_SNAKE p_snake, COORD pos);

/* 커서 관련 함수 */

void move_cursor_based_on_topright_of_board(SHORT x_coord_fluctuation, SHORT y_coord_fluctuation);

void move_cursor_based_on_bottomleft_of_board(SHORT x_coord_fluctuation, SHORT y_coord_fluctuation);

void goto_xy(SHORT pos_x, SHORT pos_y);    //(x,y)로 커서이동

void cursor_view(char s);    //0:커서숨김 | 1:커서표시

 

/****************************************************************************************************

 *                                                                                                    *

 *                                            MAIN FUNCTION                                            *

 *                                                                                                    *

 ****************************************************************************************************/

 

int main(void) {

    block_t board[BOARD_HEIGHT][BOARD_WIDTH];            //게임보드

    P_SNAKE p_snake = (P_SNAKE)malloc(sizeof(SNAKE));    //뱀

    

    draw_title(p_snake);            //게임 타이틀 표시

    game_start(p_snake, board);        //게임 시작

    GAME_THREAD{                    //THREAD

        user_input_time(p_snake);        //사용자 키 입력

        decide_action(p_snake, board);    //뱀 행동

    }

    return 0;

}

 

/****************************************************************************************************

 *                                                                                                    *

 *                                        DEFINITION OF FUNCTION                                        *

 *                                                                                                    *

 ****************************************************************************************************/

 

 /* game board 관련 함수 정의 */

void draw_game_board(const block_t(*board)[BOARD_WIDTH]) {

    int x, y;

    for (y = 0;y < BOARD_HEIGHT;y++) {

        for (x = 0;x < BOARD_WIDTH;x++) {

            goto_xy(CONVERT_X(x), CONVERT_Y(y));

            switch (board[y][x]) {

            case(EMPTY):

                printf(EMPTY_SHAPE);

                break;

            case(WALL):

                printf(WALL_SHAPE);

                break;

            case(FOOD):

                printf(FOOD_SHAPE);

                break;

            case(SNAKE_HEAD):

                printf(SNAKE_HEAD_SHAPE);

                break;

            case(SNAKE_BODY):

                printf(SNAKE_BODY_SHAPE);

                break;

            default:

                printf("error:draw_game_board");

                exit(0);

                break;

            }

        }

    }

}

void set_board_border(block_t(*board)[BOARD_WIDTH]) {

    int x, y;

    //board 초기화

    for (y = 0;y < BOARD_HEIGHT;y++) {

        for (x = 0;x < BOARD_WIDTH;x++) {

            board[y][x] = EMPTY;

        }

    }

    //상단 테두리

    for (x = 0;x < BOARD_WIDTH;x++) {

        board[0][x] = WALL;

    }

    //하단 테두리

    for (x = 0;x < BOARD_WIDTH;x++) {

        board[BOARD_HEIGHT - 1][x] = WALL;

    }

    //좌측 테두리

    for (y = 0;y < BOARD_HEIGHT;y++) {

        board[y][0] = WALL;

    }

    //우측 테두리

    for (y = 0;y < BOARD_HEIGHT;y++) {

        board[y][BOARD_WIDTH - 1] = WALL;

    }

}

/* snake 구현 관련 함수 정의 */

void set_snake(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    P_SNAKE_BLOCK curr = p_snake->head;

    board[curr->pos.Y][curr->pos.X] = SNAKE_HEAD;    //머리 블럭 설정

    curr = curr->next;

    while (curr != p_snake->tail) {

        board[curr->pos.Y][curr->pos.X] = SNAKE_BODY;    //몸통 블럭 설정

        curr = curr->next;

    }

    board[curr->pos.Y][curr->pos.X] = EMPTY;    //꼬리 블럭 설정

}

void create_new_snake(P_SNAKE p_snake) {

    int i;

    p_snake->direction = RIGHT;                //시작 진행 방향 설정

    p_snake->food_count = 0;            //먹은 개수 초기화

    p_snake->level = SNAKE_START_LEVEL;    //시작레벨 설정

    p_snake->extension_time = 0;        //power사용 효과 초기화

    setting_snake_according_to_difficulty(p_snake);

    p_snake->head = (P_SNAKE_BLOCK)malloc(sizeof(SNAKE_BLOCK));

    p_snake->tail = (P_SNAKE_BLOCK)malloc(sizeof(SNAKE_BLOCK));

    new_head(p_snake);

    new_tail(p_snake);

    for (i = p_snake->length - 1;i > 0;i--) {    //총 길이에서 머리와 꼬리(EMPTY)를 빼면 (총 길이-1)번 추가해야 함

        insert_body_block(p_snake);

        /* 초기에는 head좌표 변동이 없으니 추가된 body블럭들이 나란히 이어지게 좌표 조정 */

        p_snake->head->next->pos.X -= (i - 1);

    }

}

void setting_snake_according_to_difficulty(P_SNAKE p_snake) {

    switch (p_snake->difficulty) {

    case(EASY):

        p_snake->speed = SNAKE_BASIC_SPEED + 100;

        p_snake->speed_fluctuation = SNAKE_SPEED_FLUCTUATION_VALUE + 10;

        p_snake->length = SNAKE_BASIC_LENGTH - 1;

        p_snake->power_count = 5;

        break;

    case(NORMAL):

        p_snake->speed = SNAKE_BASIC_SPEED;

        p_snake->speed_fluctuation = SNAKE_SPEED_FLUCTUATION_VALUE;

        p_snake->length = SNAKE_BASIC_LENGTH + 2;

        p_snake->power_count = 3;

        break;

    case(HARD):

        p_snake->speed = SNAKE_BASIC_SPEED - 100;

        p_snake->speed_fluctuation = SNAKE_SPEED_FLUCTUATION_VALUE - 10;

        p_snake->length = SNAKE_BASIC_LENGTH + 6;

        p_snake->power_count = 1;

        break;

    }

}

void new_head(P_SNAKE p_snake) {

    p_snake->head->pos.X = SNAKE_START_POS_X;

    p_snake->head->pos.Y = SNAKE_START_POS_Y;

    p_snake->head->shape = SNAKE_HEAD;

    p_snake->head->prev = NULL;

    p_snake->head->next = p_snake->tail;

}

void new_tail(P_SNAKE p_snake) {

    p_snake->tail->pos.X = SNAKE_START_POS_X - p_snake->length;

    p_snake->tail->pos.Y = SNAKE_START_POS_Y;

    p_snake->tail->shape = EMPTY;

    p_snake->tail->prev = p_snake->head;

    p_snake->tail->next = NULL;

}

void insert_body_block(P_SNAKE p_snake) {

    P_SNAKE_BLOCK new_block = (P_SNAKE_BLOCK)malloc(sizeof(SNAKE_BLOCK));

    new_block->shape = SNAKE_BODY;

    set_node_coord_behind_head(p_snake, new_block);        //new_block의 좌표 지정

    set_node_location_behind_head(p_snake, new_block);    //new_block의 노드 위치 지정

}

void set_node_coord_behind_head(P_SNAKE p_snake, P_SNAKE_BLOCK block) {

    switch (p_snake->direction) {

    case(UP):

        block->pos.X = p_snake->head->pos.X;

        block->pos.Y = p_snake->head->pos.Y + 1;

        break;

    case(DOWN):

        block->pos.X = p_snake->head->pos.X;

        block->pos.Y = p_snake->head->pos.Y - 1;

        break;

    case(LEFT):

        block->pos.X = p_snake->head->pos.X + 1;

        block->pos.Y = p_snake->head->pos.Y;

        break;

    case(RIGHT):

        block->pos.X = p_snake->head->pos.X - 1;

        block->pos.Y = p_snake->head->pos.Y;

        break;

    }

}

void set_node_location_behind_head(P_SNAKE p_snake, P_SNAKE_BLOCK block) {

    block->next = p_snake->head->next;    //현재 첫 번째 노드를 block의 다음 노드로 설정

    block->prev = p_snake->head;        //head를 block의 이전 노드로 설정

    p_snake->head->next->prev = block;    //현재 첫 번째 노드의 이전 노드를 head에서 block로 바꿈

    p_snake->head->next = block;        //head의 다음 노드를 block로 바꿈

}

void delete_snake(P_SNAKE p_snake) {

    delete_snake_all_body(p_snake);

    free(p_snake->head);

    p_snake->head = NULL;

    free(p_snake->tail);

    p_snake->tail = NULL;

}

void delete_snake_all_body(P_SNAKE p_snake) {

    P_SNAKE_BLOCK temp_block;

    while (p_snake->head->next != p_snake->tail) {

        temp_block = p_snake->head->next;

        p_snake->head->next = p_snake->head->next->next;

        p_snake->head->next->prev = p_snake->head;

        free(temp_block);

    }

}

/* 이동 관련 함수 정의 */

void move_forward(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    board[p_snake->head->pos.Y][p_snake->head->pos.X] = SNAKE_BODY;    //기존 head 위치 몸통 블럭으로 교체

    move_head(p_snake);        //head좌표 이동

    change_tailnode(p_snake);

    board[p_snake->tail->pos.Y][p_snake->tail->pos.X] = EMPTY;        //새로운 tail 위치 비움

    board[p_snake->head->pos.Y][p_snake->head->pos.X] = SNAKE_HEAD;    //새로운 head 위치 머리 블럭 설정

    redraw_snake(p_snake);

}

void move_head(P_SNAKE p_snake) {

    switch (p_snake->direction) {

    case(UP):

        p_snake->head->pos.Y--;

        break;

    case(DOWN):

        p_snake->head->pos.Y++;

        break;

    case(LEFT):

        p_snake->head->pos.X--;

        break;

    case(RIGHT):

        p_snake->head->pos.X++;

        break;

    }

}

void change_tailnode(P_SNAKE p_snake) {

    p_snake->tail->shape = SNAKE_BODY;        //현재 tail의 모양을 BODY로 바꿈

    p_snake->tail = p_snake->tail->prev;    //새로운 tail 설정

    p_snake->tail->shape = EMPTY;            //new tail의 모양은 EMPTY

    /* new tail로 이전 tail에 접근하여 이전 tail 노드의 좌표, 연결리스트 위치 조정 */

    set_node_coord_behind_head(p_snake, p_snake->tail->next);

    set_node_location_behind_head(p_snake, p_snake->tail->next);

    p_snake->tail->next = NULL;        //new tail의 next 노드는 NULL

}

void redraw_snake(const P_SNAKE p_snake) {

    goto_xy(CONVERT_X(p_snake->tail->pos.X), CONVERT_Y(p_snake->tail->pos.Y));

    printf(EMPTY_SHAPE);

    goto_xy(CONVERT_X(p_snake->head->pos.X), CONVERT_Y(p_snake->head->pos.Y));

    printf(SNAKE_HEAD_SHAPE);

    goto_xy(CONVERT_X(p_snake->head->next->pos.X), CONVERT_Y(p_snake->head->next->pos.Y));

    printf(SNAKE_BODY_SHAPE);

}

/* 행동 관련 함수 정의 */

void user_input_time(P_SNAKE p_snake) {

    if (p_snake->extension_time > 0) management_extension_time(&p_snake->extension_time);

    _sleep(p_snake->speed - (p_snake->speed_fluctuation * (p_snake->level - 1)) + p_snake->extension_time);    //입력 제한 시간

    if (kbhit()) {

        switch (get_key()) {

        case(UP):

            if (p_snake->head->next->pos.Y != p_snake->head->pos.Y - 1) p_snake->direction = UP;

            break;

        case(DOWN):

            if (p_snake->head->next->pos.Y != p_snake->head->pos.Y + 1) p_snake->direction = DOWN;

            break;

        case(LEFT):

            if (p_snake->head->next->pos.X != p_snake->head->pos.X - 1) p_snake->direction = LEFT;

            break;

        case(RIGHT):

            if (p_snake->head->next->pos.X != p_snake->head->pos.X + 1) p_snake->direction = RIGHT;

            break;

        case(PAUSE):    //일시정지

            game_pause();

            break;

        case(X):

            if (p_snake->power_count != 0) {

                p_snake->power_count--;

                p_snake->extension_time = 400;

                show_power_count(p_snake);

            }

            break;

        default:

            break;

        }

    }

}

ASCII get_key() {

    ASCII key_value;

    key_value = getch();

    if (is_extended_key(key_value)) key_value = getch();

    return key_value;

}

void game_pause() {

    int output_pos_x = CONVERT_X(BOARD_WIDTH) - 8;

    int output_pos_y = BOARD_START_POS_Y + BOARD_HEIGHT + 1;

    cursor_view(1);

    goto_xy(output_pos_x, output_pos_y);

    printf("[PAUSE]");

    goto_xy(output_pos_x - 24, output_pos_y + 2);

    printf("Press the Space bar to start...");

    while (getch() != PAUSE);

    goto_xy(output_pos_x, output_pos_y);

    printf("       ");

    goto_xy(output_pos_x - 24, output_pos_y + 2);

    printf("                               ");

    cursor_view(0);

}

void management_extension_time(clock_t* p_extension_time) {

    static clock_t std_power_clock;

    clock_t lapse_power_clock = clock();

    if (lapse_power_clock - std_power_clock > 300) {    //power를 사용하고 일정 시간이 경과했을 때

        std_power_clock = clock();    //기준 clock 초기화

        *p_extension_time -= 20;

    }

}

void decide_action(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    switch (check_front_block(p_snake, board, false)) {

    case(EMPTY):        //move

        move_forward(p_snake, board);

        break;

    case(FOOD):            //extend

        move_head(p_snake);

        insert_body_block(p_snake);

        set_snake(p_snake, board);

        redraw_snake(p_snake);

        create_food(board);

        check_level_up(p_snake);

        p_snake->length++;

        show_length(p_snake);

        break;

    case(WALL):            //die

        game_over(p_snake, board);

        break;

    case(SNAKE_BODY):    //move or die

        if (check_front_block(p_snake, board, true)) move_forward(p_snake, board);

        else game_over(p_snake, board);

        break;

    default:  //정한 블럭이 아닌 다른 무언가가 있을때...ㄷㄷ

        goto_xy(0, 0);

        printf("decide_action_Error");

        exit(0);

        break;

    }

}

void create_food(block_t(*board)[BOARD_WIDTH]) {

    COORD pos;

    do {    //랜덤 좌표가 비어있지 않으면 좌표 지정 반복

        pos.X = (rand() % (BOARD_WIDTH - 2)) + 1;

        pos.Y = (rand() % (BOARD_HEIGHT - 2)) + 1;

    } while (!is_empty_to_coord(pos, board));

    board[pos.Y][pos.X] = FOOD;

    goto_xy(CONVERT_X(pos.X), CONVERT_Y(pos.Y));

    printf(FOOD_SHAPE);

}

/* 조건 확인 관련 함수 정의 */

bool is_extended_key(const ASCII ASCII_code) {

    if (ASCII_code == 224 || ASCII_code == 0) return true;

    else return false;

}

block_t check_front_block(const P_SNAKE p_snake, const block_t(*board)[BOARD_WIDTH], bool is_checking_last_block) {

    if (is_checking_last_block) return is_last_block_of_snake(p_snake);    //전방 블록이 snake의 마지막 블록인지 검사할 때

    else {        //일반적으로 앞의 블록 검사

        switch (p_snake->direction) { 

        case(UP):

            return board[p_snake->head->pos.Y - 1][p_snake->head->pos.X];

        case(DOWN):

            return board[p_snake->head->pos.Y + 1][p_snake->head->pos.X];

        case(LEFT):

            return board[p_snake->head->pos.Y][p_snake->head->pos.X - 1];

        case(RIGHT):

            return board[p_snake->head->pos.Y][p_snake->head->pos.X + 1];

        default: return false;

        }

    }

}

bool is_last_block_of_snake(const P_SNAKE p_snake) {

    switch (p_snake->direction) {

    case(UP):

        if ((p_snake->head->pos.X == p_snake->tail->prev->pos.X) && (p_snake->head->pos.Y - 1 == p_snake->tail->prev->pos.Y)) return true;

        else return false;

    case(DOWN):

        if ((p_snake->head->pos.X == p_snake->tail->prev->pos.X) && (p_snake->head->pos.Y + 1 == p_snake->tail->prev->pos.Y)) return true;

        else return false;

    case(LEFT):

        if ((p_snake->head->pos.X - 1 == p_snake->tail->prev->pos.X) && (p_snake->head->pos.Y == p_snake->tail->prev->pos.Y)) return true;

        else return false;

    case(RIGHT):

        if ((p_snake->head->pos.X + 1 == p_snake->tail->prev->pos.X) && (p_snake->head->pos.Y == p_snake->tail->prev->pos.Y)) return true;

        else return false;

    default: return false;

    }

}

bool is_empty_to_coord(COORD pos, block_t(*board)[BOARD_WIDTH]) {

    return (board[pos.Y][pos.X] == EMPTY);

}

void check_level_up(P_SNAKE p_snake) {

    static const short target_food_count[SNAKE_MAX_LEVEL] = { 2, 3, 4, 5, 7, 9, 11, 14, 18, 9999 };

    p_snake->food_count++;

    if (p_snake->food_count >= target_food_count[p_snake->level - 1] && p_snake->level < (SNAKE_MAX_LEVEL - 1)) {

        p_snake->level++;

        show_level(p_snake);

        p_snake->food_count = 0;

    }

}

/* 게임 시작, 종료 관련 함수 정의 */

void draw_title(P_SNAKE p_snake) {

    system("tiTle snake game");    //콘솔창 이름 변경

    system("mode con: cols=78 lines=26");    //콘솔창 크기 변경

    cursor_view(0);

    draw_game_title();

    choose_difficulty(p_snake);

    system("cls");

}

void draw_game_title() {

    COORD start_pos = { 10, 4 };

    goto_xy(start_pos.X, start_pos.Y);

    printf("　■■■　　■　　　■　　■■■　　■　　　■　■■■■■");

    goto_xy(start_pos.X, start_pos.Y + 1);

    printf("■　　　■　■■　　■　■　　　■　■　　■　　■　　　　");

    goto_xy(start_pos.X, start_pos.Y + 2);

    printf("■　　　　　■■　　■　■　　　■　■　■　　　■　　　　");

    goto_xy(start_pos.X, start_pos.Y + 3);

    printf("　■■■　　■　■　■　■■■■■　■■　　　　■■■■■");

    goto_xy(start_pos.X, start_pos.Y + 4);

    printf("　　　　■　■　　■■　■　　　■　■　■　　　■　　　　");

    goto_xy(start_pos.X, start_pos.Y + 5);

    printf("■　　　■　■　　■■　■　　　■　■　　■　　■　　　　");

    goto_xy(start_pos.X, start_pos.Y + 6);

    printf("　■■■　　■　　　■　■　　　■　■　　　■　■■■■■");

}

void choose_difficulty(P_SNAKE p_snake) {

    difficulty difficulty_type = EASY;

    COORD start_pos = { 26, 20 };

    draw_difficulty_selection_menu(start_pos);

    while (true) {

        display_selected_difficulty(difficulty_type, start_pos);

        switch (get_key()) {

        case(LEFT):

            if (difficulty_type != EASY) difficulty_type--;

            break;

        case(RIGHT):

            if (difficulty_type != HARD) difficulty_type++;

            break;

        case(ENTER):

            p_snake->difficulty = difficulty_type;

            return;    //난이도 설정 후 빠져나오는 루트

        }

    }

}

void draw_difficulty_selection_menu(COORD start_pos) {

    goto_xy(start_pos.X, start_pos.Y);

    printf("EASY");

    goto_xy(start_pos.X + 10, start_pos.Y);

    printf("NORMAL");

    goto_xy(start_pos.X + 22, start_pos.Y);

    printf("HARD");

}

void display_selected_difficulty(difficulty difficulty_type, COORD start_pos) {

    switch (difficulty_type) {

    case(EASY):

        goto_xy(start_pos.X - 3, start_pos.Y);

        printf("▷");

        goto_xy(start_pos.X + 7, start_pos.Y);

        printf("　");

        goto_xy(start_pos.X + 19, start_pos.Y);

        printf("　");

        break;

    case(NORMAL):

        goto_xy(start_pos.X - 3, start_pos.Y);

        printf("　");

        goto_xy(start_pos.X + 7, start_pos.Y);

        printf("▷");

        goto_xy(start_pos.X + 19, start_pos.Y);

        printf("　");

        break;

    case(HARD):

        goto_xy(start_pos.X - 3, start_pos.Y);

        printf("　");

        goto_xy(start_pos.X + 7, start_pos.Y);

        printf("　");

        goto_xy(start_pos.X + 19, start_pos.Y);

        printf("▷");

        break;

    }

}

void game_start(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    cursor_view(0);

    srand((unsigned int)time(NULL));

    set_board_border(board);

    create_new_snake(p_snake);

    set_snake(p_snake, board);

    draw_game_board(board);

    show_info(p_snake);

    create_food(board);

}

void game_over(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    save_top_score(p_snake);

    delete_snake(p_snake);

    decide_retry(p_snake, board);

}

void save_top_score(const P_SNAKE p_snake) {

    FILE* fp = NULL;

    int score_array[DIFFICULTY_COUNT];    //EASY ~ HARD 까지 점수 저장

 

        /* 읽기 모드로 파일 열기 */

 

    /* 파일 열기 성공 시 */

    if (open_score_file(&fp, "rt")) {

        store_score_in_array(fp, score_array);    //각각의 점수를 배열에 저장함

        close_score_file(&fp);    //파일 닫음

    }

    /* 파일 열기 실패 시 */

    else {

            /* 쓰기 모드로 파일 만들기 */

 

        /* 파일 만들기 성공 시 */

        if (open_score_file(&fp, "wt")) store_score_in_array(fp, score_array);

        /* 파일 만들기 실패 시 */

        else {

            move_cursor_based_on_bottomleft_of_board(48, 5);

            printf("점수 저장에 실패했습니다.");

            return;    //함수 종료

        }

    }

    compare_scores(p_snake, score_array);    /* 난이도에 따른 현재 점수와 비교 */

    store_score_to_file(fp, score_array);    /* 파일에 점수 저장 */

}

bool open_score_file(FILE** file_pointer, const char* _Mode) {

    *file_pointer = fopen(".\\top score.dat", _Mode);

    if (*file_pointer != NULL) return true;

    else return false;

}

void close_score_file(FILE** file_pointer) {

    if (*file_pointer != NULL) fclose(*file_pointer);

}

void compare_scores(const P_SNAKE p_snake, int score_array[]) {

    if (p_snake->length > score_array[p_snake->difficulty]) {    //최고 기록일 때

        score_array[p_snake->difficulty] = p_snake->length;

        move_cursor_based_on_bottomleft_of_board(49, 2);

        printf("☆ 최고기록갱신!! ★");

    }

}

void store_score_in_array(FILE* fpr, int score_array[]) {

    difficulty difficulty;

    for (difficulty = EASY;difficulty <= HARD;difficulty++) {

        /* 만약 데이터를 가져오는데 오류가 생기면 모든 점수를 0으로 배열에 저장하고 가져오기를 중단함 */

        if (fscanf(fpr, "%d", &score_array[difficulty]) == EOF) {

            for (difficulty = EASY;difficulty <= HARD;difficulty++) {

                score_array[difficulty] = 0;

            }

            return;

        }

    }

    score_encryption_and_decryption(score_array);    //복호화

}

void store_score_to_file(FILE* fpw, int score_array[]) {

    difficulty difficulty;

    score_encryption_and_decryption(score_array);    //암호화

    open_score_file(&fpw, "wt");    //쓰기 모드로 파일 열기

    for (difficulty = EASY;difficulty <= HARD;difficulty++) {

        fprintf(fpw, "%d\n", score_array[difficulty]);

    }

    close_score_file(&fpw);

}

void score_encryption_and_decryption(int score_array[]) {

    difficulty difficulty;

    int encryption_key_value[DIFFICULTY_COUNT] = { 0X1234ABCD, 0X2345BCDE, 0X3456CDEF };

    for (difficulty = EASY;difficulty <= HARD;difficulty++) {

        score_array[difficulty] ^= encryption_key_value[difficulty];

    }

}

void decide_retry(P_SNAKE p_snake, block_t(*board)[BOARD_WIDTH]) {

    bool retry = true;

    move_cursor_based_on_bottomleft_of_board(2, 2);

    printf("·Retry  ·Exit");

    move_cursor_based_on_bottomleft_of_board(2, 3);

    printf("  ────         ");

    while (true) {

        switch (get_key()) {

        case(LEFT):

            move_cursor_based_on_bottomleft_of_board(2, 3);

            printf("  ────         ");

            retry = true;

            break;

        case(RIGHT):

            move_cursor_based_on_bottomleft_of_board(2, 3);

            printf("           ─── ");

            retry = false;

            break;

        case(ENTER):

            if (retry) {

                system("cls");

                game_start(p_snake, board);

                return;

            }

            else {

                free(p_snake);

                exit(0);

            }

        }

    }

}

/* 정보 출력 관련 함수 정의 */

void show_info(const P_SNAKE p_snake) {

    show_difficulty(p_snake);

    draw_snake_info_border();

    show_level(p_snake);

    show_length(p_snake);

    show_power_count(p_snake);

    COORD std_pos = { 4, 8 };

    show_move_key_info(std_pos);

    show_pause_key_info(std_pos);

    show_power_key_info(std_pos);

    show_recommended_font_size(std_pos);

    show_top_score(p_snake, std_pos);

}

void show_difficulty(const P_SNAKE p_snake) {

    move_cursor_based_on_topright_of_board(4, 0);

    switch (p_snake->difficulty) {

    case(EASY):

        printf("%16s", "EASY MODE");

        break;

    case(NORMAL):

        printf("%16s", "NORMAL MODE");

        break;

    case(HARD):

        printf("%16s", "HARD MODE");

        break;

    }

}

void draw_snake_info_border() {

    int i;

    move_cursor_based_on_topright_of_board(4, 1);

    printf("┌───────────────┐");

    for (i = 2;i <= 6;i++) {

        move_cursor_based_on_topright_of_board(4, i);

        printf("│               │");

    }

    move_cursor_based_on_topright_of_board(4, 7);

    printf("└───────────────┘");

}

void show_level(const P_SNAKE p_snake) {

    move_cursor_based_on_topright_of_board(6, 2);

    printf("현재 레벨 : %d", p_snake->level);

 

}

void show_length(const P_SNAKE p_snake) {

    move_cursor_based_on_topright_of_board(6, 4);

    printf("현재 길이 : %d", p_snake->length);

}

void show_power_count(const P_SNAKE p_snake) {

    int i;

    move_cursor_based_on_topright_of_board(6, 6);

    printf("[");

    move_cursor_based_on_topright_of_board(19, 6);

    printf("]");

    move_cursor_based_on_topright_of_board(8, 6);

    printf("☆☆☆☆☆");

    move_cursor_based_on_topright_of_board(8, 6);

    for (i = 0;i < p_snake->power_count;i++) {

        printf("★");

    }

}

void show_move_key_info(COORD pos) {

    move_cursor_based_on_topright_of_board(pos.X + 12, pos.Y);

    printf("△");

    move_cursor_based_on_topright_of_board(pos.X, pos.Y + 1);

    printf("※ 이동 : ◁  ▷");

    move_cursor_based_on_topright_of_board(pos.X + 12, pos.Y + 2);

    printf("▽");

}

void show_pause_key_info(COORD pos) {

    move_cursor_based_on_topright_of_board(pos.X, pos.Y + 4);

    printf("※ 일시정지 : Space bar");

}

void show_power_key_info(COORD pos) {

    move_cursor_based_on_topright_of_board(pos.X, pos.Y + 6);

    printf("※ 파워 사용 : X");

}

void show_recommended_font_size(COORD pos) {

    move_cursor_based_on_topright_of_board(pos.X, pos.Y + 8);

    printf("※ 권장 글꼴 크기 : 16pt");

}

void show_top_score(const P_SNAKE p_snake, COORD pos) {

    FILE* fpr = NULL;

    int score_array[3];

 

    move_cursor_based_on_topright_of_board(pos.X, pos.Y + 10);

    if (open_score_file(&fpr, "rt")) {

        store_score_in_array(fpr, score_array);

        switch (p_snake->difficulty) {

        case(EASY):

            printf("%6s 최고점수 : %d", "EASY", score_array[EASY]);

            break;

        case(NORMAL):

            printf("%6s 최고점수 : %d", "NORMAL", score_array[NORMAL]);

            break;

        case(HARD):

            printf("%6s 최고점수 : %d", "HARD", score_array[HARD]);

            break;

        }

        close_score_file(&fpr);

    }

    else printf("최고점수 가져오기 실패");

}

/* 커서 관련 함수 정의 */

void move_cursor_based_on_topright_of_board(SHORT x_coord_fluctuation, SHORT y_coord_fluctuation) {

    goto_xy(CONVERT_X(BOARD_WIDTH) - 2 + x_coord_fluctuation, BOARD_START_POS_Y + y_coord_fluctuation);

}

void move_cursor_based_on_bottomleft_of_board(SHORT x_coord_fluctuation, SHORT y_coord_fluctuation) {

    goto_xy(BOARD_START_POS_X + x_coord_fluctuation, BOARD_START_POS_Y + BOARD_HEIGHT - 1 + y_coord_fluctuation);

}

void goto_xy(SHORT pos_x, SHORT pos_y) {

    COORD Pos;

    Pos.X = pos_x;

    Pos.Y = pos_y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);

}

void cursor_view(char s) {

    HANDLE hConsole;

    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = s;

    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole, &ConsoleCursor);

}
