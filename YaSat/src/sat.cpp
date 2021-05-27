#include <bits/stdc++.h>
#include "SatSolver.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cout << "usage: ./yasat input.cnf [output.sat]\n";
		return 0;
	}
	auto fcnf = std::string(argv[1]);
	auto fsat = (argc == 3) ? std::string(argv[2]) : fcnf.substr(0, fcnf.size() - 3) + "sat";

	// solve
	auto result = SatSolver(fcnf).solve();
	auto fout = std::ofstream(fsat.data());
	fout << result;

	return 0;
}
