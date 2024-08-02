#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <regex>
#include "Data.h"

// Handles all actions related to the function of the lexer
class Tokenizer {

private:
	std::string program;
	unsigned int cursor = 0;
	unsigned int lineNumber = 1;
	unsigned int columnNumber = 1;

	// Function to convert source code file to a string
	std::string fileToString(const std::string& path) {

		std::ifstream input_file(path);

		if (!input_file.is_open()) {
			std::cerr << "Could not open the file - '"
				<< path << "'" << std::endl;
			exit(EXIT_FAILURE);
		}

		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}

public:
	Tokenizer() {
		this->program = "";
		this->cursor = 0;
		data::init_data();
	}

	Tokenizer(const std::string& path) {
		this->program = fileToString(path);
		this->cursor = 0;
		data::init_data();
	}

	void setProgram(const std::string path) {
		this->program = fileToString(path);
	}

	bool hasMoreTokens() {
		return this->cursor < this->program.length();
	}

	std::tuple<int, int, std::string, std::string> getNextToken();
};

std::tuple<int, int, std::string, std::string> Tokenizer::getNextToken() {

	if (!this->hasMoreTokens())
		return std::make_tuple(lineNumber, columnNumber, "NULL", "");

	std::string sliced = this->program.substr(this->cursor);

	std::regex reg_ex;
	std::smatch match;

	for (auto& exp : data::expressions::primary) {

		reg_ex.assign(exp.second);

		if (regex_search(sliced, match, reg_ex, std::regex_constants::match_continuous)) {

			std::string type = exp.first;

			auto return_value = make_tuple(lineNumber, columnNumber, type, match[0]);
			this->cursor += match[0].length();

			for (int i = 0; i < match[0].length(); i++) {
				if (static_cast<std::string>(match[0]).at(i) == '\n') {
					lineNumber++;
					columnNumber = 1;
				}
				else
					columnNumber++;
			}

			return return_value;
		}
	}

	sliced = sliced[0];
	return std::make_tuple(lineNumber, columnNumber, "UNEXPECTED TOKEN", sliced);
}

#endif