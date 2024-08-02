#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "Tokenizer.h"

using namespace std;

ofstream TKOutput("TKOutput.txt");
ofstream tkns("tokens.txt");

int main(int argc, char* argv[]) {

	string codeName;

	if (argc > 1 && *argv[1] == 'i')
		codeName = "IRCode.respec";
	else
		codeName = "ProgramCode.respec";

	Tokenizer tz(codeName);

	tuple<int, int, string, string> token;
	vector<string> tokens;

	while (tz.hasMoreTokens()) {

		bool ignore = false;
		token = tz.getNextToken();

		for (string str : data::ignoreList) {
			if (str == get<2>(token)) {
				ignore = true;
				break;
			}
		}

		if (ignore)
			continue;

		if (get<2>(token) == "IDENTIFIER") {
			for (string str : data::keywords) {
				if (str == get<3>(token)) {
					get<2>(token) = "KEYWORD";
					break;
				}
			}
		}

		auto type = get<2>(token);

		if (type == "IDENTIFIER")
			type = "id";
		else if (type == "INTEGER LITERAL")
			type = "INTEGER_LITERAL";
		else if (type == "STRING LITERAL")
			type = "STRING_LITERAL";
		else if (type == "BOOLEAN LITERAL")
			type = "BOOLEAN_LITERAL";
		else if (type == "FLOAT LITERAL")
			type = "FLOAT_LITERAL";

		string print_col = "[" + to_string(get<1>(token)) + "]";

		if (!ignore) {
			tokens.push_back(type);
			tokens.push_back(get<3>(token));
		}

		if (!ignore) {
			TKOutput << left
				<< "Line " << setw(3) << right << get<0>(token) << left
				<< setw(4) << print_col << '\t'
				<< setw(15) << get<2>(token) << " : " << get<3>(token) << endl;
		}

		if (get<2>(token) == "UNEXPECTED TOKEN") {
			tkns << "DEAD_END_KILL_ERROR";
			TKOutput.close();
			tkns.close();
			exit(0);
		}
	}

	tokens.push_back("END");
	tokens.push_back("$");
	reverse(tokens.begin(), tokens.end());

	for (auto x : tokens)
		tkns << x << '#';

	TKOutput.close();
	tkns.close();
	return 0;
}