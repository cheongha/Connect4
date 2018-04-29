#include <iostream>

using namespace std;

#define MAP_WIDTH_SIZE 7
#define MAP_HEIGHT_SIZE 6
#define MAX_GAME_LENGTH 42

int map[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];					// Coonect 4 는 map은 가로 7칸 세로 6칸의 7 by 6 짜리 게임

int STONE_COLOR = 1;												// 현재 어떤 색이 착수할 차례인지 (1==○ 2==●)
																	// !**선/후공 나누는 구현 필요**!

int game_length;													// 몇개의 착수가 이뤄졌는지
int game_state[MAX_GAME_LENGTH];									// 현재까지 게임이 어떻게 진행되었는지 저장하는 배열

int heuristic_table[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1] = {{ 3, 4, 5, 7, 5, 4, 3 },				// heuristic table
																{ 4, 6, 8, 10, 8, 6, 4 },				// 각 점수는 해당 위치에 돌이 존재할 때 승리하게 될 가중치
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 4, 6, 8, 10, 8, 6, 4 },
																{ 3, 4, 5, 7, 5, 4, 3 } };


void print_intro();													// 최초 실행 시 나타나는
void select_first_player();											// 선공 결정
void select_play_method();											// 1.search algorithm	2.rule base		3.직접 입력

void compute_by_search_algorithm();									// search algorithm 을 통한 착수점 계산
int negamax(int depth);												// search algorithm 에 사용될 negamax 함수

void compute_by_rule_base();										// rule base 로 착수점 계산

bool check_possible_position(int stone_location);
void push_stone(char input_stone_location);							//

void print_current_map();

void play_connect4();



int main() {

	print_intro();
	// 선/후공 결정 구현 필요
	// 현재 무조건 흑이 선공
//	select_first_player();
	play_connect4();
/*	do {
		select_play_method();
		print_current_map();
	} while (true);*/

	return 0;
}

void print_intro() {																// 최초 실행 시 나타나는 안내문구
	cout << "**************************************************************\n";
//	printf("**************************************************************\n");
	cout << "**************************Connect 4***************************\n";
//	printf("**************************Connect 4***************************\n");
	cout << "**************************************************************\n";
//	printf("**************************************************************\n");
	cout << "*********Copyright by 아버지날보고있다면정답을알려조**********\n";
//	printf("*********Copyright by 아버지날보고있다면정답을알려조**********\n");
	cout << "**************************************************************\n";
//	printf("**************************************************************\n");
	cout << "****************************************2018.04.29************\n";
//	printf("****************************************2018.04.29************\n");
	cout << "**************************************************************\n";
//	printf("**************************************************************\n");


}

void select_first_player() {
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

void select_play_method() {															// 작수 방식 결정
	cout << endl;
//	printf("\n");
	bool is_select_finished = false;
	do {
		cout << "착수 방식을 선택해주세요. (1. Search Algorithm, 2. Rule Base, 3. 직접 입력)" << endl;
//		printf("착수 방식을 선택해주세요. (1. Search Algorithm, 2. Rule Base, 3. 직접 입력)\n");
		char input_play_method;
		cin >> input_play_method;
//		scanf("%s", &input_play_method);
		if (input_play_method == '1') {
			cout << "Search Algorithm에 기반하여 착수합니다." << endl;
//			printf("Search Algorithm에 기반하여 착수합니다.\n");
			
			compute_by_search_algorithm();

			is_select_finished = true;
		}
		else if (input_play_method == '2') {
			cout << "Rule Base로 착수합니다." << endl;
//			printf("Rule Base로 착수합니다.\n");

			compute_by_rule_base();

			is_select_finished = true;
		}
		else if (input_play_method == '3') {
			cout << "착수할 지점의 번호(1~7)를 입력해주세요." << endl;
//			printf("착수할 지점의 번호(1~7)를 입력해주세요.\n");
			getchar();
			char input_stone_location;
			do {
				cout << "착수 지점 : ";
//				printf("착수 지점 : ");
				cin >> input_stone_location;
//				scanf("%c", &input_stone_location);
				if (input_stone_location >= '1' && input_stone_location <= '7') {
					is_select_finished = true;
				}
				else {
					cout << "착수 지점을 잘못 입력하였습니다. 다시 입력해주세요." << endl;
//					printf("착수 지점을 잘못 입력하였습니다. 다시 입력해주세요.\n");
				}
			} while (!is_select_finished);
			bool push_complete = check_possible_position(input_stone_location-'0');			// 입력 받은 열에 돌을 놓는다.
			if (!push_complete) {											// 입력 받은 열에 돌을 놓을 수 없다.
				cout << "해당 지점은 돌이 가득 차 있습니다.다시 입력해주세요." << endl;
//				printf("해당 지점은 돌이 가득 차 있습니다. 다시 입력해주세요.\n");
				is_select_finished = false;
			}
			else {
				push_stone(input_stone_location);
			}
		}
		else {
			cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
//			printf("잘못된 입력입니다. 다시 입력해주세요.\n\n");
		}
	} while (!is_select_finished);
	
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

void compute_by_search_algorithm() {							// Search Algorithm 에 의해 계산된 지점에 착수
	getchar();

	int stone_location;
	for (stone_location = 1; stone_location <= MAP_WIDTH_SIZE; stone_location++) {
		if (check_possible_position(stone_location)) {						// 착수 가능한 지점인지 판별

		}
	}










}

void compute_by_rule_base() {									// Rule Base 에 의해 계산된 지점에 착수
	getchar();
}

void push_stone(char input_stone_location) {					// 계산된 위치에 착수
	
	getchar();
	int i, stone_location=input_stone_location-'0';
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			break;
		}
	}
	map[i + 1][stone_location] = STONE_COLOR;
	game_state[game_length++] = stone_location;	// 현재까지 어떤 순서로 돌이 놓였는지 저장해준다.
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

int negamax(int depth) {
	if (depth == 7) {
		return 0;
	}
	int i;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {

	}
	
}

void play_connect4() {
	do {
		select_play_method();
		print_current_map();
	} while (true);
}