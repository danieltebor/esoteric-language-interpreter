#pragma once

#include "SyntaxTree.h"

namespace EsoInterpreter
{
	class Executor
	{
	private:
		int variables[26];

		void ExecuteNode(Node* node);
		void ExecuteLogic(Node* node);
		void ExecuteAssignment(Node* node);
		bool ExecuteComparison(Node* node);
		int ExecuteBinOperation(Node* node);
		int AssignVariable(Node* node);
		int& GetVariable(char var);
	public:
		void ExecuteTree(SyntaxTree tree);
	};
}