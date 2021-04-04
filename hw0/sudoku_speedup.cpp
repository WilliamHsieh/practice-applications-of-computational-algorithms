#include <bits/stdc++.h>
using namespace std;

int N, n;
bitset<30> row[30], col[30], blk[30];

// #Print the result
void print(vector<vector<int> > &board, vector<int> &rand_map) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k <= N; k++) if (rand_map[k] == board[i][j]) {
				cout << k << " \n"[j == N - 1];
				break;
			}
		}
	}
}

// #Set mask && Check if pos(x, y) is valid
inline int block_idx(int x, int y) {
	return n * (x / n) + (y / n);
}

inline void set_mask(int num, int x, int y, int mask) {
	row[x][num] = mask;
	col[y][num] = mask;
	blk[block_idx(x, y)][num] = mask;
}

inline bool is_valid(int x, int y, int guess) {
	return row[x][guess] & col[y][guess] & blk[block_idx(x, y)][guess];
}

// #Solve the sudoku
bool sol(vector<vector<int> > &board, int x, int y) {
	if (y == N) x += 1, y = 0;
	if (x == N) return true;
	if (board[x][y] != 0) return sol(board, x, y + 1);

	for (int guess = 1; guess <= N; guess++) {
		if (is_valid(x, y, guess)) {
			board[x][y] = guess;
			set_mask(guess, x, y, 0);
			if (sol(board, x, y + 1)) return true;
			board[x][y] = 0;
			set_mask(guess, x, y, 1);
		}
	}
	return false;
}

// #Main function
int main() {
	// input sudoku board
	vector<int> v;
	for (int x; cin >> x; v.push_back(x));

	// initial bit mask
	N = sqrt(v.size());
	n = sqrt(N);
	for (int i = 0; i < N; i++) row[i].set(), col[i].set(), blk[i].set();

	// randomly assign testcase numbers
	srand(time(0));
	vector<int> rand_map(N + 1);
	for (int i = 0; i <= N; i++) rand_map[i] = i;
	random_shuffle(rand_map.begin() + 1, rand_map.end());

	// swap testcase numbers
	vector<vector<int> > board(N, vector<int>(N));
	for (int i = 0, idx = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			board[i][j] = rand_map[v[idx++]];
			if (board[i][j] != 0) set_mask(board[i][j], i, j, 0);
		}
	}

	// solve
	if (sol(board, 0, 0)) {
		print(board, rand_map);
	} else {
		cout << "NO" << endl;
	}
}

