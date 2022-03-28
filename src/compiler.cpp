#include "../include_ast/ast.hpp"

int main (int argc, char* argv[])
{
	yyin = fopen(argv[2], "r");
	std::ofstream output(argv[4]);

	Context content;
	const Node* root = parse();
	
	root->compile(output, content);

	return 0;
}

