#include <iostream>
#include <fstream>
#include <iomanip>
#include "Parser.h"

using namespace std;

int main() {

	Parser p{};
	vector<string> tokens;
	ifstream tkns("tokens.txt");
	string line;

	getline(tkns, line);

	stringstream ss(line);
	string token;

	while (getline(ss, token, '#')) {
		if (token == "DEAD_END_KILL_ERROR") {
			tkns.close();
			exit(0);
		}
		tokens.push_back(token);
	}

	p.parse(tokens);

	return 0;
}