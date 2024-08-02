#ifndef DATA_H
#define	DATA_H

#include <unordered_map>

namespace data {
	namespace expressions {
		std::vector<std::pair<std::string, std::string>> primary;
	}

	std::vector<std::string> keywords;
	std::vector<std::string> ignoreList;

	// Function to initialize the regular expressions and keywords
	void init_data() {
		expressions::primary.push_back(std::make_pair("SINGLE LINE COMMENT", "^//.*"));
		expressions::primary.push_back(std::make_pair("MULTI LINE COMMENT", "/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/"));
		expressions::primary.push_back(std::make_pair("WHITESPACE", "^\\s+"));
		expressions::primary.push_back(std::make_pair("DELIMITER", "^(\\{|\\}|\\(|\\)|\\[|\\]|;|,)"));
		expressions::primary.push_back(std::make_pair("BOOLEAN LITERAL", "^(false|true)"));
		expressions::primary.push_back(std::make_pair("IDENTIFIER", "^[_a-zA-Z]+[_a-zA-Z0-9]*"));
		expressions::primary.push_back(std::make_pair("OPERATOR", "^(\\+\\+|\\+|-|\\-\\-|\\*|/|%|==|=|>=|<=|>|<|!=|&&|\\|\\|)"));
		expressions::primary.push_back(std::make_pair("FLOAT LITERAL", "^[+-]?([0-9]*[.])[0-9]+"));
		expressions::primary.push_back(std::make_pair("INTEGER LITERAL", "^[+-]?[0-9]+"));
		expressions::primary.push_back(std::make_pair("STRING LITERAL", "^\"[^\"]*\""));

		ignoreList.push_back("WHITESPACE");
		ignoreList.push_back("SINGLE LINE COMMENT");
		ignoreList.push_back("MULTI LINE COMMENT");

		keywords.push_back("main");
		keywords.push_back("call");
		keywords.push_back("int");
		keywords.push_back("float");
		keywords.push_back("bool");
		keywords.push_back("string");
		keywords.push_back("array");
		keywords.push_back("while");
		keywords.push_back("if");
		keywords.push_back("else");
		keywords.push_back("return");
		keywords.push_back("break");
		keywords.push_back("continue");
	}
}

#endif