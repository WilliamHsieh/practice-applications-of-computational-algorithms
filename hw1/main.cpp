#include <bits/stdc++.h>
#define MAX_BOARD_SIZE 100

// TODO: don't use global variable
int N, n;
std::bitset<MAX_BOARD_SIZE> row[MAX_BOARD_SIZE], col[MAX_BOARD_SIZE], box[MAX_BOARD_SIZE];

// #Set mask && is_valid
inline int block_idx(int x, int y) {
	return n * (x / n) + (y / n);
}

inline void set_mask(int num, int x, int y, int mask) {
	row[x][num] = mask;
	col[y][num] = mask;
	box[block_idx(x, y)][num] = mask;
}

inline bool is_valid(int x, int y, int guess) {
	return row[x][guess] & col[y][guess] & box[block_idx(x, y)][guess];
}

// #Helper function
std::ostream& operator<< (std::ostream& os, const std::vector<int>& v) {
	for (size_t i = 0, s = v.size(); i < s; i++) {
		if (i != 0) os << " ";
		os << v[i];
	}
	return os;
}

// transfer cnf variable to index(r, c, d)
auto to_idx (int x) {
	x -= 1;
	int rc = x / N;
	return std::tuple {rc / N, rc % N, x % N + 1};
}

// transfer index(r, c, d) to cnf variable
auto to_var (int r, int c, int d) {
	return (r * N + c) * N + d;
	// 1 : 0, 0, 1
	// 9 : 0, 0, 9
	// 10: 0, 1, 1
}

// exactly-once encoding (x_idx, y_idx, vector of element)
void to_clause (auto& cnf, auto& v) {
	int s = v.size();
	if (s == 0) return;

	// at most one
	for (int i = 0; i < s; i++) {
		for (int j = i + 1; j < s; j++) {
			cnf.push_back({-v[i], -v[j]});
		}
	}

	// at least one
	cnf.emplace_back(std::move(v));
}

// #Encode (from sudoku to SAT)
void encode (auto& board) {
	auto cnf = std::vector<std::vector<int>>{};

	// generate clause for (row, col, box, single block)
	auto cand = std::vector<int>{};
	for (int i = 0; i < N; i++) {
		for (int num = 1; num <= N; num++) {
			// row
			for (int j = 0; j < N; j++) {
				cand.push_back(to_var(i, j, num));
			}
			to_clause(cnf, cand);

			// col
			for (int j = 0; j < N; j++) {
				cand.push_back(to_var(j, i, num));
			}
			to_clause(cnf, cand);

			// box
			for (int j = i / n * n; j < (i / n + 1) * n; j++) {
				for (int k = i % n * n; k < (i % n + 1) * n; k++) {
					cand.push_back(to_var(j, k, num));
				}
			}
			to_clause(cnf, cand);
		}

		// single block
		for (int j = 0; j < N; j++) {
			for (int num = 1; num <= N; num++) {
				if (board[i][j] == 0 and is_valid(i, j, num)) {
					cand.push_back(to_var(i, j, num));
				} else if (board[i][j] != 0) {
					cnf.push_back({to_var(i, j, num) * (num == board[i][j] ? 1 : -1)});
				}
			}
			to_clause(cnf, cand);
		}
	}

	// encoding done (SAT input)
	auto fout = std::fstream("sat.in", std::ios::out | std::ios::trunc);
	fout << "p cnf " << pow(N, 3) << " " << cnf.size() << "\n";
	for (auto& v : cnf) {
		fout << v << " 0\n";
	}
	fout.close();
}

// #Decode (from SAT to sudoku)
bool decode (auto& board) {
	auto fin = std::fstream("sat.out", std::ios::in);
	auto buf = std::string{};
	std::getline(fin, buf);
	if (buf == "UNSAT") return false;
	std::getline(fin, buf);

	// decode variables back to sudoku
	auto ss = std::stringstream{buf};
	for (int x; ss >> x and x != 0; ) {
		if (x < 0) continue;
		auto [r, c, d] = to_idx(x);
		board[r][c] = d;
	}

	fin.close();
	return true;
}

// #Main function
int main (int argc, char** argv) {
	if (argc != 4) {
		std::cout << "usage: ./solver [sudoku.in] [sudoku.out] [MiniSatExe]\n";
		return 0;
	}

	// input sudoku digits
	auto fin = std::fstream(argv[1], std::ios::in);
	auto tmp = std::vector<int>{};
	for (int x; fin >> x; tmp.push_back(x));
	fin.close();

	// initial bit mask
	N = sqrt(tmp.size());
	n = sqrt(N);
	for (int i = 0; i < N; i++) row[i].set(), col[i].set(), box[i].set();

	// set mask && build sudoku board
	auto board = std::vector(N, std::vector<int>(N));
	for (int i = 0, idx = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			board[i][j] = tmp[idx++];
			if (board[i][j] != 0) set_mask(board[i][j], i, j, 0);
		}
	}

	// encode && solve
	encode(board);
	std::system("./MiniSat_v1.14_linux sat.in sat.out");

	// decode && output sudoku answer
	if (!decode(board)) {
		std::cout << "NO\n";
	} else {
		auto fout = std::fstream(argv[2], std::ios::out | std::ios::trunc);
		for (auto& v : board) {
			fout << v << "\n";
		}
		fout.close();
	}

	return 0;
}

