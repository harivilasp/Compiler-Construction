#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <stack>
#include <map>
#include <vector>
#include <sstream>

std::ofstream ParserOutput("Parser Output.txt");

// This class handles all actions related to parsing
class Parser {
private:
	// Map of action table
	std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> action;
	// Map of LHS -> RHS conversion in our grammar
	std::map<int, std::pair<std::string, int>> reduce;

	// Function to print a stack
	void printStack(std::stack<std::string> s) {
		ParserOutput << "[ ";
		while (!s.empty()) {
			ParserOutput << s.top() << ' ';
			s.pop();
		}
		ParserOutput << ']';
	}

	// Function to initialize action and reduce maps
	void init(std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>>& action, std::map<int, std::pair<std::string, int>>& reduce);
public:
	Parser() {
		init(action, reduce);
	}

	// Function to parse the tokens
	void parse(std::vector<std::string>& tokens);
};

void Parser::init(std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>>& action, std::map<int, std::pair<std::string, int>>& reduce) {

	//Build the LR Table

	std::ifstream LR("Parser Simplified.txt");
	std::string line;
	while (std::getline(LR, line)) {

		std::stringstream ss(line);
		std::string s;

		std::vector<std::string> v;

		while (std::getline(ss, s, ' ')) {
			v.push_back(s);
		}

		auto key = std::make_pair(v[0], v[1]);
		auto value = std::make_pair(v[2], v[3]);

		action.insert(make_pair(key, value));
	}
	LR.close();

	// Build the reduction rule table

	std::ifstream CFG("Parser CFG.txt");
	int no = 0;
	while (std::getline(CFG, line)) {
		std::stringstream ss(line);
		std::string s;
		std::getline(ss, s, ' ');

		std::string s2;
		std::getline(ss, s2, ' ');
		int count = 0;

		while (std::getline(ss, s2, ' ')) {
			count++;
		}
		reduce.insert(make_pair(no++, make_pair(s, count)));
	}
	CFG.close();
}

void Parser::parse(std::vector<std::string>& tokens) {
	std::stack<std::string> in, st;

	for (int i = 0; i < tokens.size(); i += 2) {
		std::string name = tokens[i];
		std::string type = tokens[i + 1];

		if (type == "id" ||
			type == "INTEGER_LITERAL" ||
			type == "STRING_LITERAL" ||
			type == "FLOAT_LITERAL" ||
			type == "BOOLEAN_LITERAL") {
			in.push(type);
		}
		else
			in.push(name);
	}

	st.push("$");
	st.push("0");
	bool stop = false;

	while (!stop) {
		auto k1 = st.top();
		auto k2 = in.top();

		ParserOutput << '\n';

		ParserOutput << "Stack: ";
		printStack(st);
		ParserOutput << '\n';

		ParserOutput << "Input: ";
		printStack(in);
		ParserOutput << '\n';

		if (action.find(std::make_pair(k1, k2)) != action.end()) {
			auto operation = action[std::make_pair(k1, k2)];

			ParserOutput << "Action: " << operation.first << ' ' << operation.second;

			if (operation.first == "shift") {
				in.pop();
				st.push(k2);
				st.push(operation.second);
			}
			else if (operation.first == "reduce") {

				int rule = std::stoi(operation.second);
				int count = reduce[rule].second;

				while (count--) {
					st.pop();
					st.pop();
				}

				std::string n = st.top();
				st.push(reduce[rule].first);

				// goto action
				auto temp = action[std::make_pair(n, reduce[rule].first)];
				ParserOutput << " & " << temp.first << ' ' << temp.second;
				st.push(temp.second);
			}
			else if (operation.first == "goto") {
				st.push(operation.second);
			}
			else if (operation.first == "acc") {
				stop = true;
			}

			ParserOutput << '\n';
		}
		else {
			ParserOutput << "\n\nERROR! Popping input stack\n\n";
			in.pop();
			if (in.empty()) {
				ParserOutput << "\n\nUNSOLVABLE ERROR!";
				ParserOutput.close();
				exit(0);
			}
		}
	}

	ParserOutput.close();
}

#endif