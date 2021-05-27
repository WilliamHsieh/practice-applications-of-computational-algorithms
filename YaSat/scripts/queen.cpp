#include <bits/stdc++.h>
using namespace std;

// #onehot
void onehot(vector<int> &clause, vector<vector<int>> &cnf){
	int N = clause.size();
	vector<int> dnf;

	cnf.push_back(clause);
	for (int j = 0; j < N-1; j++){
		for (int k = j+1; k < N; k++){
			dnf.push_back(-clause[j]);
			dnf.push_back(-clause[k]);
			cnf.push_back(dnf);
			dnf.clear();
		}
	}
	clause.clear();
}

// #AtMostOne
void AtMostOne(vector<int> &clause, vector<vector<int>> &cnf){
	int N = clause.size();
	vector<int> dnf;

	for (int j = 0; j < N-1; j++){
		for (int k = j+1; k < N; k++){
			dnf.push_back(-clause[j]);
			dnf.push_back(-clause[k]);
			cnf.push_back(dnf);
			dnf.clear();
		}
	}
	clause.clear();
}

// #main
int main (int argc, char * argv[]){
	if (argc != 3) {
		cout << "usage: ./queen board_size output.cnf\n";
		return 0;
	}

	int N = atoi(argv[1]);
	int num_vars = N * N;
	auto var = vector(N, vector(N, 0));
	for (int i = 0, k = 1; i < N; i++){
		for (int j = 0; j < N; j++){
			var[i][j] = k++;
		}
	}

	vector<vector<int>> cnf;
	vector<int> clause;

	// row
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			clause.push_back(var[i][j]);
		}
		onehot(clause, cnf);
	}

	// col
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			clause.push_back(var[j][i]);
		}
		onehot(clause, cnf);
	}

	// diagonal
	for (int i = 0; i < N-1; i++){
		for (int j = 0; j < N-i; j++){
			clause.push_back(var[j][i+j]);
		}
		AtMostOne(clause, cnf);
	}

	for (int i = 1; i < N-1; i++){
		for (int j = 0; j < N-i; j++){
			clause.push_back(var[j+i][j]);
		}
		AtMostOne(clause, cnf);
	}

	for (int i = 0; i < N-1; i++){
		for (int j = 0; j < N-i; j++){
			clause.push_back(var[j][N-1 - (i+j)]);
		}
		AtMostOne(clause, cnf);
	}

	for (int i = 1; i < N-1; i++){
		for (int j = 0; j < N-i; j++){
			clause.push_back(var[j+i][N-1-j]);
		}
		AtMostOne(clause, cnf);
	}

	// encode to sat
	auto fout = ofstream(argv[2]);
	fout << "p cnf " << num_vars << " " << cnf.size() << endl;

	for (size_t i = 0; i < cnf.size(); i++){
		for (size_t j = 0; j < cnf[i].size(); j++){
			fout << cnf[i][j] << " ";
		}
		fout << '0' << endl;
	}
	fout << endl;

	// solve with yasat
	auto cmd = "./yasat "s + argv[2];
	system(cmd.data());

	// decode
	auto fname = string(argv[2]);
	fname = fname.substr(0, fname.size() - 3) + "sat";
	auto fin = ifstream(fname.data());
	string buf;
	fin >> buf >> buf;

	if (buf == "SATISFIABLE") {
		fin >> buf;
		for (int i = 0; i < N; i++){
			for (int j = 0; j < N; j++){
				int tmp;
				fin >> tmp;
				cout << (tmp > 0) << ' ';
			}
			cout << endl;
		}
	}

	return 0;
}
