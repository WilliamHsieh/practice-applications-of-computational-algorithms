#include <bits/stdc++.h>
using namespace std;

double sol(string &buf, int L, int R, double p) {
	if (R - L == 1) {
		return buf[L] - '0';
	}

	int cnt = 0, idx = -1;
	for (int i = L + 3; i < R; i++) {
		if (buf[i] == '(') {
			cnt++;
		} else if (buf[i] == ')') {
			cnt--;
		} else if (buf[i] == ',' and cnt == 0 and idx == -1) {
			idx = i;
		}
	}

	double l = sol(buf, L + 3, idx, p);
	double r = sol(buf, idx + 1, R - 1, p);

	return l * (1 - p) + r * p;
}

int main() {
	cin.tie(0) -> sync_with_stdio(0);

	double p;
	string buf;
	while (cin >> p >> buf) {
		if (p == 0 and buf == "0") break;
		cout << fixed << setprecision(6) << sol(buf, 0, buf.size(), p) << '\n';
	}
}
