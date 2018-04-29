#include <cstdio>

#define MAP_WIDTH_SIZE 7
#define MAP_HEIGHT_SIZE 6

int map[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];					// Coonect 4 는 map은 가로 7칸 세로 6칸의 7 by 6 짜리 게임이다.

int stone_color = 1;												// 현재 어떤 색이 착수할 차례인지 (1==● 2==○)
																	// !**선/후공 나누는 구현 필요**!

int game_length;

int heuristic_table[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1] = {{ 3, 4, 5, 7, 5, 4, 3 },				// heuristic table
																{ 4, 6, 8, 10, 8, 6, 4 },				// 각 점수는 해당 위치에 돌이 존재할 때 승리하게 될 가중치이다.
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 5, 8, 11, 13, 11, 8, 5 },				
																{ 4, 6, 8, 10, 8, 6, 4 },
																{ 3, 4, 5, 7, 5, 4, 3 } };


void print_intro();

void select_play_method();

void compute_by_search_algorithm();
int negamax(int depth);

void compute_by_rule_base();

bool push_stone(char input_stone_location);

void print_current_map();


int main() {

	print_intro();
	// 선/후공 결정 구현 필요
	// 현재 무조건 흑이 선공
	do {
		select_play_method();
		print_current_map();
	} while (true);

	return 0;
}

void print_intro() {																// 최초 실행 시 나타나는 안내문구
	printf("**************************************************************\n");
	printf("**************************Connect 4***************************\n");
	printf("**************************************************************\n");
	printf("*********Copyright by 아버지날보고있다면정답을알려조**********\n");
	printf("**************************************************************\n");
	printf("****************************************2018.04.29************\n");
	printf("**************************************************************\n");


}

void select_play_method() {															// 작수 방식 결정
	printf("\n");
	bool is_select_finished = false;
	do {
		printf("착수 방식을 선택해주세요. (1. Search Algorithm, 2. Rule Base, 3. 직접 입력)\n");
		char input_play_method;
		scanf("%c", &input_play_method);
		if (input_play_method == '1') {
			printf("Search Algorithm에 기반하여 착수합니다.\n");
			
			compute_by_search_algorithm();

			is_select_finished = true;
		}
		else if (input_play_method == '2') {
			printf("Rule Base로 착수합니다.\n");

			compute_by_rule_base();

			is_select_finished = true;
		}
		else if (input_play_method == '3') {
			printf("착수할 지점의 번호(1~7)를 입력해주세요.\n");
			getchar();
			char input_stone_location;
			do {
				printf("착수 지점 : ");
				scanf("%c", &input_stone_location);
				if (input_stone_location >= '1' && input_stone_location <= '7') {
					is_select_finished = true;
				}
				else {
					printf("착수 지점을 잘못 입력하였습니다. 다시 입력해주세요.\n");
				}
			} while (!is_select_finished);
			bool push_complete = push_stone(input_stone_location);			// 입력 받은 열에 돌을 놓는다.
			if (!push_complete) {											// 입력 받은 열에 돌을 놓을 수 없다.
				printf("해당 지점은 돌이 가득 차 있습니다. 다시 입력해주세요.\n");
				is_select_finished = false;
			}
		}
		else {
			printf("잘못된 입력입니다. 다시 입력해주세요.\n\n");
		}
	} while (!is_select_finished);
	
}

void compute_by_search_algorithm() {							// Search Algorithm 에 의해 계산된 지점에 착수
	getchar();

	negamax(0);










}

void compute_by_rule_base() {									// Rule Base 에 의해 계산된 지점에 착수
	getchar();
}

bool push_stone(char input_stone_location) {					// 계산된 위치에 착수
	
	getchar();
	int i, stone_location=input_stone_location-'0';
	printf("%c %d\n", input_stone_location, stone_location);
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			if (i == MAP_HEIGHT_SIZE) return false;		// 해당 열에 6개의 돌이 모두 들어가 있는 경우
			break;
			printf("%d", map[i][stone_location]);
		}
	}
	printf("%d\n", stone_color);
	map[i + 1][stone_location] = stone_color;
	if (stone_color==1) {						// 흑●
		stone_color = 2;
	}
	else {										// 백○
		stone_color = 1;
	}
	printf("%d\n", stone_color);
	return true;
}

void print_current_map() {
	int i, j;

	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map[i][j] == 0) {				// 빈칸
				printf(" . ");
			}
			else if (map[i][j] == 1) {			// 흑●
				printf("● ");
			}
			else if (map[i][j] == 2) {			// 백○
				printf("○ ");
			}
		}
		printf("\n");
	}
}

int negamax(int depth) {
	if (depth == 10) {
		return 0;
	}
	int i;
	for (i = 1; i <= MAP_WIDTH_SIZE; i++) {

	}
	
}