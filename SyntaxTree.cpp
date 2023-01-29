#include <sstream>

#include "SyntaxTree.h"

namespace EsoInterpreter
{
	SyntaxTree::SyntaxTree(std::string raw_code)
	{
		root_ = new Node();
		root_->token.type = "START";
		
		std::istringstream iss(raw_code);
		bool block_finished = false;
		int depth = 0;
		std::vector<std::string> code_blocks;
		std::string block;
		
		for (std::string line; std::getline(iss, line); )
		{
			if (LineStartsWithLogicOperator(line)) depth++;
			if (line.find('$') == 0) depth--;
			if (depth == 0) block_finished = true;

			block += line + "\n";

			if (block_finished)
			{
				block;
				block_finished = false;
				block = block.substr(0, block.size() - 1);
				code_blocks.push_back(block);
				block = "";
			}
		}

		std::string prev_block;
		for (std::string block : code_blocks)
		{
			prev_block = block;
			if (block.rfind("loop", 0) != std::string::npos)
			{
				ParseLogicBlock(block, root_, "loop");
			}
			else if (block.rfind("elif", 0) != std::string::npos && prev_block[prev_block.length() - 1] == '$')
			{
				ParseLogicBlock(block, root_, "elif");
			}
			else if (block.rfind("if") != std::string::npos)
			{
				ParseLogicBlock(block, root_, "if");
			}
			else if (block.rfind("else", 0) != std::string::npos && prev_block[prev_block.length() - 1] == '$')
			{
				ParseLogicBlock(block, root_, "else");
			}
			//else if (block.rfind("print", 0))
			//{
			//	ParsePrint(block, root_);
			//}
			else if (CharIsVar(block[0]))
			{
				ParseAssignment(block, root_);
			}
		}
	}

	void SyntaxTree::ParseLogicBlock(std::string& block, Node* current_parent, std::string logic_type)
	{
		std::cout << "-------\nParsing:\n" << block <<std::endl;

		Node* parent = new Node();
		current_parent->nodes.push_back(parent);
		parent->token.type = "LOGIC";
		parent->token.value = logic_type;
		current_parent = parent;

		bool block_finished = false;
		std::string line;
		bool comp_ran = false;

		while (!block_finished)
		{
			line = block.substr(0, block.find_first_of('\n'));
			if (LineContainsCompOperator(line))
			{
				if (!comp_ran)
				{
					block.erase(0, block.find_first_of('\n') + 1);
				}
			}
			else
			{
				block.erase(0, block.find_first_of('\n') + 1);
			}
			while (line.length() > 0)
			{
				if (LineContainsCompOperator(line) && !comp_ran)
				{
					ParseComparison(line.substr(logic_type.size()), current_parent);
					comp_ran = true;
					line = "";
				}
				else if (line.rfind("loop",  0) != std::string::npos)
				{
					ParseLogicBlock(block, current_parent, "loop");
					line = block.substr(0, block.find_first_of('\n'));
				}
				else if (line.rfind("elif", 0) != std::string::npos)
				{
					ParseLogicBlock(block, current_parent, "elif");
					line = block.substr(0, block.find_first_of('\n'));
				}
				else if (line.rfind("if", 0) != std::string::npos)
				{
					std::cout << line << std::endl;
					ParseLogicBlock(block, current_parent, "if");
					line = block.substr(0, block.find_first_of('\n'));
				}
				else if (line.rfind("else", 0) != std::string::npos && logic_type == "else")
				{
					line = "";
				}
				else if (line.rfind("else", 0) != std::string::npos)
				{
					ParseLogicBlock(block, current_parent, "else");
					line = block.substr(0, block.find_first_of('\n'));
				}
				//else if (line.rfind("print", 0))
				//{
				//	ParsePrint(line, current_parent);
				//}
				else if (CharIsVar(line[0]))
				{
					ParseAssignment(line, current_parent);
					line = "";
				}
				else if (line.find('$') != std::string::npos)
				{
					block_finished = true;
					line = "";
				}
			}
		}

		std::cout << "Logic block parsing finished." << std::endl;
	}

	void SyntaxTree::ParseComparison(std::string line, Node* current_parent)
	{
		std::cout << "-------\nParsing:\n" << line << std::endl;

		long long offset = 0;

		std::string constant;
		bool var_is_constant = CharIsConst(line[offset]);
		if (var_is_constant)
		{
			constant = ParseConstant(line, offset);
		}
		else
		{
			offset++;
		}

		int op_length = 0;
		for (std::string op : kCompOperators)
		{
			if (line.find(op) != std::string::npos)
			{
				op_length = op.length();
			}
		}

		Node left_parent;
		current_parent->nodes.push_back(&left_parent);
		left_parent.token.type = "COMPARE";
		left_parent.token.value = line.substr(offset, offset + op_length);
		offset += op_length;

		Node left;
		left_parent.nodes.push_back(&left);
		if (LineContainsBinOperator(line))
		{
			ParseBinOperation(line, &left, offset);
		}
		else
		{
			if (!var_is_constant)
			{
				left.token.type = "VARIABLE";
				left.token.value = line[offset];

				offset++;
			}
			else
			{
				left.token.type = "CONSTANT";
				left.token.value = constant;
			}
		}

		Node right;
		left_parent.nodes.push_back(&right);
		if (LineContainsBinOperator(line))
		{
			ParseBinOperation(line, &right, offset);
		}
		else
		{
			var_is_constant = CharIsConst(line[offset]);
			if (var_is_constant)
			{
				constant = ParseConstant(line, offset);
			}
			else
			{
				offset++;
			}

			if (!var_is_constant)
			{
				right.token.type = "VARIABLE";
				right.token.value = line[offset];
			}
			else
			{
				right.token.type = "CONSTANT";
				right.token.value = constant;
			}
		}

		std::cout << "Comparison parsing finished." << std::endl;
	}

	void SyntaxTree::ParsePrint(std::string line, Node* current_parent)
	{
		std::cout << "-------\nParsing:\n" << line << std::endl;

		Node* current_node = new Node();
		current_parent->nodes.push_back(current_node);
		current_node->token.type = "PRINT";
		current_node->token.value = line.substr(5);
	}

	void SyntaxTree::ParseAssignment(std::string line, Node* current_parent)
	{
		std::cout << "-------\nParsing:\n" << line << std::endl;

		Node* current_node = new Node();

		long long offset = 0;
		while(offset < line.length())
		{
			if (offset + 1 < line.length() && line[offset + 1] == '=')
			{
				current_parent->nodes.push_back(current_node);
				current_node->token.type = "ASSIGN";

				Node left;
				current_node->nodes.push_back(&left);
				left.token.type = "VARIABLE";
				left.token.value = line[offset];

				offset += 2;
			}
			else if (LineContainsBinOperator(line))
			{
				ParseBinOperation(line, current_node, offset);
			}
			else if (CharIsVar(line[offset]))
			{
				Node right;
				current_node->nodes.push_back(&right);
				right.token.type = "VARIABLE";
				right.token.value = line[offset];

				offset++;
			}
			else if (CharIsConst(line[offset]))
			{
				Node right;
				current_node->nodes.push_back(&right);
				right.token.type = "CONSTANT";
				right.token.value = ParseConstant(line, offset);
			}
			else if (line[offset] == ';')
			{
				offset++;
			}
		}

		std::cout << "Assignment parsing finished." << std::endl;
	}

	void SyntaxTree::ParseBinOperation(std::string& line, Node* current_parent, long long& offset)
	{
		std::string constant;
		bool var_is_constant = CharIsConst(line[offset]);
		if (var_is_constant)
		{
			constant = ParseConstant(line, offset);
		}
		else
		{
			offset++;
		}

		Node new_parent;
		current_parent->nodes.push_back(&new_parent);
		new_parent.token.type = "OPERATE";
		new_parent.token.value = line[offset];
		line[offset] = ' ';
		offset++;

		Node left;
		new_parent.nodes.push_back(&left);
		if (!var_is_constant)
		{
			left.token.type = "VARIABLE";
			left.token.value = line[offset];
		}
		else
		{
			left.token.type = "CONSTANT";
			left.token.value = constant;
		}

		Node right;
		new_parent.nodes.push_back(&right);
		if (LineContainsBinOperator(line))
		{
			ParseBinOperation(line, &right, offset);
		}
		else
		{
			var_is_constant = CharIsConst(line[offset]);
			if (var_is_constant)
			{
				constant = ParseConstant(line, offset);
			}
			else
			{
				offset++;
			}

			if (!var_is_constant)
			{
				right.token.type = "VARIABLE";
				right.token.value = line[offset];
			}
			else
			{
				right.token.type = "CONSTANT";
				right.token.value = constant;
			}
		}
	}

	std::string SyntaxTree::ParseConstant(std::string line, long long& offset)
	{
		std::string constant;
		for (offset; offset < line.length(); offset++)
		{
			if (!CharIsConst(line[offset]))
			{
				break;
			}
			constant += line[offset];
		}
		return constant;
	}

	bool SyntaxTree::CharIsConst(char c)
	{
		if (std::find(std::begin(kConstants), std::end(kConstants),
			c) != std::end(kConstants))
		{
			return true;
		}
		return false;
	}

	bool SyntaxTree::CharIsVar(char c)
	{
		if ((int)c >= 97 && (int)c <= 122)
		{
			return true;
		}
		return false;
	}
	
	bool SyntaxTree::LineContainsBinOperator(std::string line)
	{
		for (char op : kBinOperators)
		{
			if (line.find(op) != std::string::npos)
			{
				return true;
			}
		}
		return false;
	}
	
	bool SyntaxTree::LineContainsCompOperator(std::string line)
	{
		for (std::string op : kCompOperators)
		{
			if (line.find(op) != std::string::npos)
			{
				return true;
			}
		}
		return false;
	}

	bool SyntaxTree::LineStartsWithLogicOperator(std::string line)
	{
		for (std::string op : kLogicOperators)
		{
			if (line.rfind(op, 0) != std::string::npos)
			{
				return true;
			}
		}
		return false;
	}

	void SyntaxTree::Print()
	{
		std::cout << ToStr(1, root_);
	}

	std::string SyntaxTree::ToStr(int lvl, Node* current_parent)
	{
		std::string info;
		if (current_parent->nodes.empty())
		{
			return "";
		}
		for (Node* n : current_parent->nodes)
		{
			info += ToStr(lvl + 1, n) + " ";
		}
		return info + "\n";
	}

	Node* SyntaxTree::GetRoot()
	{
		return root_;
	}
}