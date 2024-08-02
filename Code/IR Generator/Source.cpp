#include <iostream>
#include <fstream>
#include <iomanip>
#include "Intermediate.h"

using namespace std;

// Driver function
int main()
{
	Intermediate i{};
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

	i.generate(tokens);
}