#include <iostream>
#include <memory>

using namespace std;

#define MAP_WIDTH_SIZE 7
#define MAP_HEIGHT_SIZE 6
#define MAX_GAME_LENGTH 42

#define max2(x,y) (x>y?x:y)

int map[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];					// Coonect 4 는 map은 가로 7칸 세로 6칸의 7 by 6 짜리 게임
int HEIGHT[MAP_WIDTH_SIZE + 1];

int map_temp[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];				// map_temp
int HEIGHT_TEMP[MAP_WIDTH_SIZE + 1];


int STONE_COLOR = 1;												// 현재 어떤 색이 착수할 차례인지 (1==○ 2==●)
																	// !**선/후공 나누는 구현 필요**!

int game_length;													// 몇개의 착수가 이뤄졌는지
int game_length_temp;
int GAME_STATE[MAX_GAME_LENGTH];									// 현재까지 게임이 어떻게 진행되었는지 저장하는 배열
int GAME_STATE_TEMP[MAX_GAME_LENGTH];

int heuristic_table[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1] = {{ 3, 4, 5, 7, 5, 4, 3 },				// heuristic table
																{ 4, 6, 8, 10, 8, 6, 4 },				// 각 점수는 해당 위치에 돌이 존재할 때 승리하게 될 가중치
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 4, 6, 8, 10, 8, 6, 4 },
																{ 3, 4, 5, 2, 5, 4, 3 } };


void print_intro();													// 최초 실행 시 나타나는
void select_first_player();											// 선공 결정
int select_play_method();											// 1.search algorithm	2.rule base		3.직접 입력

int compute_by_search_algorithm();									// search algorithm 을 통한 착수점 계산
int negamax(int depth);												// search algorithm 에 사용될 negamax 함수
pair<int,int> negamax_temp(int depth);								// 평가함수
pair<int, int> negamax_temp2(int depth, int color);					// 평가함수 개선

pair<int, int> negamax_alphabeta_temp(int depth, int alpha, int beta); // 평가함수 with alphabeta prunning

int evaluate_state_temp();
int evaluate_board_state();

void compute_by_rule_base();										// rule base 로 착수점 계산

bool check_possible_position(int stone_location);					// 해당 지점에 돌을 놓을 수 있는지?
void push_stone(char input_stone_location);							// 착수점에 착수

void print_current_map();											// 현재까지 진행된 게임의 state

void play_connect4();												// 게임 플레이

bool is_winning(int column);
bool is_winning_temp(int column);									// 지금 상태에서 column에 뒀을 때 이기는가?

int main() {

	print_intro();
	// 선/후공 결정 구현 필요 ?
	// 현재 무조건 흑이 선공
//	select_first_player();
	play_connect4();

	return 0;
}



void print_intro() {																// 최초 실행 시 나타나는 안내문구
	cout << "******************************************************************\n";
	cout << "****************************Connect 4*****************************\n";
	cout << "******************************************************************\n";
	cout << "***********Copyright by 아버지날보고있다면정답을알려조************\n";
	cout << "******************************************************************\n";
	cout << "********************************************2018.04.29************\n";
	cout << "******************************************************************\n";



}

void play_connect4() {
	int stone;
	do {
		stone = select_play_method();
		print_current_map();
		cout << "착수된 지점 : " << stone << endl;
		if (is_winning(stone)) {
			if (game_length % 2) cout << "선공의 승리" << endl;	// 먼저 둔 사람이 이긴 상황
			else cout << "후공의 승리" << endl;					// 나중에 둔 사람이 이긴 상황
			break;
		}
		if (game_length == MAX_GAME_LENGTH) {					// game 의 승패가 나지 않았다.
			cout << "무승부" << endl;
			break;
		}
	} while (true);
	cout << "게임 종료";
	getchar();
}

void select_first_player() {									// 선 후공 결정
	bool is_input_correct = false;
	char input_first_player;
	do {
		cout << "선공을 결정해주세요. (1. Computer 2. 사람) : ";
		cin >> input_first_player;
		if (input_first_player == '1') {
			cout << "컴퓨터(○)의 선공입니다." << endl;
			is_input_correct = true;
		}
		else if (input_first_player == '2') {
			cout << "사람(○)의 선공입니다." << endl;
			is_input_correct = true;
		}
		else {
			cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
		}
	} while (!is_input_correct);
}

int select_play_method() {															// 작수 방식 결정
	cout << endl;

	bool is_select_finished = false;
	int stone;
	do {
		cout << "착수 방식을 선택해주세요. (1. Search Algorithm, 2. Rule Base, 3. 직접 입력)" << endl;

		char input_play_method;
		cin >> input_play_method;

		if (input_play_method == '1') {
			cout << "Search Algorithm에 기반하여 착수합니다." << endl;

			stone = compute_by_search_algorithm();
			cout << "계산된 착수점 : " << stone << endl;
			push_stone(stone+'0');

			is_select_finished = true;
		}
		else if (input_play_method == '2') {
			cout << "Rule Base로 착수합니다." << endl;


			compute_by_rule_base();

			is_select_finished = true;
		}
		else if (input_play_method == '3') {
			cout << "착수할 지점의 번호(1~7)를 입력해주세요." << endl;

			getchar();
			char input_stone_location;
			do {
				cout << "착수 지점 : ";

				cin >> input_stone_location;

				if (input_stone_location >= '1' && input_stone_location <= '7') {
					stone = input_stone_location - '0';
					is_select_finished = true;
				}
				else {
					cout << "착수 지점을 잘못 입력하였습니다. 다시 입력해주세요." << endl;

				}
			} while (!is_select_finished);
			bool push_complete = check_possible_position(input_stone_location-'0');			// 입력 받은 열에 돌을 놓는다.
			if (!push_complete) {											// 입력 받은 열에 돌을 놓을 수 없다.
				cout << "해당 지점은 돌이 가득 차 있습니다.다시 입력해주세요." << endl;

				is_select_finished = false;
			}
			else {
				push_stone(input_stone_location);
			}
		}
		else {
			cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
		}
	} while (!is_select_finished);
	return stone;
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

int compute_by_search_algorithm() {							// Search Algorithm 에 의해 계산된 지점에 착수
	getchar();

	memcpy(map_temp,map,sizeof(map));
	memcpy(HEIGHT_TEMP, HEIGHT, sizeof(HEIGHT));
	memcpy(GAME_STATE_TEMP, GAME_STATE, sizeof(GAME_STATE));
	game_length_temp = game_length;


	pair<int, int> p;
//	p = negamax_temp(0);									// through negamax

//	p = negamax_temp2(0, 1);								// evaluating 도입

	p = negamax_alphabeta_temp(0, -10000000, 10000000);
	return p.first;
}

void compute_by_rule_base() {									// Rule Base 에 의해 계산된 지점에 착수
	getchar();
}

void push_stone(char input_stone_location) {					// 계산된 위치에 착수
	
//	getchar();
	int i, stone_location=input_stone_location-'0';
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			break;
		}
	}
	map[i + 1][stone_location] = STONE_COLOR;
	GAME_STATE[game_length++] = stone_location;	// 현재까지 어떤 순서로 돌이 놓였는지 저장해준다.
	HEIGHT[stone_location]++;
	if (STONE_COLOR==1) {						// 흑● 일 때 백○ 의 차례로 바꾸어준다.
		STONE_COLOR = 2;
	}
	else {										// 백○ 일 때 흑● 의 차례로 바꾸어준다.
		STONE_COLOR = 1;
	}
}

void print_current_map() {
	int i, j;

	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map[i][j] == 0) {				// 빈칸
				cout << " . ";
			}
			else if (map[i][j] == 1) {			// 흑○
				cout << "○ ";
			}
			else if (map[i][j] == 2) {			// 백●
				cout << "● ";
			}
		}
		cout << "\n";
	}
}

void print_current_map_temp() {
	int i, j;

	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map_temp[i][j] == 0) {				// 빈칸
				cout << " . ";
			}
			else if (map_temp[i][j] == 1) {			// 흑○
				cout << "○ ";
			}
			else if (map_temp[i][j] == 2) {			// 백●
				cout << "● ";
			}
		}
		cout << "\n";
	}
}


int negamax(int depth) { // 하......^0^;
	if (depth == 7) {
		return 0;
	}
	int i;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (check_possible_position(i)) {

		}
	}
	
}

pair<int, int> negamax_alphabeta_temp(int depth, int alpha, int beta) {
	int color = 1 + (game_length + depth) % 2; // 1 선공(흑) 2 후공(백)
	if (depth == 8 || MAX_GAME_LENGTH - game_length_temp <= 0) {
		int i, j;
		/*		for (i = 1; i <= game_length_temp; i++) cout << GAME_STATE_TEMP[i] << " ";
		cout << endl;
		for (i = 1; i <= 7; i++) {
		cout << HEIGHT_TEMP[i] << " ";
		}
		cout << endl;
		print_current_map_temp();
		cout << "평가값 : " << evaluate_state_temp() << endl;
		getchar();*/
		if (color == 1) {
			return make_pair(0, evaluate_board_state());
//			return make_pair(0, evaluate_state_temp());
		}
		else {
			return make_pair(0, -evaluate_board_state());
//			return make_pair(0, -evaluate_state_temp());
		}
		//		return make_pair(0,evaluate_state_temp());
	}
	int i, best = -100000, value;
	int best_move = 0;
	pair<int, int> p;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {

			HEIGHT_TEMP[i]++;
			game_length_temp++;
			GAME_STATE_TEMP[game_length + depth + 1] = i;
			map_temp[HEIGHT_TEMP[i]][i] = color;

			p = negamax_alphabeta_temp(depth + 1,-beta,-alpha);
			value = -p.second;
			
			//			value = -negamax_temp(depth + 1).second;		// 다음 레벨 탐색

			map_temp[HEIGHT_TEMP[i]][i] = 0;
			GAME_STATE_TEMP[game_length + depth + 1] = 0;
			game_length_temp--;
			HEIGHT_TEMP[i]--;

			if (best <= value) {						// 좋은 값을 찾는다. best
				best = value;
				best_move = i;
			}
			if (alpha < value) {
				alpha = value;
				if (alpha >= beta) break;
			}
			
			//			best = max2(best, value);				// 좋은 값을 찾는다. best
		}
	}
	return make_pair(best_move, best);
}

int evaluate_state_temp() { // 하.............^0^;
	int i, j, value=0;
	for (i = 1; i <= MAP_HEIGHT_SIZE; i++) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map_temp[i][j] == 1) {
				value += heuristic_table[i][j];
			}
			else if (map_temp[i][j] == 2) {
				value -= heuristic_table[i][j];
			}
		}
	}
	return value;
}

int evaluate_vertically(int column, int color) {			// 끝에서부터 연속된 동일한 색상의 돌이 많을수록 고득점
	int i, value=0, cnt;
	int v[4] = { 1,8,128,10000 };
	cnt = 0;
	for (i = HEIGHT_TEMP[column]; i >= 1; i--) {
		if (map_temp[i][column] == color) cnt++;
		else break;
	}
	if (cnt < 4) return v[cnt];
	else return v[3];
}

int evaluate_horizontally(int row, int color) {				// 중간에 띄워서 놓는 경우의 수는? 나중에..
	int i, j, value = 0, cnt;
	int v[4] = { 1,8,128,10000 };
	cnt = 0;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (map_temp[row][i] == color) cnt++;
		else break;
//		else if (map_temp[row][i] != 0) break;
	}
	if (cnt < 4) return v[cnt];
	else return v[3];
}

int evaluate_diagonally(int column, int row, int color) {
	int i, value = 0, cnt;
	int v[4] = { 1,8,128,10000 };
	cnt = 0;
	int y, x;
	y = row;
	x = column;
	for (;y >= 1 && x>=1; y--, x--) {			// 좌하향 대각선탐색
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != color) break;
	}
	if (cnt < 4) value += v[cnt];
	else value += v[3];
	cnt = 0;
	y = row;
	x = column;
	for (;y <= MAP_HEIGHT_SIZE && x <= MAP_WIDTH_SIZE; y++, x++) {	// 우하향 대각선탐색
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != color) break;
	}
	if (cnt < 4) return value + v[cnt];
	return value + v[3];
}

int evaluate_board_state() {					// color 를 받으면 + - 를 결정해서 이기고 지는걸 세분화 시킬 수 있을까? 아니면 호출한 부분에서 바로 결정해줄수 있는건가?
	int i, j;
	int value_black = 0;
	int value_white = 0;
	int v[4] = { 1,8,128,10000 };
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (map_temp[HEIGHT_TEMP[i]][i] == 1) {
			value_black += evaluate_vertically(i, 1);
		}
		else if (map_temp[HEIGHT_TEMP[i]][i] == 2) {
			value_white += evaluate_vertically(i, 2);
		}
	}
	for (i = 1; i <= MAP_HEIGHT_SIZE; i++) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map_temp[i][j] == 1) {			// 선공일 때 점수 계산
//				value_black += evaluate_vertically(i, 1);
				value_black += evaluate_horizontally(j, 1);
				value_black += evaluate_diagonally(i, j, 1);
			}
			else if (map_temp[i][j] == 2) {
//				value_white += evaluate_vertically(i, 2);
				value_white += evaluate_horizontally(j, 2);
				value_white += evaluate_diagonally(i, j, 2);
			}
		}
	}
	return value_black - value_white;
}

pair<int,int> negamax_temp2(int depth, int color) {
	if (depth == 8 || MAX_GAME_LENGTH - game_length_temp <= 0) {
		return make_pair(0, evaluate_board_state());
	}
	int i, ccolor=1+game_length_temp%2;
	int best = -1000000, best_move;
	int value;
	pair<int, int> p;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {
			HEIGHT_TEMP[i]++;
			game_length_temp++;
			GAME_STATE_TEMP[game_length + depth + 1] = i;
			map_temp[HEIGHT_TEMP[i]][i] = ccolor;

			p = negamax_temp2(depth + 1,-color);
			value = -p.second;

			//			value = -negamax_temp(depth + 1).second;		// 다음 레벨 탐색

			map_temp[HEIGHT_TEMP[i]][i] = 0;
			GAME_STATE_TEMP[game_length + depth + 1] = 0;
			game_length_temp--;
			HEIGHT_TEMP[i]--;

			if (best <= value) {						// 좋은 값을 찾는다. best
				best = value;
				best_move = i;
			}
		}
	}
	return make_pair(best_move, best);
}

pair<int,int> negamax_temp(int depth) { // 제바ㅏㅏㅏㅏㅏㅏㅏㅏㅏㄹ
	int color = 1 + (game_length + depth) % 2; // 1 선공(흑) 2 후공(백)
	if (depth == 9 || MAX_GAME_LENGTH-game_length_temp<=0) {
		int i, j;
/*		for (i = 1; i <= game_length_temp; i++) cout << GAME_STATE_TEMP[i] << " ";
		cout << endl;
		for (i = 1; i <= 7; i++) {
			cout << HEIGHT_TEMP[i] << " ";
		}
		cout << endl;
		print_current_map_temp();
		cout << "평가값 : " << evaluate_state_temp() << endl;
		getchar();*/
		if (color == 1) {
			return make_pair(0, evaluate_state_temp());
		}
		else {
			return make_pair(0, -evaluate_state_temp());
		}
//		return make_pair(0,evaluate_state_temp());
	}
	int i, best=-100000, value;
	int best_move=0;
	pair<int, int> p;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
		if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {
/*			if (is_winning_temp(i)) {			// color가 i에 뒀더니 이겼을 때 (color == 1 //선공, color == 2 // 후공)
				if(color==1){
					return make_pair(i, 10000);	// ???????????????
				}
				else {
					return make_pair(i, -10000); // ??????????????

				}
			}*/
			HEIGHT_TEMP[i]++;
			game_length_temp++;
			GAME_STATE_TEMP[game_length + depth + 1] = i;
			map_temp[HEIGHT_TEMP[i]][i] = color;

			p = negamax_temp(depth + 1);
			value = -p.second;

//			value = -negamax_temp(depth + 1).second;		// 다음 레벨 탐색

			map_temp[HEIGHT_TEMP[i]][i] = 0;
			GAME_STATE_TEMP[game_length + depth + 1] = 0;
			game_length_temp--;
			HEIGHT_TEMP[i]--;

			if (best <= value) {						// 좋은 값을 찾는다. best
				best = value;
				best_move = i;
			}
//			best = max2(best, value);				// 좋은 값을 찾는다. best
		}
	}
	return make_pair(best_move,best);//???????????????????????????????
}



bool is_winning_temp(int column) {
	int color = 1 + game_length_temp%2;
	int dx, dy, cnt=0;
	
	if(HEIGHT_TEMP[column]>=3													// 세로로 만들어진 경우
		&& map_temp[HEIGHT_TEMP[column]-1][column]==color
		&& map_temp[HEIGHT_TEMP[column]-2][column]==color
		&& map_temp[HEIGHT_TEMP[column] - 3][column] == color) return true;
	for (dy = -1; dy <= 1; dy++) {												// 대각선 및 가로로 만들어진 경우
		cnt = 0;
		for (dx = -1; dx <= 1; dx += 2) {
			for (int x = column + dx, y = HEIGHT_TEMP[column] + dx*dy; x >= 1 && x <= MAP_WIDTH_SIZE && y >= 1 && y <= MAP_HEIGHT_SIZE && map_temp[y][x] == color;) {
				x += dx;
				y += dx * dy;
				cnt++;
			}
		}
		if (cnt >= 3) return true;
	}
	return false;
}

bool is_winning(int column) {
	int color = 1 + game_length_temp % 2;
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