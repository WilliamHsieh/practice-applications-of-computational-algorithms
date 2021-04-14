#include <bits/stdc++.h>
#include "parser.h"
#define all(x) begin(x),end(x)
#define what_is(x) cerr << "[what] " << #x << " is " << x << endl
#define exec(x) cerr << "[exec] " << #x << endl; x

using namespace std;

// #Global
auto num_vars		= 0;
auto num_clauses	= 0;
auto clauses		= std::vector<std::vector<int>>{};
auto prop_q			= queue<int>{};

// #Curent state
struct State {
	State() :
		guess(num_vars + 1), watch(num_clauses, {0, 1})
	{
		guess[0] = true;
	}

	optional<bool>& var(int x) {
		return guess[abs(x)];
	}

	friend ostream& operator<< (ostream &os, State &state) {
		// show current variable
		int idx = 0;
		for (auto &x : state.guess) {
			os << "[" << idx++ << ": ";
			if (!x) {
				os << "no";
			} else {
				os << boolalpha << *x;
			}
			os << "] ";
		}
		os << endl;

		// show watched literal
		for (int i = 0; i < num_clauses; i++) {
			auto &[la, lb] = state.watch[i];
			os << '[' << i << "] " << la << ' ' << lb << endl;
		}

//		getchar();
		return os;
	}

	vector<optional<bool>> guess; //decision of each variable
	vector<array<int, 2>> watch; //record the index of watched literal
};

// #Helper
std::ostream& operator<< (std::ostream& os, const std::vector<int>& v) {
	for (size_t i = 0, s = v.size(); i < s; i++) {
		if (i != 0) os << " ";
		os << v[i];
	}
	return os;
}

// #Pick and set a variable
int pick_variable(State &state) {
	static auto gen = std::default_random_engine(std::random_device{}());
	static auto dis = std::uniform_int_distribution<int>(1, num_vars);

	int x;
	while (true) {
		x = dis(gen);
		if (!state.guess[x]) {
			return x;
		}
	}

//	for (int i = 1; i <= num_vars; i++) {
//		if (!state.guess[i]) {
//			return i;
//		}
//	}

//	assert(false and "shouldn't reach here!");
	return -1;
}

void set_variable(State &state, int x) {
	state.var(x) = (x > 0);
	prop_q.push(-x);
}

// #Initial all structure
auto init() {
	sort(all(clauses), [](const vector<int> &v1, const vector<int> &v2) {
		auto x1 = (v1.size() == 1 ? LLONG_MAX : v1.size());
		auto x2 = (v2.size() == 1 ? LLONG_MAX : v2.size());
		return x1 < x2;
//TODO: why wrong?
//		if (v1.size() == 1 and v2.size() == 1) return true;
//		if (v2.size() == 1) return true;
//		if (v1.size() == 1) return false;
//		return v1.size() < v2.size();
	});
	auto state = State{};

	// assign unit clauses as decision
	for (size_t i = 0; i < clauses.size(); i++) {
		if (clauses[i].size() == 1) {
			state.watch[i] = {-1, -1};
			set_variable(state, clauses[i][0]);
		}
	}

	return state;
}

// #Unit propagate
bool unit_propagate(State &state) {
	while (!prop_q.empty()) {
		int false_literal = prop_q.front(); prop_q.pop();
//		what_is(false_literal);

		for (int i = 0; i < num_clauses; i++) {
			if (clauses[i].size() == 1) break;
			auto watch_not_false = [&](int &self, int &other) {
				if (self == -1) return;
				// already watching at not false literal
//				cout << "col: " << i << ", watching: " << self << ", val: " << clauses[i][self] << endl;
				if (clauses[i][self] != false_literal) return;

				for (size_t j = max(self, other) + 1; j < clauses[i].size(); j++) {
					auto &var = state.var(clauses[i][j]);
					if (!var.has_value()) {
						// not false
						self = j;
						return;
					} else if (*var == (clauses[i][j] > 0)) {
						// true
						self = other = -1;
						return;
					}
				}

				self = -1;
			};

			// this clause is satisfied
			auto &[la, lb] = state.watch[i];
			if (la == -1 and lb == -1) continue;

			// one of the watched literal is already true
			auto watch_is_true = [&](int watched) {
				auto &var = state.var(clauses[i][watched]);
				if (var.has_value() and *var == (clauses[i][watched] > 0)) {
					return true;
				}
				return false;
			};
			if (watch_is_true(la) or watch_is_true(lb)) {
				la = lb = -1;
//				cout << "[col: " << i << "] already true\n" << state;
				continue;
			}

			// watch literals that are not false
			watch_not_false(la, lb);
			watch_not_false(lb, la);

			// check if this became unit clause (implication)
			if (la == -1) swap(la, lb);
			if (la != -1 and lb == -1) {
				if (state.var(clauses[i][la]).has_value()) {
					if (*state.var(clauses[i][la]) != (clauses[i][la] > 0)) {
						// conflict
						prop_q = {};
						return false;
					}
				}
				set_variable(state, clauses[i][la]);
				la = -1;
			}
#ifdef debug
			assert((la == -1) == (lb == -1));
			cout << "[col: " << i << "]\n" << state;
#endif
		}
	}
	return true;
}

// #DPLL
optional<vector<int>> dpll() {
	// 0. init
	auto decision = stack<State>{};
	auto branch = stack<int>{};
	decision.emplace(init());
	branch.push(0);

	while (!decision.empty()) {
		auto &state = decision.top();
		cout << "decision level: " << decision.size() << '\n';

		// 1. unit propagate
		if (!unit_propagate(state)) {
			cout << "propagation failed\n";
			while (!branch.empty() and branch.top() == 0) {
				decision.pop();
				branch.pop();
			}
			if (decision.empty()) return nullopt; //UNSAT

			// backtrack
			auto tmp = std::move(decision.top()); //retain the memory
			decision.pop();
			tmp = decision.top(); //copy previous state
			decision.emplace(std::move(tmp));
			set_variable(decision.top(), branch.top());
			branch.top() = 0;
			continue;
		}

		// 2. check if curent state is satisfied already
		auto it = find_if(all(state.watch), [](array<int, 2> &watched) {
			return pair(watched[0], watched[1]) != pair(-1, -1);
		});
		if (it == state.watch.end()) break;

		// 3. if not, apply new decision
		decision.push(decision.top());	//copy from previous state
		int g = pick_variable(decision.top());
		set_variable(decision.top(), g);
		branch.push(-g);
	}

	// 4. SAT: return ans;
	auto res = vector<int>(num_vars);
	auto &state = decision.top();
	for (int i = 1; i <= num_vars; i++) {
		if (state.guess[i].has_value() and *state.guess[i]) {
			res[i-1] = i;
		} else {
			res[i-1] = -i;
		}
	}
	return res;
}

// #Main
int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "usage: ./yasat [input.cnf]" << endl;
		return 0;
	}

	// parse DIMACS cnf
	auto fin_name = string(argv[1]);
	parse_DIMACS_CNF(clauses, num_vars, fin_name.data());
	num_clauses = clauses.size();

	// solve
	auto fout_name = fin_name.substr(0, fin_name.size() - 3) + "sat";
	auto fout = fstream(fout_name.data(), ios::out | ios::trunc);
	if (auto res = dpll(); res) {
		fout << "s SATISFIABLE\n";
		fout << "v " << *res << " 0";
	} else {
		fout << "s UNSATISFIABLE\n";
	}

	return 0;
}
