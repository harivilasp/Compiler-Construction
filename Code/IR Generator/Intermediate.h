#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include <iostream>
#include <fstream>
#include <stack>
#include <map>
#include <vector>
#include <sstream>

std::ofstream IntermediateOutput("IROutput.txt");

// Struct to store the .code and .name parts of IR Generation
struct SddObj {
	std::string in_name;
	std::string temp;
	std::string code;

	SddObj() {
		in_name = "";
		temp = "";
		code = "";
	}

	SddObj(std::string t1, std::string t2) {
		in_name = "";
		temp = t1;
		code = t2;
	}
};

// Class to handle all actions related to IR code generation
class Intermediate {
private:
	// Map to store the action table
	std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>> action;
	// Map to store the RHS -> LHS reduction rules
	std::map<int, std::pair<std::string, int>> reduce;

	// Function to generate a new temporary name
	std::string newTemp() {
		static int n = 1;
		return "t" + std::to_string(n++);
	}
	// Function to generate a new temporary label
	std::string newLabel() {
		static int n = 1;
		return "L" + std::to_string(n++);
	}

	//Functions to print a stack
	void printStack(std::stack<std::string> s) {
		std::cout << "[ ";
		while (!s.empty()) {
			std::cout << s.top() << ' ';
			s.pop();
		}
		std::cout << ']';
	}
	void printStack(std::stack<std::pair<std::string, std::string>> s) {
		std::cout << "[ ";
		while (!s.empty()) {
			std::cout << s.top().first << ' ';
			s.pop();
		}
		std::cout << ']';
	}

	// Function to initialize the action and reduce maps
	void init(std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>>& action, std::map<int, std::pair<std::string, int>>& reduce);
public:
	Intermediate() {
		init(action, reduce);
	}

	// Function to generate the IR code
	void generate(std::vector<std::string>& tokens);
};

void Intermediate::init(std::map<std::pair<std::string, std::string>, std::pair<std::string, std::string>>& action, std::map<int, std::pair<std::string, int>>& reduce) {

	//Build the LR Table

	std::ifstream LR("IR Simplified.txt");
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

	std::ifstream CFG("IR CFG.txt");
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

void Intermediate::generate(std::vector<std::string>& tokens) {
	std::stack<std::string>st;
	std::stack<std::pair<std::string, std::string>> in;
	std::stack<SddObj> ir;

	for (int i = 0; i < tokens.size(); i += 2) {
		std::string name = tokens[i];
		std::string type = tokens[i + 1];

		if (type == "id" ||
			type == "INTEGER_LITERAL" ||
			type == "STRING_LITERAL" ||
			type == "FLOAT_LITERAL" ||
			type == "BOOLEAN_LITERAL") {
			in.push(std::make_pair(type, name));
		}
		else
			in.push(std::make_pair(name, type));
	}

	st.push("$");
	st.push("0");

	bool stop = false;

	while (!stop) {
		auto k1 = st.top();
		auto k2 = in.top().first;

		if (action.find(std::make_pair(k1, k2)) != action.end()) {
			auto operation = action[std::make_pair(k1, k2)];

			if (operation.first == "shift") {

				SddObj t;
				t.in_name = k2;
				t.temp = in.top().second;
				ir.push(t);

				in.pop();
				st.push(k2);
				st.push(operation.second);
			}
			else if (operation.first == "reduce") {

				int rule = std::stoi(operation.second);

				if (rule == 25) {
					auto id = ir.top();
					ir.pop();
					SddObj E;
					E.temp = id.temp;
					E.code = "";
					ir.push(E);
				}
				else if (rule == 4) {
					ir.pop();
					auto E = ir.top();
					ir.pop();
					ir.pop();
					auto t = ir.top();
					ir.pop();

					SddObj S;
					S.code = E.code + t.temp + " = " + E.temp + '\n';
					ir.push(S);
				}
				else if (rule == 21) {
					ir.pop();
					auto E2 = ir.top();
					ir.pop();
					auto op = ir.top();
					ir.pop();
					auto E1 = ir.top();
					ir.pop();
					ir.pop();

					SddObj E;
					E.temp = newTemp();
					E.code = E1.code + E2.code + E.temp + " = " + E1.temp + " " + op.in_name + " " + E2.temp + "\n";
					ir.push(E);
				}
				else if (rule == 22) {
					ir.pop();
					auto E1 = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();

					SddObj E;
					E.temp = newTemp();
					E.code = E1.code + E.temp + " = -" + E1.temp + '\n';
					ir.push(E);
				}
				else if (rule == 23) {
					ir.pop();
					auto id2 = ir.top();
					ir.pop();
					auto op = ir.top();
					ir.pop();
					auto id1 = ir.top();
					ir.pop();
					ir.pop();

					SddObj E;
					std::string l1, l2;

					l1 = newLabel();
					l2 = newLabel();

					E.temp = newTemp();
					E.code += id1.code + id2.code;
					E.code += "if " + id1.temp + " " + op.in_name + " " + id2.temp + " goto " + l1 + "\n";
					E.code += E.temp + " = 0\n";
					E.code += "goto " + l2 + "\n";
					E.code += l1 + ":\n";
					E.code += E.temp + " = 1\n";
					E.code += l2 + ":\n";

					ir.push(E);
				}
				else if (rule == 30) {
					ir.pop();
					auto S = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();
					auto E = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();

					SddObj T;
					std::string l1 = newLabel();
					T.code = E.code;
					T.code += "if " + E.temp + " == 0 goto " + l1 + '\n';
					T.code += S.code;
					T.code += l1 + ":";

					ir.push(T);
				}
				else if (rule == 31) {
					ir.pop();
					auto S2 = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();
					ir.pop();
					auto S1 = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();
					auto E = ir.top();
					ir.pop();
					ir.pop();
					ir.pop();

					SddObj T;
					std::string l1 = newLabel();
					std::string l2 = newLabel();

					T.code = E.code;
					T.code += "if " + E.temp + " == 0 goto " + l1 + '\n';
					T.code += S1.code;
					T.code += "goto " + l2 + '\n';
					T.code += l1 + ":\n";
					T.code += S2.code;
					T.code += l2 + ":";

					ir.push(T);
				}
				else if (rule == 32) {
					auto S2 = ir.top();
					ir.pop();
					auto S1 = ir.top();
					ir.pop();

					SddObj T;
					T.code += S1.code + S2.code;
					ir.push(T);
				}

				int count = reduce[rule].second;

				while (count--) {
					st.pop();
					st.pop();
				}

				std::string n = st.top();
				st.push(reduce[rule].first);

				// goto action
				auto temp = action[std::make_pair(n, reduce[rule].first)];
				st.push(temp.second);
			}
			else if (operation.first == "goto") {
				st.push(operation.second);
			}
			else if (operation.first == "acc") {
				stop = true;
			}
		}
		else {
			IntermediateOutput << "\n\nERROR! Popping input stack\n\n";
			// exit(0);
			in.pop();

			if (in.empty()) {
				IntermediateOutput << "\n\nUNSOLVABLE ERROR!";
				IntermediateOutput.close();
				exit(0);
			}
		}
	}

	IntermediateOutput << "IR Code:\n\n";

	while (ir.top().code.length() == 0)
		ir.pop();
	IntermediateOutput << ir.top().code;
}

#endif