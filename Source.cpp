#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define MAP_WIDTH_SIZE 7											// Connect 4 ������ ���� ���̴� 7ĭ
#define MAP_HEIGHT_SIZE 6											// Connect 4 ������ ���� ���̴� 6ĭ
#define MAX_GAME_LENGTH 42											// Connect 4 ���� �ִ� ������ ���̴� 42ȸ(6*7)

int map[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];					// Coonect 4 �� map�� ���� 7ĭ ���� 6ĭ�� 7 by 6 ¥�� ����, ���� �Ͽ��� ������ ���� ���¸� ��Ÿ���� �迭
int HEIGHT[MAP_WIDTH_SIZE + 1];										// �� ������ ��� ���� ������ �ִ��� ������ ���� �迭
int GAME_STATE[MAX_GAME_LENGTH + 1];								// �� �ϸ��� � ��ġ�� ���� �ξ����� ������ ���� �迭


int map_temp[MAP_HEIGHT_SIZE + 1][MAP_WIDTH_SIZE + 1];				// �ӽ÷� ����� map
int HEIGHT_TEMP[MAP_WIDTH_SIZE + 1];								// �ӽ÷� ����� HEIGHT

int a, b, c, d;														// Rule base ���� ����� check ����


int STONE_COLOR = 1;												// ���� � ���� ������ �������� (1==�� 2==��)
																	// !**��/�İ� ������ ���� �ʿ�**!

int game_length;													// ��� ������ �̷�������
int game_length_temp;												// �ӽ÷� ����� game_length_temp


void print_intro();													// ���� ���� �� ��Ÿ���� ȭ��
void select_first_player();											// ���� ����
int select_play_method();											// 1.search algorithm	2.rule base		3.���� �Է�

int compute_by_search_algorithm();									// search algorithm �� ���� ������ ���
int compute_by_rule_base();											// rule base �� ������ ���

void push_stone(char input_stone_location);							// �������� ����

void print_current_map();											// ������� ����� ������ state

void play_connect4();												// ���� �÷���

int evaluate_board_state();

bool check_possible_position(int stone_location);					// �ش� ������ ���� ���� �� �ִ���?
bool is_winning(int column);										// ���� ���¿��� column�� ���� �� �̱�°�?
bool is_winning_temp(int column);									// ���� ���¿��� column�� ���� �� �̱�°�? �ӽ� �ڵ�



int heuristic_value[3][3][3][3][3][3][3];

int minmax(int depth);
int minmax_alphabeta_pruning(int depth, int alpha, int beta);

int check_connect3(int count, int color);
int check_nconnect3(int count, int color);

int find_another_column(int count);

int main() {

	print_intro();
	// ��/�İ� ���� ���� �ʿ� ?
	// ���� ������ ���� ����
	//	select_first_player();
	play_connect4();

	return 0;
}
void print_intro() {																// ���� ���� �� ��Ÿ���� �ȳ�����
	printf("**************************************************************************\n");
	printf("********************************Connect 4*********************************\n");
	printf("**************************************************************************\n");
	printf("***************Copyright by �ƹ����������ִٸ��������˷���****************\n");
	printf("**************************************************************************\n");
	printf("****************************************************2018.05.03************\n");
	printf("**************************************************************************\n");

}

void play_connect4() {											// ������ �����Ѵ�
	int stone;
	do {
		stone = select_play_method();							// ��� ���� �ξ����� �޾ƿ´�. play_method�� 1. Search, 2.Rule, 3.�����Է� �� �ִ�.
		print_current_map();									// ������ ������� ���� �ξ��� �� ������ ���� ���� ��
		printf("������ ���� : %d\n", stone);						// ��� �״��� Ȯ��
													
		if (is_winning(stone)) {								// ���� �� ���� ���и� �����ߴ°�?
			if (game_length % 2) printf("������ �¸�\n");		// ���� �� ����� �̱� ��Ȳ
			else printf("�İ��� �¸�\n");						// ���߿� �� ����� �̱� ��Ȳ
			break;
		}
		if (game_length == MAX_GAME_LENGTH) {					// game �� ���а� ���� �ʾҴ�.
			printf("���º�\n");
			break;
		}
	} while (true);
	printf("���� ����\n");
	getchar();
	printf("�����Ϸ��� �ƹ�Ű�� ��������.\n");
	getchar();
}


int select_play_method() {															// �ۼ� ��� ����
	printf("\n");

	bool is_select_finished = false;												// ���� ���������� ���� ������ while ���� ��� ���ƾ� �ϹǷ� �̸� �Ǵ����ִ� boolean ����
	int stone;

	clock_t begin, end;

	do {
		printf("���� ����� �������ּ���. (1. Search Algorithm, 2. Rule Base, 3. ���� �Է�) : ");

		char input_play_method;
		scanf("%c",&input_play_method);												// ���� ��� �Է¹���
//		getchar(); // ?
		begin = clock();															// �� ������� �����ϴµ� �ɸ��� �ð� ���� ����

		if (input_play_method == '1') {												// 1. Search �� ���� �� ��
			printf("Search Algorithm�� ����Ͽ� �����մϴ�.\n");

			stone = compute_by_search_algorithm();									// compute_by_search_algorithm �Լ��� �̿��ؼ� ��� ��ġ�� ���� �� ������ �˾ƿ´�.
			//cout << "���� ������ : " << stone << endl;
			push_stone(stone + '0');												// �˾ƿ� ���� ��ġ�� ���� �д�

			is_select_finished = true;												// ���� �ξ����� while���� ���������� ���� true�� �ٲ��ش�.
		}
		else if (input_play_method == '2') {										// 2. Rule �� ���� �� ��
			printf("Rule Base�� �����մϴ�.\n");

			stone = compute_by_rule_base();													// compute_by_rule_base �Լ��� �̿��ؼ� ��� ��ġ�� ���� �� ������ �˾ƿ´�.
			push_stone(stone + '0');
			is_select_finished = true;												// ���� rule base �� �δ� �Լ� ���� �ȵ�
		}
		else if (input_play_method == '3') {
			printf("������ ������ ��ȣ(1~7)�� �Է����ּ���.\n");
			getchar();
			char input_stone_location;
			do {
				printf("���� ���� : ");
				scanf("%c",&input_stone_location);										// ����ڰ� ���� �α�� ���� �Է��� ��ġ
				getchar();
				if (input_stone_location >= '1' && input_stone_location <= '7') {	// 1~7 ������ ��Ȯ�ϰ� �Է����� ��
					stone = input_stone_location - '0';
					is_select_finished = true;
				}
				else {																// 1~7 �̿��� �Է��� �޾����� �̴� �߸��� �Է�
					printf("���� ������ �߸� �Է��Ͽ����ϴ�. �ٽ� �Է����ּ���.\n");

				}
			} while (!is_select_finished);
			bool push_complete = check_possible_position(input_stone_location - '0');			// �Է� ���� ���� ���� ���� �� �ִ��� Ȯ�����ش�.
			if (!push_complete) {																// �Է� ���� ���� ���� ���� �� ����.
				printf("�ش� ������ ���� ���� �� �ֽ��ϴ�.�ٽ� �Է����ּ���.\n");
				is_select_finished = false;
			}
			else {																				// �Է� ���� ���� ���� �� �� �ִ�.
				push_stone(input_stone_location);
			}
		}
		else {																		// 1.Search, 2.Rule, 3.�����Է� ���� �ٸ� �Է����� ���� �߸��� �Է�.
			printf("�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
		}

		end = clock();																// ���� ���µ� �ɸ� �ð� ���� ����

		printf("����ð� : %d��\n",((end - begin) / CLOCKS_PER_SEC));			// ���� �ϳ� ���µ� �ɸ� �ð��� �ʴ����� ���
	} while (!is_select_finished);
	return stone;
}

int compute_by_search_algorithm() {							// Search Algorithm �� ���� ���� ������ ����
	getchar();
	int best = -1000000, best_move = 0, value;
	if (game_length == 0) return 3;							// ó���� 4�� ���� �� �� ��� �� �������� ���� ���� 3�� ���� �д�.
	if (game_length % 2 == 1) best = 1000000;
	for (int i = 1; i <= MAP_WIDTH_SIZE; i++) {				// �ش� �Ͽ��� �� �� �ִ� ��ġ Ž��
		if (HEIGHT[i] < MAP_HEIGHT_SIZE) {					// �ش� ĭ�� ���̰� ����Ͽ� �� �� �� �� ���� ��
			HEIGHT[i]++;									// �״ٰ� ����, �ؿ��� -- ���� ����.
			map[HEIGHT[i]][i] = 1 + game_length % 2;		// �״ٰ� ����, 1�� ���� 2�� �İ�/ game_length�� 0���� ������
			game_length++;									// �״ٰ� ����
			if (is_winning(i)) {							// �˰��� �̱�� ������?
				game_length--;								// ������ ���� �ִ� �Լ��� push_stone�̹Ƿ� ��� �־��� ���¸� ����
				map[HEIGHT[i]][i] = 0;
				HEIGHT[i]--;
				return i;									// �̱�� ���� �����ϵ��� �� ��ȯ
			}
			game_length--;

			memcpy(map_temp, map, sizeof(map));				// �ӽù迭�� �̿��� depth Ž��
			memcpy(HEIGHT_TEMP, HEIGHT, sizeof(HEIGHT));

			game_length_temp = game_length + 1;				// �� �� �����ϱ� +1
															//			value = minmax(1);									// �� �� ���� ���� ����. minmax �̿�
			value = minmax_alphabeta_pruning(1, -1000000, 1000000); // �� �� ���� ���� ����. minmax with alphabeta pruning

			if (game_length % 2 == 0) {							// ������ ���� ����� max
				if (best <= value) {
					if (best == value) {						// ���� ���� best�� �����ȴٸ�
						if (abs(4 - i) < abs(4 - best_move)) best_move = i;	// ����� ����� �ָ� ������ �� �ֵ���
					}
					else {
						best = value;
						best_move = i;
					}
				}
//				cout << "game_length : " << game_length + 1 << " score : " << value << " which? : " << i << endl;	// ������
				printf("game_length : %d score : %d which? : %d\n",game_length+1,value,i);
			}
			else {												// �İ��� ���� ����� min
				if (best >= value) {
					if (best == value) {						// ���� ���� best�� �����ȴٸ�
						if (abs(4 - i) < abs(4 - best_move)) best_move = i;	// ����� ����� �ָ� ������ �� �ֵ���
					}
					else {
						best = value;
						best_move = i;
					}
				}
//				cout << "game_length : " << game_length + 1 << " score : " << value << " which? : " << i << endl;	// ������
				printf("game_length : %d score : %d which? : %d\n",game_length+1,value,i);
			}
			map[HEIGHT[i]][i] = 0;							// ���� ���� Ž���ϴµ� ������ ��ġ�� �ʱ� ���� �α� ���� ���·� ��������
			HEIGHT[i]--;
		}
	}
	return best_move;										// ���� ���� ���� ���� ��ġ�� ��ȯ
}


int check_connect3(int count, int color) {// 1-> color 2->opponent_color
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 8; j++) {
			//���η� 3�� �����ؼ� ���� ��
			//�� 3���� �� �Ʒ��ٿ� ���η� �������� ������
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
			//�� �Ʒ����� ������ �ٿ� 3���� ���η� �������� ������
			if (i>1 && j < 5) {				// ������ �д�
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j + 3] == 0 &&
					(map[i - 1][j + 3] == 1 || map[i - 1][j + 3] == 2)) {
					return j + 3;
				}
			}
			if (i > 1 && j > 1 && j < 6) {	// ������ �д�
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 2] == color && map[i][j - 1] == 0 &&
					(map[i - 1][j - 1] == 1 || map[i - 1][j - 1] == 2)) {
					return j - 1;
				}
			}
			// OOO ó�� ��

			//���η� 3���� �������� ���� ��
			if (i < 4) {
				if (map[i][j] == color && map[i + 1][j] == color && map[i + 2][j] == color && map[i + 3][j] == 0 && i < 4 && i>0) {
					return j;
				}
			}
			// ���η� 3�� ���� ó�� ��

			//�밢������ 3�� ������ case 1(�� ����)
			if (i > 2 && i < 6 && j>1 && j < 6) {		// ���� ���ʿ� �ּ� �밢��4���� �����
				if (map[i][j] == color && map[i - 1][j + 1] == color && map[i - 2][j + 2] == color) {
					if (map[i + 1][j - 1] == 0 && (map[i][j - 1] == 1 || map[i][j - 1] == 2)) {
						return j - 1;
					}
				}
			}
			if (i > 3 && j < 5) {						// ���� �Ʒ��ʿ� �ּ� �밢��4���� �����.
				if (map[i][j] == color && map[i - 1][j + 1] == color && map[i - 2][j + 2] == color) {
					if (map[i - 3][j + 3] == 0 && (map[i - 4][j + 3] == 1 || map[i - 4][j + 3] == 2 || i == 4)) {
						return j + 3;
					}
				}
			}
			//�밢������ 3�� ���� case 1 ó�� 


			//�밢������ 3�� ������ case 2 (�� ����)		// ���� �Ʒ��ʿ� �ּ� �밢�� 4���� �����
			if (i > 3 && j > 3) {
				if (map[i][j] == color && map[i - 1][j - 1] == color && map[i - 2][j - 2] == color) {
					if (map[i - 3][j - 3] == 0 && (map[i - 4][j - 3] == 1 || map[i - 4][j - 3] == 2 || i == 4)) {
						return j - 3;
					}
				}
			}
			if (i > 2 && i < 6 && j>2 && j < 7) {		// ���� ���ʿ� �ּ� �밢�� 4���� �����
				if (map[i][j] == color && map[i - 1][j - 1] == color && map[i - 2][j - 2] == color) {
					if (map[i + 1][j + 1] == 0 && (map[i][j + 1] == 1 || map[i][j + 1] == 2)) {
						return j + 1;
					}
				}
			}//�밢������ 3�� ���� case 2 ó�� �� 
		}
	}//for�� ��
	return check_nconnect3(count, color);
}

//������� ������ �Ѽ��� �� �θ� ������ ���� ������ ��
int check_nconnect3(int count, int color) {
	for (int i = 1; i < 7; i++) {
		for (int j = 1; j < 8; j++) {

			// oo o �϶�, �� �Ʒ����� �ƴ� ��
			if (i > 1 && j<5) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 3] == color && map[i][j + 2] == 0 &&
					(map[i - 1][j + 2] == 1 || map[i - 1][j + 2] == 2)) {
					return j + 2;
				}
			}
			//�� �Ʒ����϶�
			if (i == 1 && j<5) {
				if (map[i][j] == color && map[i][j + 1] == color && map[i][j + 3] == color && map[i][j + 2] == 0) {
					return j + 2;
				}
			}

			//o oo �϶�, �� �Ʒ����� �ƴ� ��
			if (i > 1 && j < 5) {
				if (map[i][j] == color && map[i][j + 2] == color && map[i][j + 3] == color && map[i][j + 1] == 0 &&
					(map[i - 1][j + 1] == 1 || map[i - 1][j + 1] == 2)) {
					return j + 1;
				}
			}
			//�� �Ʒ����϶�
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
	}//for�� ��
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

int compute_by_rule_base() {									// Rule Base �� ���� ���� ������ ����
	getchar();

	/*
	created by Seonyoung Kim
	*/
	if (game_length == 0) return 3;
	int count = game_length + 1;
	int color = 1 + (game_length) % 2;
	int opponent_color = 1 + (game_length + 1) % 2;				// ���� �� �Ǵ�
	int ret;


	ret = check_connect3(count, color);							// �� ������� ���� �� �������� 4���� ���� �� �ִ��� �Ǵ�
	if (ret == 0) {												// 4���� ���� �� ������ ������ 4���� ���� �� �ִ��� �Ǵ�
		ret = check_connect3(count, opponent_color);
		if (ret != 0) return ret;								// ������ 4���� ���� �� �ִٸ� ���´�
	}
	else return ret;											// 4���� ���� �� �ִٸ� �д�

																/*
																�迭 index ������...
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
	if (HEIGHT[GAME_STATE[count - 1]] < MAP_HEIGHT_SIZE) {				// ������ ��� ���� �� �� ���� �δ� ��
		return GAME_STATE[count - 1];
	}
	else {																// ���� �� ���� ���� �� �ٸ� ���� ã�Ƽ� �д�.
		return find_another_column(count);
	}

}

bool check_possible_position(int stone_location) {				// ���� ������ �������� �Ǻ�(=�ش� ���� �̹� ���� 6���� �� �ִ��� �ƴ��� �Ǻ�)
	int i;
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			if (i == MAP_HEIGHT_SIZE) return false;
			return true;
		}
	}
	return true;
}
void push_stone(char input_stone_location) {					// ���� ��ġ�� ����

	int i, stone_location = input_stone_location - '0';
	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		if (map[i][stone_location]) {
			break;
		}
	}
	map[i + 1][stone_location] = STONE_COLOR;
	HEIGHT[stone_location]++;
	game_length++;
	GAME_STATE[game_length] = stone_location;	// �� ȸ°�� � ĭ�� �ξ����� ����
	if (STONE_COLOR == 1) {						// ��� �� �� ��� �� ���ʷ� �ٲپ��ش�.
		STONE_COLOR = 2;
	}
	else {										// ��� �� �� ��� �� ���ʷ� �ٲپ��ش�.
		STONE_COLOR = 1;
	}
}

void print_current_map() {										// ������� ������ ���� ���¸� ǥ��
	int i, j;

	for (i = MAP_HEIGHT_SIZE; i > 0; i--) {
		for (j = 1; j <= MAP_WIDTH_SIZE; j++) {
			if (map[i][j] == 0) {				// ��ĭ
				printf(" . ");
			}
			else if (map[i][j] == 1) {			// ���
				printf("�� ");
			}
			else if (map[i][j] == 2) {			// ���
				printf("�� ");
			}
		}
		printf("\n");
	}
}

bool is_winning_temp(int column) {								// �ӽ� map �� ��Ȳ�� �̱�� ��Ȳ�� �Ǵ��� �Ǵ����ִ� �Լ�
	int color = 1 + (game_length_temp + 1) % 2;
	int dx, dy, cnt = 0;

	if (HEIGHT_TEMP[column] >= 3													// ���η� ������� ���
		&& map_temp[HEIGHT_TEMP[column] - 1][column] == color
		&& map_temp[HEIGHT_TEMP[column] - 2][column] == color
		&& map_temp[HEIGHT_TEMP[column] - 3][column] == color) return true;
	for (dy = -1; dy <= 1; dy++) {												// �밢�� �� ���η� ������� ���
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

bool is_winning(int column) {									// map�� ��Ȳ�� �̱�� ��Ȳ�� �Ǵ��� �Ǵ����ִ� �Լ�
	int color = 1 + (game_length + 1) % 2;
	int dx, dy, cnt = 0;

	if (HEIGHT[column] >= 3													// ���η� ������� ���
		&& map[HEIGHT[column] - 1][column] == color
		&& map[HEIGHT[column] - 2][column] == color
		&& map[HEIGHT[column] - 3][column] == color) return true;
	for (dy = -1; dy <= 1; dy++) {												// �밢�� �� ���η� ������� ���
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
	if (depth >= 12) {														// 12�� �ձ��� ���ٺ��ų� 12�� ���� ������ ������ ���ٺ��ų�
		if (game_length_temp > 12) {											// ������ ���̰� �������
			if (depth == 12 + (game_length_temp - 13) / 5) {					// ���� �� ���ٺ���
				return evaluate_board_state();
			}
		}
		else {
			return evaluate_board_state();
		}
	}
	int i;
	int best;
	if ((game_length + depth) % 2 == 0) {				// ���� max���� ����
		best = -1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// �ش� ���� �� �� �ִٸ�
				HEIGHT_TEMP[i]++;						// �ϳ��� �ֺ���
				map_temp[HEIGHT_TEMP[i]][i] = 1;
				game_length_temp++;
				if (is_winning_temp(i)) {				// �ٵ� ���⿡ �θ� �̱��?
					game_length_temp--;						// �ӽù迭 ���� <- ���� ���� push_stone ���� �־��ְ� �ǰ�, ���� �ٸ� ��忡�� ������� ������ ���� �ʱ� ���� �ӽù迭�� �ֱ� �� ���·� �����־���Ѵ�.
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return (21 - (game_length_temp + 1) / 2) * 10000;		// ��ŭ�� Ƚ���� �Ʋ��� ���ӿ��� �¸��ϴ���, �� ������ ���̴� 42ȸ �̰�, �� �÷��̾�� 21ȸ�� ������ �÷��� �� �� �ִ�.
				}

				value = minmax_alphabeta_pruning(depth + 1, alpha1, beta1);	// ���� �� Ž��
				if (best <= value) {					// ���� ������ ������ ���� �� ���ٸ�
					best = value;
				}
				game_length_temp--;						// ���� ���� ������ ���� �ʱ� ���� �ӽù迭�� �ֱ� �� ���·� �����ش�.
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
				if (alpha1 < best) {					// alpha ���� �ٿ����� ��
					alpha1 = best;
					if (beta1 <= alpha1) break;			// alpha cut
				}
			}
		}
	}
	else {												// �İ� min���� ����
		best = 1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// �ش� ���� �� �� �ִٸ�
				HEIGHT_TEMP[i]++;						// �ֺ��µ�
				map_temp[HEIGHT_TEMP[i]][i] = 2;
				game_length_temp++;
				if (is_winning_temp(i)) {				// �ٵ� ���⿡ �θ� �̱��?
					game_length_temp--;							// �ӽù迭 ����
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return -((21 - (game_length_temp + 1) / 2) * 10000);		// ��ŭ�� Ƚ���� �Ʋ��� ���ӿ��� �¸��ϴ���, �İ��� ���̹Ƿ� ����(-)�� ��ȯ(?)
				}
				value = minmax_alphabeta_pruning(depth + 1, alpha1, beta1);		// ���� �� Ž��
				if (best >= value) {
					best = value;
				}
				game_length_temp--;
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
				if (beta1 > best) {						// beta ���� �ٿ����� ��
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
			if (depth == 8 + (game_length_temp - 13) / 8) {				// �ð��� ���� ���� depth Ž�� ��ȭ
				return evaluate_board_state();							// ���� ������ ���� ������ ��ȯ���ش�. >0 �� ��� ������ ���� <0 �� ��� �İ��� ������ ��Ȳ(black-white�̹Ƿ�)
			}
		}
		return evaluate_board_state();
	}
	int i;
	int best = -1000000;
	if ((game_length + depth) % 2) best = 1000000;
	if ((game_length + depth) % 2 == 0) {				// ���� max���� ����
		best = -1000000;
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {			// �� �� �ִ� ĭ�� �� ���ƺ��µ�
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// �ش� ĭ�� ���̰� ����ؼ� ���� �� �ִٸ�

				HEIGHT_TEMP[i]++;						// �ӽù迭�� ���� �� ���� ���ٺ��� ����
				map_temp[HEIGHT_TEMP[i]][i] = 1;
				game_length_temp++;
				if (is_winning_temp(i)) {				// �ٵ� ���⿡ �θ� �̱��?
					game_length_temp--;						// �ӽù迭 ����
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return (21 - (game_length_temp + 1) / 2) * 10000;		// ��ŭ�� Ƚ���� �Ʋ��� ���ӿ��� �¸��ϴ���
				}
				value = minmax(depth + 1);				// �� �� ���� �� ��������
				if (best <= value) {					// �� �� �� ������ ���� �ٸ� ��ġ�� �� �� ���� ���� ����� ��´ٸ�
					best = value;						// ���� ����
				}
				game_length_temp--;						// �ӽù迭 ����
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
			}
		}
	}
	else {												// �İ� min���� ����
		for (i = 1; i <= MAP_WIDTH_SIZE; i++) {			// �� �� �ִ� ĭ�� �� ���ƺ��µ�
			if (HEIGHT_TEMP[i] < MAP_HEIGHT_SIZE) {		// �ش� ĭ�� ���̰� ����ؼ� ���� �� �ִٸ�
														/*				cout << "game_length : " << game_length << " game_length_temp : " << game_length_temp << " i : " << i << endl;
														print_current_map_temp();
														getchar();*/
				HEIGHT_TEMP[i]++;						// �ӽù迭�� �̿��ؼ� �� ���� ���ٺ� �� �ֵ��� ����
				map_temp[HEIGHT_TEMP[i]][i] = 2;
				game_length_temp++;
				if (is_winning_temp(i)) {				// �ٵ� ���⿡ �θ� �̱��?
					game_length_temp--;							// �ӽù迭 ����
					map_temp[HEIGHT_TEMP[i]][i] = 0;
					HEIGHT_TEMP[i]--;
					return -((21 - (game_length_temp + 1) / 2) * 10000);		// ��ŭ�� Ƚ���� �Ʋ��� ���ӿ��� �¸��ϴ���, �İ��� ���̹Ƿ� ����(-)�� ��ȯ(?)
				}
				value = minmax(depth + 1);				// �� �� �� �� ��������
				if (best >= value) {					// �ռ� �� ������ ���� �ٸ� ��ġ�� �� �ͺ��� ���� ����� ��´ٸ�(�İ��� ���� �������� ���� ���� ���� ����)
					best = value;
				}
				game_length_temp--;							// �ӽù迭 ����
				map_temp[HEIGHT_TEMP[i]][i] = 0;
				HEIGHT_TEMP[i]--;
			}
		}
	}
	return best;
}
int evaluate_vertically_temp(int column, int color) {		// evaluate_vertically ������ ������ Ȯ�����ش�
	int i, value = 0, cnt = 0;
	int v[5] = { 0,1,3,9,100 };								// heuristic value
	for (i = HEIGHT_TEMP[column]; i >= 1; i--) {
		if (map_temp[i][column] == color) cnt++;
		else break;
		if (cnt == 4) return v[4];
	}
	if (HEIGHT_TEMP[column] == MAP_HEIGHT_SIZE) return 0;	// ���ǹ�
	return v[cnt];
}

int evaluate_horizontally_temp(int row, int column, int color) {					// ���� ���� ���� Ž��
	int value = 0;
	int i;
	int cnt = 0;
	int nb = 0;
	int y, x;
	int v[5] = { 0,1,3,9,100 };
	y = row;
	x = column;

	for (;x <= MAP_WIDTH_SIZE && nb < 4; x++) {								// �ش��������� 4ĭ ���η� ���
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != 0) break;								// �ٸ� ���� �߰��� ������ count ����
		nb++;
	}
	bool left_block = false;												// ������ ���� ��������
	bool right_block = false;												// �������� ���� ��������
	if (cnt == 4) {															// four in a row!
		return v[4];
	}
	else if (nb<4) {															// 4ĭ���� �� ������ �����ִ� ���� ����� ����		
		if (column == 1) {													// ���� ������ ���� �ִٴ� ���� ������ �����ִٴ� ��
			left_block = true;
		}
		else {
			if (map_temp[row][column - 1] != color && map_temp[row][column - 1] != 0) {	// ���� ������ ������ ������ ���� �� �� ���� ���� �ٸ� ���� �ִٴ� �� ���� ������ ���� �ִٴ� ��
				left_block = true;
			}
		}
		if (x == MAP_WIDTH_SIZE + 1) {										// x�� ������ ���� �Ѿ�ٴ� ���� column>=4�� ��Ȳ������ �����ϰ�, ������ ������ ��� �̹Ƿ� ������ ���� �����ִ�.
			right_block = true;
		}
		else {
			if (map_temp[row][x] != color && map_temp[row][x] != 0) {		// x�� ���κ��̹Ƿ� �ٸ� ���� �����ִٴ� ��
				right_block = true;
			}
		}
		if (left_block && right_block) {									// ������ ������ �� �� ���������� �̰��� ������ ���� ����
//			cnt = 0;														// ���� �ʿ�
		}
	}
	return v[cnt];
}

int evaluate_diagonally_temp(int row, int column, int color) {				// �밢 Ž�� ����(������ �ƴµ� ȿ���� ����)

	int v[5] = { 0,1,3,9,100 };
	int value = 0;
	int cnt = 0;
	int nb = 0;
	int y, x;

	y = row;
	x = column;

	for (;y >= 1 && x >= 1 && nb < 4; y--, x--) {							// �� �밢��
		nb++;
		if (map_temp[y][x] == color) cnt++;
		else if (map_temp[y][x] != 0) break;
	}
	bool top_block = false;
	bool bottom_block = false;
	if (cnt == 4) {
		//		value += v[cnt];												// 4ĭ�� �������.
	}
	else {
		if (y == 0) bottom_block = true;										// �ؿ��� �����ִ�.
		else if (nb < 4) bottom_block = true;									// �ؿ��� �����ִ�.
		if (row == MAP_HEIGHT_SIZE) top_block = true;							// ������ �����ִ�.
		else {
			if (column != MAP_WIDTH_SIZE) {
				if (map_temp[row + 1][column + 1] != color && map_temp[row + 1][column + 1] != 0) {		// ������ �����ִ�.
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
	for (;y >= 1 && x <= MAP_WIDTH_SIZE && nb < 4; y--, x++) {								// �� �밢�� �ִ� 4ĭ Ž��
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

int evaluate_board_state() {					// color �� ������ + - �� �����ؼ� �̱�� ���°� ����ȭ ��ų �� ������? �ƴϸ� ȣ���� �κп��� �ٷ� �������ټ� �ִ°ǰ�?
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
			if (map_temp[i][j] == 1) {			// ������ �� ���� ���
				value_black += evaluate_diagonally_temp(i, j, 1);
			}
			else if (map_temp[i][j] == 2) {
				value_white += evaluate_diagonally_temp(i, j, 2);
			}
		}
	}
	return (value_black - value_white);
}