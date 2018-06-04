#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define MAP_WIDTH_SIZE 7											// Connect 4 게임의 가로 길이는 7칸
#define MAP_HEIGHT_SIZE 6											// Connect 4 게임의 세로 길이는 6칸
#define MAX_GAME_LENGTH 42											// Connect 4 에서 최대 게임의 길이는 42회(6*7)

int map[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];					// Coonect 4 는 map은 가로 7칸 세로 6칸의 7 by 6 짜리 게임, 현재 턴에서 게임의 진행 상태를 나타내는 배열
int HEIGHT[MAP_WIDTH_SIZE + 1];										// 각 열마다 몇개의 돌이 놓여져 있는지 저장해 놓은 배열
int GAME_STATE[MAX_GAME_LENGTH + 1];								// 각 턴마다 어떤 위치에 돌을 두었는지 저장해 놓은 배열


int map_temp[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];				// 임시로 사용할 map
int HEIGHT_TEMP[MAP_WIDTH_SIZE + 1];								// 임시로 사용할 HEIGHT

int a, b, c, d;														// Rule base 에서 사용할 check 인자


int STONE_COLOR = 1;												// 현재 어떤 색이 착수할 차례인지 (1==○ 2==●)
																	// !**선/후공 나누는 구현 필요**!

int game_length;													// 몇개의 착수가 이뤄졌는지
int game_length_temp;												// 임시로 사용할 game_length_temp


void print_intro();													// 최초 실행 시 나타나는 화면
void select_first_player();											// 선공 결정
int select_play_method();											// 1.search algorithm	2.rule base		3.직접 입력

int compute_by_search_algorithm();									// search algorithm 을 통한 착수점 계산
int compute_by_rule_base();											// rule base 로 착수점 계산

void push_stone(char input_stone_location);							// 착수점에 착수

void print_current_map();											// 현재까지 진행된 게임의 state

void play_connect4();												// 게임 플레이

int evaluate_board_state();

bool check_possible_position(int stone_location);					// 해당 지점에 돌을 놓을 수 있는지?
bool is_winning(int column);										// 지금 상태에서 column에 뒀을 때 이기는가?
bool is_winning_temp(int column);									// 지금 상태에서 column에 뒀을 때 이기는가? 임시 코드



int heuristic_value[3][3][3][3][3][3][3];

int minmax(int depth);
int minmax_alphabeta_pruning(int depth, int alpha, int beta);

int check_connect3(int count, int color);
int check_nconnect3(int count, int color);

int find_another_column(int count);

int main() {

	print_intro();
	// 선/후공 결정 구현 필요 ?
	// 현재 무조건 흑이 선공
	//	select_first_player();
	play_connect4();

	return 0;
}
void print_intro() {																// 최초 실행 시 나타나는 안내문구
	printf("**************************************************************************\n");
	printf("********************************Connect 4*********************************\n");
	printf("**************************************************************************\n");
	printf("***************Copyright by 아버지날보고있다면정답을알려조****************\n");
	printf("**************************************************************************\n");
	printf("****************************************************2018.05.03************\n");
	printf("**************************************************************************\n");

}

void play_connect4() {											// 게임을 실행한다
	int stone;
	do {
		stone = select_play_method();							// 어디에 돌을 두었는지 받아온다. play_method는 1. Search, 2.Rule, 3.직접입력 이 있다.
		print_current_map();									// 선택한 방법으로 돌을 두었을 때 현재의 게임 진행 맵
		printf("착수된 지점 : %d\n", stone);						// 어디에 뒀는지 확인
													
		if (is_winning(stone)) {								// 지금 돈 둘이 승패를 결정했는가?
			if (game_length % 2) printf("선공의 승리\n");		// 먼저 둔 사람이 이긴 상황
			else printf("후공의 승리\n");						// 나중에 둔 사람이 이긴 상황
			break;
		}
		if (game_length == MAX_GAME_LENGTH) {					// game 의 승패가 나지 않았다.
			printf("무승부\n");
			break;
		}
	} while (true);
	printf("게임 종료\n");
	getchar();
	printf("종료하려면 아무키나 누르세요.\n");
	getchar();
}


int select_play_method() {															// 작수 방식 결정
	printf("\n");

	bool is_select_finished = false;												// 돌을 성공적으로 놓을 때까지 while 문은 계속 돌아야 하므로 이를 판단해주는 boolean 변수
	int stone;

	clock_t begin, end;

	do {
		printf("착수 방식을 선택해주세요. (1. Search Algorithm, 2. Rule Base, 3. 직접 입력) : ");

		char input_play_method;
		scanf("%c",&input_play_method);												// 착수 방식 입력받음
//		getchar(); // ?
		begin = clock();															// 돌 놓기까지 결정하는데 걸리는 시간 측정 시작

		if (input_play_method == '1') {												// 1. Search 로 돌을 둘 때
			printf("Search Algorithm에 기반하여 착수합니다.\n");

			stone = compute_by_search_algorithm();									// compute_by_search_algorithm 함수를 이용해서 어디 위치에 돌을 둘 것인지 알아온다.
			//cout << "계산된 착수점 : " << stone << endl;
			push_stone(stone + '0');												// 알아온 곳의 위치에 돌을 둔다

			is_select_finished = true;												// 돌을 두었으니 while문을 빠져나오기 위해 true로 바꿔준다.
		}
		else if (input_play_method == '2') {										// 2. Rule 로 돌을 둘 때
			printf("Rule Base로 착수합니다.\n");

			stone = compute_by_rule_base();													// compute_by_rule_base 함수를 이용해서 어디 위치에 돌을 둘 것인지 알아온다.
			push_stone(stone + '0');
			is_select_finished = true;												// 아직 rule base 로 두는 함수 개발 안됨
		}
		else if (input_play_method == '3') {
			printf("착수할 지점의 번호(1~7)를 입력해주세요.\n");
			getchar();
			char input_stone_location;
			do {
				printf("착수 지점 : ");
				scanf("%c",&input_stone_location);										// 사용자가 돌을 두기로 직접 입력한 위치
				getchar();
				if (input_stone_location >= '1' && input_stone_location <= '7') {	// 1~7 범위로 정확하게 입력했을 때
					stone = input_stone_location - '0';
					is_select_finished = true;
				}
				else {																// 1~7 이외의 입력을 받았으면 이는 잘못된 입력
					printf("착수 지점을 잘못 입력하였습니다. 다시 입력해주세요.\n");

				}
			} while (!is_select_finished);
			bool push_complete = check_possible_position(input_stone_location - '0');			// 입력 받은 열에 돌을 놓을 수 있는지 확인해준다.
			if (!push_complete) {																// 입력 받은 열에 돌을 놓을 수 없다.
				printf("해당 지점은 돌이 가득 차 있습니다.다시 입력해주세요.\n");
				is_select_finished = false;
			}
			else {																				// 입력 받은 열에 돌을 둘 수 있다.
				push_stone(input_stone_location);
			}
		}
		else {																		// 1.Search, 2.Rule, 3.직접입력 외의 다른 입력으로 인한 잘못된 입력.
			printf("잘못된 입력입니다. 다시 입력해주세요.\n");
		}

		end = clock();																// 돌을 놓는데 걸린 시간 측정 종료

		printf("수행시간 : %d초\n",((end - begin) / CLOCKS_PER_SEC));			// 돌을 하나 놓는데 걸린 시간을 초단위로 출력
	} while (!is_select_finished);
	return stone;
}

int compute_by_search_algorithm() {							// Search Algorithm 에 의해 계산된 지점에 착수
	getchar();
	int best = -1000000, best_move = 0, value;
	if (game_length == 0) return 3;							// 처음에 4번 열에 둘 수 없어서 그 다음으로 좋은 값인 3번 열에 둔다.
	if (game_length % 2 == 1) best = 1000000;
	for (int i = 1; i <= MAP_WIDTH_SIZE; i++) {				// 해당 턴에서 둘 수 있는 위치 탐색
		if (HEIGHT[i] < MAP_HEIGHT_SIZE) {					// 해당 칸의 높이가 충분하여 한 수 둘 수 있을 때
			HEIGHT[i]++;									// 뒀다고 가정, 밑에서 -- 해줄 것임.
			map[HEIGHT[i]][i] = 1 + game_length % 2;		// 뒀다고 가정, 1은 선공 2는 후공/ game_length는 0부터 시작함
			game_length++;									// 뒀다고 가정
			if (is_winning(i)) {							// 알고보니 이기는 수였다?
				game_length--;								// 실제로 돌을 넣는 함수는 push_stone이므로 잠시 넣었던 상태를 해제
				map[HEIGHT[i]][i] = 0;
				HEIGHT[i]--;
				return i;									// 이기는 수에 착수하도록 값 반환
			}
			game_length--;

			memcpy(map_temp, map, sizeof(map));				// 임시배열을 이용한 depth 탐색
			memcpy(HEIGHT_TEMP, HEIGHT, sizeof(HEIGHT));

			game_length_temp = game_length + 1;				// 한 수 뒀으니까 +1
															//			value = minmax(1);									// 한 수 앞을 보러 간다. minmax 이용
			value = minmax_alphabeta_pruning(1, -1000000, 1000000); // 한 수 앞을 보러 간다. minmax with alphabeta pruning

			if (game_length % 2 == 0) {							// 선공의 점수 계산은 max
				if (best <= value) {
					if (best == value) {						// 같은 값이 best로 측정된다면
						if (abs(4 - i) < abs(4 - best_move)) best_move = i;	// 가운데에 가까운 애를 선택할 수 있도록
					}
					else {
						best = value;
						best_move = i;
					}
				}
//				cout << "game_length : " << game_length + 1 << " score : " << value << " which? : " << i << endl;	// 디버깅용
				printf("game_length : %d score : %d which? : %d\n",game_length+1,value,i);
			}
			else {												// 후공의 점수 계산은 min
				if (best >= value) {
					if (best == value) {						// 같은 값이 best로 측정된다면
						if (abs(4 - i) < abs(4 - best_move)) best_move = i;	// 가운데에 가까운 애를 선택할 수 있도록
					}
					else {
						best = value;
						best_move = i;
					}
				}
//				cout << "game_length : " << game_length + 1 << " score : " << value << " which? : " << i << endl;	// 디버깅용
				printf("game_length : %d score : %d which? : %d\n",game_length+1,value,i);
			}
			map[HEIGHT[i]][i] = 0;							// 다음 수를 탐색하는데 영향을 미치지 않기 위해 두기 이전 상태로 돌려놓음
			HEIGHT[i]--;
		}
	}
	return best_move;										// 가장 좋은 수로 계산된 위치를 반환
}


int check_connect3(int count, int color) {// 1-> color 2->opponent_color
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 8; j++) {
			//가로로 3개 연속해서 있을 때
			//돌 3개가 맨 아랫줄에 가로로 연속으로 있을때
			if (i == 1 && j < 5) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j + 3] == 0) {
					return j + 3;
				}
			}
			if (j > 1 && j < 6 && i == 1) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j - 1] == 0) {
					return j - 1;
				}
			}
			//맨 아랫줄을 제외한 줄에 3개가 가로로 연속으로 있을때
			if (i>1 && j < 5) {				// 좌측에 둔다
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j + 3] == 0 &&
					(map[i - 1][j + 3] == 1 || map[i - 1][j + 3] == 2)) {
					return j + 3;
				}
			}
			if (i > 1 && j > 1 && j < 6) {	// 우측에 둔다
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j - 1] == 0 &&
					(map[i - 1][j - 1] == 1 || map[i - 1][j - 1] == 2)) {
					return j - 1;
				}
			}
			// OOO 처리 끝

			//세로로 3개가 연속으로 있을 때
			if (i < 4) {
				if (map[i][j] == color && map[i + 1][j] == color && map[i + 2][j] == color && map[i + 3][j] == 0 && i < 4 && i>0) {
					return j;
				}
			}
			// 세로로 3개 연속 처리 끝

			//대각선으로 3개 있을때 case 1(우 하향)
			if (i > 2 && i < 6 && j>1 && j < 6) {		// 좌측 위쪽에 둬서 대각선4개를 만든다
				if (map[i][j] == color && map[i - 1][j + 1] == color && map[i - 2][j + 2] == color) {
					if (map[i + 1][j - 1] == 0 && (map[i][j - 1] == 1 || map[i][j - 1] == 2)) {
						return j - 1;
					}
				}
			}
			if (i > 3 && j < 5) {						// 우측 아래쪽에 둬서 대각선4개를 만든다.
				if (map[i][j] == color && map[i - 1][j + 1] == color && map[i - 2][j + 2] == color) {
					if (map[i - 3][j + 3] == 0 && (map[i - 4][j + 3] == 1 || map[i - 4][j + 3] == 2 || i == 4)) {
						return j + 3;
					}
				}
			}
			//대각선으로 3개 연속 case 1 처리 


			//대각선으로 3개 있을때 case 2 (좌 하향)		// 좌측 아래쪽에 둬서 대각선 4개를 만든다
			if (i > 3 && j > 3) {
				if (map[i][j] == color && map[i - 1][j - 1] == color && map[i - 2][j - 2] == color) {
					if (map[i - 3][j - 3] == 0 && (map[i - 4][j - 3] == 1 || map[i - 4][j - 3] == 2 || i == 4)) {
						return j - 3;
					}
				}
			}
			if (i > 2 && i < 6 && j>2 && j < 7) {		// 우측 위쪽에 둬서 대각선 4개를 만든다
				if (map[i][j] == color && map[i - 1][j - 1] == color && map[i - 2][j - 2] == color) {
					if (map[i + 1][j + 1] == 0 && (map[i][j + 1] == 1 || map[i][j + 1] == 2)) {
						return j + 1;
					}
				}
			}//대각선으로 3개 연속 case 2 처리 끝 
		}
	}//for문 끝
	return check_nconnect3(count, color);
}

//연결되지 않지만 한수만 더 두면 끝나는 수가 존재할 때
int check_nconnect3(int count, int color) {
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 8; j++) {

			// oo o 일때, 맨 아랫줄이 아닐 때
			if (i > 1 && j<5) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 3] == color && map[i][j + 2] == 0 &&
					(map[i - 1][j + 2] == 1 || map[i - 1][j + 2] == 2)) {
					return j + 2;
				}
			}
			//맨 아랫줄일때
			if (i == 1 && j<5) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 3] == color && map[i][j + 2] == 0) {
					return j + 2;
				}
			}

			//o oo 일때, 맨 아랫줄이 아닐 때
			if (i > 1 && j < 5) {
				if (map[i][j] == color && map[i][j + 2] == color && map[i][j + 3] == color && map[i][j + 1] == 0 &&
					(map[i - 1][j + 1] == 1 || map[i - 1][j + 1] == 2)) {
					return j + 1;
				}
			}
			//맨 아랫줄일때
			if (i == 1 && j < 5) {
				if (map[i][j] == color && map[i][j + 2] == color && map[i][j + 3] == color && map[i][j + 1] == 0) {
					return j + 1;
				}
			}
			//   o
			//  
			// o
			//o
			if (i < 4 && j < 5) {
				if (map[i][j] == color && map[i + 1][j + 1] == color && map[i + 3][j + 3] == color && map[i + 2][j + 2] == 0 &&
					(map[i + 1][j + 2] == 1 || map[i + 1][j + 2] == 2)) {
					return j + 2;
				}
			}
			//   o
			//  o
			// 
			//o
			if (i < 4 && j < 5) {
				if (map[i][j] == color && map[i + 2][j + 2] == color && map[i + 3][j + 3] == color && map[i + 1][j + 1] == 0 &&
					(map[i][j + 1] == 1 || map[i][j + 1] == 2)) {
					return j + 1;
				}
			}
			//o
			// o 
			//  
			//   o
			if (i > 3 && j < 5) {
				if (map[i][j] == color && map[i - 1][j + 1] == color && map[i - 3][j + 3] == color && map[i - 2][j + 2] == 0 &&
					(map[i - 3][j + 2] == 1 || map[i - 3][j + 2] == 2)) {
					return j + 2;
				}
			}
			//o
			//  
			//  o
			//   o
			if (i > 3 && j < 5) {
				if (map[i][j] == color && map[i - 2][j + 2] == color && map[i - 3][j + 3] == color && map[i - 1][j + 1] == 0 &&
					(map[i - 2][j + 1] == 1 || map[i - 2][j + 1] == 2)) {
					return j + 1;
				}
			}

		}
	}//for문 끝
	return 0;
	//Check_connect3(count);
}

int find_another_column(int count) {
	for (int i = 1; i <= MAP_HEIGHT_SIZE; i++) {
		if (map[i][4] == 0 && (map[i - 1][4] == 1 || map[i - 1][4] == 2 || i == 1))
		{
			return 4;
		}
		if (map[i][5] == 0 && (map[i - 1][5] == 1 || map[i - 1][5] == 2 || i == 1))
		{
			return 5;
		}
		if (map[i][3] == 0 && (map[i - 1][3] == 1 || map[i - 1][3] == 2 || i == 1))
		{
			return 3;
		}
		if (map[i][6] == 0 && (map[i - 1][6] == 1 || map[i - 1][6] == 2 || i == 1))
		{
			return 6;
		}
		if (map[i][2] == 0 && (map[i - 1][2] == 1 || map[i - 1][2] == 2 || i == 1))
		{
			return 2;
		}
		if (map[i][7] == 0 && (map[i - 1][7] == 1 || map[i - 1][7] == 2 || i == 1))
		{
			return 7;
		}
		if (map[i][1] == 0 && (map[i - 1][1] == 1 || map[i - 1][1] == 2 || i == 1))
		{
			return 1;
		}
	}
	return 0;
}

int compute_by_rule_base() {									// Rule Base 에 의해 계산된 지점에 착수
	getchar();

	/*
	created by Seonyoung Kim
	*/
	if (game_length == 0) return 3;
	int count = game_length + 1;
	int color = 1 + (game_length) % 2;
	int opponent_color = 1 + (game_length + 1) % 2;				// 적성 색 판단
	int ret;


	ret = check_connect3(count, color);							// 룰 기반으로 했을 때 다음수에 4개를 만들 수 있는지 판단
	if (ret == 0) {												// 4개를 만들 수 없으니 상대방이 4개를 만들 수 있는지 판단
		ret = check_connect3(count, opponent_color);
		if (ret != 0) return ret;								// 상대방이 4개를 만들 수 있다면 막는다
	}
	else return ret;											// 4개를 만들 수 있다면 둔다

																/*
																배열 index 변경중...
																*/
	for (int j = 2; j < 6; j++) {
		if (map[1][j] == opponent_color && map[1][j + 1] == opponent_color && map[1][j - 1] == 0 && map[1][j + 2] == 0) {
			return j - 1;
		}
	}


	for (int j = 3; j < 6; j++) {
		if (map[1][j - 1] == opponent_color && map[1][j + 1] == opponent_color && map[1][j - 2] == 0 && map[1][j] == 0 && map[1][j + 2] == 0) {
			return j;
		}
	}

	if (map[1][4] == opponent_color && map[1][5] == opponent_color && a == 0) {
		if (map[1][3] == 0) {
			a++;
			return 3;
		}
		else if (map[1][6] == 0) {
			a++;
			return 6;
		}
	}

	else if (map[1][3] == opponent_color && map[1][4] == opponent_color && b == 0) {
		if (map[1][2] == 0) {
			b++;
			return 2;
		}
		else if (map[1][5] == 0) {
			b++;
			return 5;
		}
	}

	else if (map[1][2] == opponent_color && map[1][4] == opponent_color && map[1][5] == opponent_color && c == 0) {
		if (map[1][3] == 0) {
			c++;
			return 3;
		}
	}

	else if (map[1][3] == opponent_color && map[1][4] == opponent_color && map[1][6] == opponent_color && d == 0) {
		if (map[1][5] == 0) {
			d++;
			return 5;
		}
	}
	if (HEIGHT[GAME_STATE[count - 1]] < MAP_HEIGHT_SIZE) {				// 상대방이 방금 전에 둔 돌 위에 두는 룰
		return GAME_STATE[count - 1];
	}
	else {																// 위에 둘 수가 없을 때 다른 곳을 찾아서 둔다.
		return find_another_column(count);
	}

}

bool check_possible_position(int stone_location) {				// 착수 가능한 지점인지 판별(=해당 열에 이미 돌이 6개가 들어가 있는지 아닌지 판별)
	int i;
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			if (i == MAP_HEIGHT_SIZE) return false;
			return true;
		}
	}
	return true;
}
void push_stone(char input_stone_location) {					// 계산된 위치에 착수

	int i, stone_location = input_stone_location - '0';
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			break;
		}
	}
	map[i + 1][stone_location] = STONE_COLOR;
	HEIGHT[stone_location]++;
	game_length++;
	GAME_STATE[game_length] = stone_location;	// 몇 회째에 어떤 칸에 두었는지 저장
	if (STONE_COLOR == 1) {						// 흑● 일 때 백○ 의 차례로 바꾸어준다.
		STONE_COLOR = 2;
	}
	else {										// 백○ 일 때 흑● 의 차례로 바꾸어준다.
		STONE_COLOR = 1;
	}
}

void print_current_map() {										// 현재까지 게임의 진행 상태를 표현
	int i, j;

	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map[i][j] == 0) {				// 빈칸
				printf(" . ");
			}
			else if (map[i][j] == 1) {			// 흑○
				printf("○ ");
			}
			else if (map[i][j] == 2) {			// 백●
				printf("● ");
			}
		}
		printf("\n");
	}
}

bool is_winning_temp(int column) {								// 임시 map 의 상황이 이기는 상황이 되는지 판단해주는 함수
	int color = 1 + (game_length_temp + 1) % 2;
	int dx, dy, cnt = 0;

	if (HEIGHT_TEMP[column] >= 3													// 세로로 만들어진 경우
		&& map_temp[HEIGHT_TEMP[column] - 1][column] == color
		&& map_temp[HEIGHT_TEMP[column] - 2][column] == color
		&& map_temp[HEIGHT_TEMP[column] - 3][column] == color) return true;
	for (dy = -1; dy <= 1; dy++) {												// 대각선 및 가로로 만들어진 경우
		cnt = 0;
		for (dx = -1; dx <= 1; dx += 2) {
			for (int x = column + dx, y = HEIGHT_TEMP[column] + dx * dy; x >= 1 && x <= MAP_WIDTH_SIZE && y >= 1 && y <= MAP_HEIGHT_SIZE && map_temp[y][x] == color;) {
				x += dx;
				y += dx * dy;
				cnt++;
			}
		}
		if (cnt >= 3) return true;
	}
	return false;
}

bool is_winning(int column) {									// map의 상황이 이기는 상황이 되는지 판단해주는 함수
	int color = 1 + (game_length + 1) % 2;
	int dx, dy, cnt = 0;

	if (HEIGHT[column] >= 3													// 세로로 만들어진 경우
		&& map[HEIGHT[column] - 1][column] == color
		&& map[HEIGHT[column] - 2][column] == color
		&& map[HEIGHT[column] - 3][column] == color) return true;
	for (dy = -1; dy <= 1; dy++) {												// 대각선 및 가로로 만들어진 경우
		cnt = 0;
		for (dx = -1; dx <= 1; dx += 2) {
			for (int x = column + dx, y = HEIGHT[column] + dx * dy; x >= 1 && x <= MAP_WIDTH_SIZE && y >= 1 && y <= MAP_HEIGHT_SIZE && map[y][x] == color;) {
				x += dx;
				y += dx * dy;
				cnt++;
			}
		}
		if (cnt >= 3) return true;
	}
	return false;
}

int minmax_alphabeta_pruning(int depth, int alpha1, int beta1) {				// minmax with alphabeta pruning
	int value = 0;
	if (MAX_GAME_LENGTH - game_length_temp <= 0) return evaluate_board_state();
	if (depth >= 12) {														// 12수 앞까지 내다보거나 12수 이하 게임의 끝까지 내다보거나
		if (game_length_temp > 12) {											// 게임의 길이가 길어지면
			if (depth == 12 + (game_length_temp - 13) / 5) {					// 수를 더 내다본다
				return evaluate_board_state();
			}
		}
		else {
			return evaluate_board_state();
		}
	}
	int i;
	int best;
	if ((game_length + depth) % 2 == 0) {				// 선공 max값을 리턴
		best = -1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// 해당 열에 둘 수 있다면
				HEIGHT_TEMP[i]++;						// 하나를 둬본다
				map_temp[HEIGHT_TEMP[i]][i] = 1;
				game_length_temp++;
				if (is_winning_temp(i)) {				// 근데 여기에 두면 이긴다?
					game_length_temp--;						// 임시배열 원복 <- 실제 돌은 push_stone 에서 넣어주게 되고, 향후 다른 노드에서 결과값에 지장을 주지 않기 위해 임시배열은 넣기 전 상태로 돌려주어야한다.
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return (21 - (game_length_temp + 1) / 2) * 10000;		// 얼만큼의 횟수를 아껴서 게임에서 승리하는지, 총 게임의 길이는 42회 이고, 각 플레이어는 21회씩 게임을 플레이 할 수 있다.
				}

				value = minmax_alphabeta_pruning(depth + 1, alpha1, beta1);	// 다음 수 탐색
				if (best <= value) {					// 다음 수에서 가져온 값이 더 좋다면
					best = value;
				}
				game_length_temp--;						// 다음 수에 지장을 주지 않기 위해 임시배열을 넣기 전 상태로 돌려준다.
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
				if (alpha1 < best) {					// alpha 범위 줄여가는 중
					alpha1 = best;
					if (beta1 <= alpha1) break;			// alpha cut
				}
			}
		}
	}
	else {												// 후공 min값을 리턴
		best = 1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// 해당 열에 둘 수 있다면
				HEIGHT_TEMP[i]++;						// 둬보는데
				map_temp[HEIGHT_TEMP[i]][i] = 2;
				game_length_temp++;
				if (is_winning_temp(i)) {				// 근데 여기에 두면 이긴다?
					game_length_temp--;							// 임시배열 원복
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return -((21 - (game_length_temp + 1) / 2) * 10000);		// 얼만큼의 횟수를 아껴서 게임에서 승리하는지, 후공의 값이므로 음수(-)로 반환(?)
				}
				value = minmax_alphabeta_pruning(depth + 1, alpha1, beta1);		// 다음 수 탐색
				if (best >= value) {
					best = value;
				}
				game_length_temp--;
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
				if (beta1 > best) {						// beta 범위 줄여가는 중
					beta1 = best;
					if (beta1 <= alpha1) break;			// beta cut
				}
			}
		}
	}
	return best;
}

int minmax(int depth) {													// minmax
	int value = 0;
	if (depth >= 8 || MAX_GAME_LENGTH - game_length_temp <= 0) {
		if (game_length_temp > 13) {
			if (depth == 8 + (game_length_temp - 13) / 8) {				// 시간이 지날 수록 depth 탐색 강화
				return evaluate_board_state();							// 현재 보드의 상태 점수를 반환해준다. >0 일 경우 선공이 유리 <0 일 경우 후공이 유리한 상황(black-white이므로)
			}
		}
		return evaluate_board_state();
	}
	int i;
	int best = -1000000;
	if ((game_length + depth) % 2) best = 1000000;
	if ((game_length + depth) % 2 == 0) {				// 선공 max값을 리턴
		best = -1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {			// 둘 수 있는 칸을 다 돌아보는데
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// 해당 칸에 높이가 충분해서 넣을 수 있다면

				HEIGHT_TEMP[i]++;						// 임시배열을 통해 앞 수를 내다보는 과정
				map_temp[HEIGHT_TEMP[i]][i] = 1;
				game_length_temp++;
				if (is_winning_temp(i)) {				// 근데 여기에 두면 이긴다?
					game_length_temp--;						// 임시배열 원복
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return (21 - (game_length_temp + 1) / 2) * 10000;		// 얼만큼의 횟수를 아껴서 게임에서 승리하는지
				}
				value = minmax(depth + 1);				// 한 수 앞을 더 보러간다
				if (best <= value) {					// 앞 서 본 수들의 값이 다른 위치에 둔 것 보다 좋은 결과를 얻는다면
					best = value;						// 값을 갱신
				}
				game_length_temp--;						// 임시배열 원복
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
			}
		}
	}
	else {												// 후공 min값을 리턴
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {			// 둘 수 있는 칸을 다 돌아보는데
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// 해당 칸에 높이가 충분해서 넣을 수 있다면
														/*				cout << "game_length : " << game_length << " game_length_temp : " << game_length_temp << " i : " << i << endl;
														print_current_map_temp();
														getchar();*/
				HEIGHT_TEMP[i]++;						// 임시배열을 이용해서 앞 수를 내다볼 수 있도록 세팅
				map_temp[HEIGHT_TEMP[i]][i] = 2;
				game_length_temp++;
				if (is_winning_temp(i)) {				// 근데 여기에 두면 이긴다?
					game_length_temp--;							// 임시배열 원복
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return -((21 - (game_length_temp + 1) / 2) * 10000);		// 얼만큼의 횟수를 아껴서 게임에서 승리하는지, 후공의 값이므로 음수(-)로 반환(?)
				}
				value = minmax(depth + 1);				// 한 수 앞 더 보러간다
				if (best >= value) {					// 앞서 본 수들의 값이 다른 위치에 둔 것보다 좋은 결과를 얻는다면(후공의 값이 좋으려면 값이 낮을 수록 좋다)
					best = value;
				}
				game_length_temp--;							// 임시배열 원복
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
			}
		}
	}
	return best;
}
int evaluate_vertically_temp(int column, int color) {		// evaluate_vertically 세로의 점수를 확인해준다
	int i, value = 0, cnt = 0;
	int v[5] = { 0,1,3,9,100 };								// heuristic value
	for (i = HEIGHT_TEMP[column]; i >= 1; i--) {
		if (map_temp[i][column] == color) cnt++;
		else break;
		if (cnt == 4) return v[4];
	}
	if (HEIGHT_TEMP[column] == MAP_HEIGHT_SIZE) return 0;	// 무의미
	return v[cnt];
}

int evaluate_horizontally_temp(int row, int column, int color) {					// 가로 점수 전부 탐색
	int value = 0;
	int i;
	int cnt = 0;
	int nb = 0;
	int y, x;
	int v[5] = { 0,1,3,9,100 };
	y = row;
	x = column;

	for (;x <= MAP_WIDTH_SIZE && nb < 4; x++) {								// 해당지점부터 4칸 가로로 계산
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != 0) break;								// 다른 색이 중간에 나오면 count 종료
		nb++;
	}
	bool left_block = false;												// 왼쪽이 막힌 상태인지
	bool right_block = false;												// 오른쪽이 막힌 상태인지
	if (cnt == 4) {															// four in a row!
		return v[4];
	}
	else if (nb<4) {															// 4칸까지 다 봤으면 막혀있는 것은 상관이 없다		
		if (column == 1) {													// 가장 좌측에 돌이 있다는 것은 왼쪽이 막혀있다는 뜻
			left_block = true;
		}
		else {
			if (map_temp[row][column - 1] != color && map_temp[row][column - 1] != 0) {	// 가장 좌측에 있지는 않지만 왼쪽 끝 돌 옆에 색이 다른 돌이 있다는 것 역시 왼쪽이 막혀 있다는 뜻
				left_block = true;
			}
		}
		if (x == MAP_WIDTH_SIZE + 1) {										// x가 가로의 끝을 넘어갔다는 것은 column>=4인 상황에서만 가능하고, 끝까지 도달한 경우 이므로 오른쪽 끝이 막혀있다.
			right_block = true;
		}
		else {
			if (map_temp[row][x] != color && map_temp[row][x] != 0) {		// x는 끝부분이므로 다른 돌로 막혀있다는 뜻
				right_block = true;
			}
		}
		if (left_block && right_block) {									// 오른쪽 왼쪽이 둘 다 막혀있으면 이것은 점수가 없는 상태
//			cnt = 0;														// 개선 필요
		}
	}
	return v[cnt];
}

int evaluate_diagonally_temp(int row, int column, int color) {				// 대각 탐색 개선(개선은 됐는데 효과가 없네)

	int v[5] = { 0,1,3,9,100 };
	int value = 0;
	int cnt = 0;
	int nb = 0;
	int y, x;

	y = row;
	x = column;

	for (;y >= 1 && x >= 1 && nb < 4; y--, x--) {							// 좌 대각선
		nb++;
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != 0) break;
	}
	bool top_block = false;
	bool bottom_block = false;
	if (cnt == 4) {
		//		value += v[cnt];												// 4칸을 만들었다.
	}
	else {
		if (y == 0) bottom_block = true;										// 밑에가 막혀있다.
		else if (nb < 4) bottom_block = true;									// 밑에가 막혀있다.
		if (row == MAP_HEIGHT_SIZE) top_block = true;							// 위에가 막혀있다.
		else {
			if (column != MAP_WIDTH_SIZE) {
				if (map_temp[row + 1][column + 1] != color && map_temp[row + 1][column + 1] != 0) {		// 위에가 막혀있다.
					top_block = true;
				}
			}
		}
		if (top_block && bottom_block) cnt = 0;
	}
	value += v[cnt];



	y = row;
	x = column;
	nb = 0;
	cnt = 0;
	top_block = bottom_block = false;
	for (;y >= 1 && x <= MAP_WIDTH_SIZE && nb < 4; y--, x++) {								// 우 대각선 최대 4칸 탐색
		nb++;
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != 0) break;
	}
	if (cnt == 4) {
		//		value += v[cnt];
	}
	else {
		if (y == 0) bottom_block = true;
		else if (nb < 4) bottom_block = true;
		if (row == MAP_HEIGHT_SIZE) top_block = true;
		else {
			if (column != 1) {
				if (map_temp[row + 1][column - 1] != color && map_temp[row + 1][column - 1] != 0) {
					top_block = true;
				}
			}
		}
		if (top_block && bottom_block) cnt = 0;
	}
	return value + v[cnt];
}

int evaluate_board_state() {					// color 를 받으면 + - 를 결정해서 이기고 지는걸 세분화 시킬 수 있을까? 아니면 호출한 부분에서 바로 결정해줄수 있는건가?
	int i, j;
	int value_black = 0;
	int value_white = 0;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (map_temp[HEIGHT_TEMP[i]][i] == 1) {
			value_black += evaluate_vertically_temp(i, 1);
		}
		else if (map_temp[HEIGHT_TEMP[i]][i] == 2) {
			value_white += evaluate_vertically_temp(i, 2);
		}
	}
	for (i = 1; i <= MAP_HEIGHT_SIZE; i++) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map_temp[i][j] == 1) {
				value_black += evaluate_horizontally_temp(i, j, 1);
			}
			else if (map_temp[i][j] == 2) {
				value_white += evaluate_horizontally_temp(i, j, 2);
			}
		}
	}

	for (i = 1; i <= MAP_HEIGHT_SIZE; i++) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map_temp[i][j] == 1) {			// 선공일 때 점수 계산
				value_black += evaluate_diagonally_temp(i, j, 1);
			}
			else if (map_temp[i][j] == 2) {
				value_white += evaluate_diagonally_temp(i, j, 2);
			}
		}
	}
	return (value_black - value_white);
}