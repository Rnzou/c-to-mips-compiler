#include "../include/ast.hpp"

int main()
{

	const Node *root;

	Context context;
	
	root = new Add_Expression(new Constant(5), new Constant(11));

	root->print_MIPS(std::cout, context);

	return 0;
}