#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace EsoInterpreter
{
	struct Token
	{
		std::string type;
		std::string value;
	};

	struct Node
	{
		Token token;
		std::vector<Node*> nodes;
	};

	class SyntaxTree
	{
	private:
		const char kBinOperators[5] = { '*', '/', '+', '-', '%' };
		const std::string kCompOperators[6] = { "==", "!=", "<", "<=", ">", ">=" };
		const char kConstants[10] = {
			'0', '1', '2', '3', '4',
			'5', '6', '7', '8', '9'
		};
		const std::string kLogicOperators[4] = { "if", "elif", "else", "loop" };

		Node* root_;

		void ParseLogicBlock(std::string& block, Node* current_parent, std::string logic_type);
		void ParseComparison(std::string line, Node* current_parent);
		void ParsePrint(std::string line, Node* current_parent);
		void ParseAssignment(std::string line, Node* current_parent);
		void ParseBinOperation(std::string& line, Node* current_parent, long long& offset);
		std::string ParseConstant(std::string line, long long& offset);

		bool CharIsConst(char c);
		bool CharIsVar(char c);
		bool LineContainsBinOperator(std::string line);
		bool LineContainsCompOperator(std::string line);
		bool LineStartsWithLogicOperator(std::string line);

		std::string ToStr(int lvl, Node* current_parent);
	public:
		SyntaxTree(std::string raw_code);
		Node* GetRoot();
		void Print();
	};
}