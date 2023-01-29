#include <chrono>

#include "Scanner.h"
#include "SyntaxTree.h"
#include "Executor.h"

int main(int argc, char* argv)
{
	auto program_start_time = std::chrono::high_resolution_clock::now();

	const std::string kPath = "code.txt";
	std::string raw_code;

	try
	{
		raw_code = EsoInterpreter::ScanFile(kPath);
	}
	catch (std::exception e)
	{
		std::cout << "Exception scanning file: " << kPath;
		return -1;
	}

	//std::cout << raw_code;
	EsoInterpreter::SyntaxTree tree(raw_code);
	//std::cout << tree.GetRoot()->nodes[0]->nodes[0]->token.type;
	//tree.Print();

	/*
	EsoInterpreter::SyntaxTree tree("");
	EsoInterpreter::Node* current_parent = tree.GetRoot();
	EsoInterpreter::Node n1;
	current_parent->nodes.push_back(&n1);
	n1.token.type = "ASSIGNMENT";
	current_parent = &n1;
	EsoInterpreter::Node n2;
	current_parent->nodes.push_back(&n2);
	n2.token.type = "VARIABLE";
	n2.token.value = "a";
	EsoInterpreter::Node n3;
	current_parent->nodes.push_back(&n3);
	n2.token.type = "CONSTANT";
	n2.token.value = "10";
	*/

	EsoInterpreter::Executor ex;
	ex.ExecuteTree(tree);

	auto program_finish_time = std::chrono::high_resolution_clock::now();
	std::cout << "-------\nTook a total of " << std::chrono::duration_cast<std::chrono::nanoseconds>(program_finish_time - program_start_time).count() <<
		" nanoseconds to run " << kPath << std::endl;
}