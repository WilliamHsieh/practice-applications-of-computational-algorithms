#include <bits/stdc++.h>
using namespace std;

// #Viterbi
struct Viterbi {

// ##constructor
	Viterbi(auto &&param) {
		tie(start_prob, transition, emission) = param;
		n_tran = transition.size();
		n_emis = emission.front().size();

		for (auto &x : start_prob) x = log(x == 0 ? DBL_MIN : x);
		for (auto &v : transition) for (auto &x : v) x = log(x == 0 ? DBL_MIN : x);
		for (auto &v : emission) for (auto &x : v) x = log(x == 0 ? DBL_MIN : x);
	}

// ##predict
	auto predict(vector<int> &observe) {
		auto dp = start_prob;

		// initial setup
		for (int i = 0; i < n_tran; i++) {
			dp[i] += emission[i][observe.front()];
		}

		// viterbi
		auto path = vector(n_tran, vector(observe.size(), -1));
		for (size_t i = 1; i < observe.size(); i++) {
			auto tmp = vector(n_tran, -DBL_MAX);
			for (int cur = 0; cur < n_tran; cur++) {
				for (int pre = 0; pre < n_tran; pre++) {
					if (auto M = dp[pre] + transition[pre][cur]; M > tmp[cur]) {
						tmp[cur] = M;
						path[cur][i] = pre;
					}
				}
				tmp[cur] += emission[cur][observe[i]];
			}
			dp.swap(tmp);
		}

		// generate the path
		auto result = vector<int>(observe.size());
		int val = max_element(dp.begin(), dp.end()) - dp.begin();
		for (int i = observe.size() - 1; i >= 0; i--) {
			result[i] = val;
			val = path[val][i];
		}

		return result;
	}

// ##member
	int n_tran;
	int n_emis;
	vector<double> start_prob;
	vector<vector<double>> transition;
	vector<vector<double>> emission;
};

// #HW3 data
struct HW3 {

// ##constructor
	HW3() {
		start_prob = vector{0.5, 0.25, 0.25};

		transition = vector {
			vector{0.8, 0.15, 0.05},
			vector{0.2, 0.5, 0.3},
			vector{0.2, 0.2, 0.6}
		};

		emission = vector {
			vector{0.9, 0.1},
			vector{0.7, 0.3},
			vector{0.2, 0.8}
		};

		weather = {"sunny"s, "foggy"s, "rainy"s};
	}

// ##param
	auto param() {
		return tie(start_prob, transition, emission);
	}

// ##operator>>
	friend istream& operator>> (istream &is, HW3 &hw) {
		int n;
		is >> n >> ws;
		hw.state.resize(n);
		hw.observe.resize(n);

		auto mapping = map<string, int> {
			{"sunny"s, 0},
			{"foggy"s, 1},
			{"rainy"s, 2},
		};

		for (int i = 0; i < n; i++) {
			string buf;
			getline(is, buf, ',');
			hw.state[i] = mapping[buf];

			getline(is, buf);
			hw.observe[i] = (buf == "yes");
		}

		return is;
	}

// ##operator<<
	friend ostream& operator<< (ostream &os, HW3 &hw) {
		auto acc = .0;
		for (int i = hw.state.size() - 1; i >= 0; i--) {
			acc += (hw.state[i] == hw.result[i]);
		}

		os << acc / hw.state.size() << '\n';
		for (auto &x : hw.result) {
			os << hw.weather[x] << '\n';
		}

		return os;
	}

// ##member
	vector<double> start_prob;
	vector<vector<double>> transition;
	vector<vector<double>> emission;
	vector<string> weather;

	vector<int> state;
	vector<int> observe;
	vector<int> result;
};

// #Main function
int main(int argc, char **argv) {
	if (argc != 3) {
		cerr << "Usage: ./viterbi infile outfile\n";
		return 0;
	}

	auto is = ifstream(argv[1]);
	auto hw = HW3{};
	is >> hw;

	auto os = ofstream(argv[2]);
	hw.result = Viterbi(hw.param()).predict(hw.observe);
	os << hw;

	return 0;
}
