#pragma once

struct State {
	// constructor
	State(int, int);

	// function
	std::optional<bool>& var(int);
	friend std::ostream& operator<< (std::ostream&, State&);

	// member
	std::vector<std::optional<bool>> guess; //decision of each variable
	std::vector<std::array<int, 2>> watch; //record the index of watched literal
};
