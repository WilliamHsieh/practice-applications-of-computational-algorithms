#include <bits/stdc++.h>
using namespace std;

void print(vector<vector<int> > &board) {
	const int n = board.size();
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
}

bool is_valid(vector<vector<int> > &board, int x, int y, int v) {
	const int n = board.size();
	for (int i=0; i<n; i++) {
		if (i != y && v == board[x][i] || i != x && v == board[i][y]) {
			return false;
		}
	}

	int t = sqrt(n);
	for (int i = t * (x / t); i < t * (x / t + 1); i++) {
		for (int j = t * (y / t); j < t * (y / t + 1); j++) {
			if (x == i and y == j) continue;
			if (v == board[i][j]) return false;
		}
	}

	return true;
}

bool sol(vector<vector<int> > &board, int x, int y) {
	if (y == board.size()) x += 1, y = 0;
	if (x == board.size()) return true;
	if (board[x][y] != 0) return sol(board, x, y + 1);

	for (int i=1; i<=board.size(); i++) {
		if (is_valid(board, x, y, i)) {
			board[x][y] = i;
			if (sol(board, x, y + 1)) return true;
		}
		board[x][y] = 0;
	}
	return false;
}

int main() {
	vector<int> v;
	int x;
	while (cin >> x) {
		v.push_back(x);
	}

	const int n = sqrt(v.size());
	vector<vector<int> > board(n, vector<int>(n));

	for (int i=0, idx=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			board[i][j] = v[idx++];
		}
	}

	if (sol(board, 0, 0)) {
		print(board);
	} else {
		cout << "NO" << endl;
	}
}

