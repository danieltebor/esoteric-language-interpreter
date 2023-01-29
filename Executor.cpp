#include "Executor.h"

namespace EsoInterpreter
{
	void Executor::ExecuteNode(Node* node)
	{
		std::string type = node->token.type;
		std::string value = node->token.value;

		if (type == "ASSIGN")
		{
			ExecuteAssignment(node);
		}
		else if (type == "LOGIC")
		{
			ExecuteLogic(node);
		}
		else if (type == "PRINT")
		{
			std::cout << GetVariable(node->token.value[0]) << std::endl;
		}
	}

	void Executor::ExecuteLogic(Node* node)
	{
		if (node->token.value == "if"
			|| node->token.value == "elif"
			|| node->token.value == "else")
		{
			if (ExecuteComparison(node->nodes[0]))
			{
				for (int i = 1; i < node->nodes.size(); i++)
				{
					ExecuteNode(node->nodes[i]);
				}
			}
		}
		else if (node->token.value == "loop")
		{
			while (ExecuteComparison(node->nodes[0]))
			{
				for (int i = 1; i < node->nodes.size(); i++)
				{
					ExecuteNode(node->nodes[i]);
				}
			}
		}
		else
		{
			for (Node* n : node->nodes)
			{
				ExecuteNode(n);
			}
		}
	}

	void Executor::ExecuteAssignment(Node* node)
	{
		int& var = GetVariable(node->nodes[0]->token.value[0]);

		if (node->nodes[1]->token.type == "OPERATE")
		{
			var = ExecuteBinOperation(node->nodes[1]);
		}
		else
		{
			var = stoi(node->nodes[1]->token.value);
		}
	}

	bool Executor::ExecuteComparison(Node* node)
	{
		int var1 = AssignVariable(node->nodes[0]);
		int var2 = AssignVariable(node->nodes[1]);

		if (node->token.value == "==")
		{
			return var1 == var2;
		}
		else if (node->token.value == "!=")
		{
			return var1 != var2;
		}
		else if (node->token.value == "<")
		{
			return var1 < var2;
		}
		else if (node->token.value == "<=")
		{
			return var1 <= var2;
		}
		else if (node->token.value == ">")
		{
			return var1 > var2;
		}
		else if (node->token.value == ">=")
		{
			return var1 >= var2;
		}

		return false;
	}

	int Executor::ExecuteBinOperation(Node* node)
	{
		int var1 = AssignVariable(node->nodes[0]);
		int var2 = AssignVariable(node->nodes[1]);

		if (node->token.value[0] == '*')
		{
			return var1 * var2;
		}
		else if (node->token.value[0] == '/')
		{
			return var1 / var2;
		}
		else if (node->token.value[0] == '+')
		{
			return var1 + var2;
		}
		else if (node->token.value[0] == '-')
		{
			return var1 - var2;
		}
		else if (node->token.value[0] == '%')
		{
			return var1 % var2;
		}

		return 0;
	}

	int Executor::AssignVariable(Node* node)
	{
		if (node->token.type == "OPERATE")
		{
			return ExecuteBinOperation(node);
		}
		else
		{
			if (node->token.type == "VARIABLE")
			{
				return GetVariable(node->token.value[0]);
			}
			else
			{
				return stoi(node->token.value);
			}
		}
	}

	int& Executor::GetVariable(char var)
	{
		return variables[(int)var - 97];
	}

	void Executor::ExecuteTree(SyntaxTree tree)
	{
		std::cout << "-------\nOutput:\n";
		//ExecuteLogic(tree.GetRoot());

		// Debug
		Node root;

		Node parent;
		parent.token.type = "ASSIGN";
		root.nodes.push_back(&parent);
		
		Node child_left;
		parent.nodes.push_back(&child_left);
		child_left.token.type = "VARIABLE";
		child_left.token.value = 'a';
		Node child_right;
		parent.nodes.push_back(&child_right);
		child_right.token.type = "CONSTANT";
		child_right.token.value = "10";

		Node print;
		root.nodes.push_back(&print);
		print.token.type = "PRINT";
		print.token.value = 'a';
		
		ExecuteLogic(&root);
	}
}