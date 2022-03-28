#ifndef FIVEAST_STATEMENT_HPP
#define FIVEAST_STATEMENT_HPP


#include <iomanip>

/*  
https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture15/C_code.pdf c to mips
https://courses.engr.illinois.edu/cs232/fa2011/section/disc1.pdf c to mips
https://web.engr.oregonstate.edu/~walkiner/cs271-wi13/slides/07-MoreAssemblyProgramming.pdf c to mpis
*/


class Expression_Statement : public Statement
{
	private:
		Expression *expression;

	public:
		Expression_Statement ( Expression* _expression = NULL ) 
		: expression (_expression) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			if(expression != NULL)
			{	
				content.addone_stack();
				expression->compile(output, content);
				content.decreaseone_stack();
			}
		}
};

class Compound_Statement : public Statement
{
	private:
		std::vector<Statement*>* 	statement_list;
		std::vector<Declaration*>* 	declaration_list;

	public:
		Compound_Statement (std::vector<Declaration*>* _declaration_list = NULL, std::vector<Statement*>* _statement_list = NULL)
		: statement_list (_statement_list), declaration_list (_declaration_list) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			content.expand_context_scope();

			if (declaration_list != NULL)
			{
				for (auto declaration = declaration_list->begin(); declaration != declaration_list->end(); declaration++)
				{
					(*declaration)->compile(output, content);
				}
			}

			if (statement_list != NULL)
			{
				for(auto statement = statement_list->begin(); statement != statement_list->end(); statement++)
				{
					(*statement)->compile(output, content);
				}
			}

			content.shrink_context_scope();
		}
};


class Condition_If_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		Condition_If_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			content.addone_stack();
			std::string temp_register_1 = "$2";
			
			condition_expression->compile(output, content);

			content.decreaseone_stack();

			std::string if_return_label = content.make_label("IF_RETURN");

			output << "\t" << "bnq " << "\t" << "$0" << "," << temp_register_1 << "," << if_return_label << std::endl;

			true_statement->compile(output, content);

			output << if_return_label << ":" << std::endl;
		}
};

class Condition_If_Else_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* 	true_statement;
		Statement* 	false_statement;
		
	public:
		Condition_If_Else_Statement (Expression* _condition_expression, Statement* _true_statement, Statement* _false_statement)
		: condition_expression (_condition_expression), true_statement (_true_statement), false_statement (_false_statement) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			content.addone_stack();
			std::string temp_register_1 = "$2";
			int frame_pointer_1 = content.get_stack_pointer();

			condition_expression->compile(output, content);
			content.lw_register(output, temp_register_1, frame_pointer_1);

			content.decreaseone_stack();


			std::string if_return_label = content.make_label("IF_RETURN");
			std::string else_return_label = content.make_label("ELSE_RETURN");

			output << "\t" << "bnq " << "\t" << "$0" << "," << temp_register_1 << "," << if_return_label << std::endl;

			true_statement->compile(output, content);

			output << "\t" << "b " << "\t" << "\t" << else_return_label << std::endl;

			
			output << if_return_label << ":" << std::endl;

			false_statement->compile(output, content);

			
			output << else_return_label << ":" << std::endl;

		}

};

class While_Statement : public Statement
{
	private:
		Expression* condition_expression;
		Statement* true_statement;
		
	public:
		While_Statement (Expression* _condition_expression, Statement* _true_statement)
		: condition_expression(_condition_expression), true_statement(_true_statement) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			std::string START_label = content.make_label("START_WHILE");
			std::string FINISH_label = content.make_label("FINISH_WHILE");

			output << START_label << ":" << std::endl;

			content.addone_stack();
			std::string temp_register_1 = "$2";
			int frame_pointer_1 = content.get_stack_pointer();
			content.lw_register(output, temp_register_1, frame_pointer_1);

			condition_expression->compile(output, content);

			content.decreaseone_stack();
			
			output << "\t" << "beq " << "\t" << "$0" << "," << temp_register_1 << "," << FINISH_label << std::endl;

			true_statement->compile(output, content);

			output << "\t" << "b " << "\t"  << "\t" << START_label << std::endl;
			output << "\t" << FINISH_label << ":" << std::endl;
		}

};


class For_Statement : public Statement
{
	private:
		Expression* initialisation_expression;
		Expression* condition_expression;
		Expression* update_expression;
		Statement*	true_statement;
		
	public:
		For_Statement (Expression* _init_expr, Expression* _condition_expression, Expression* _update_expression, Statement* _true_statement)
		: initialisation_expression(_init_expr),condition_expression(_condition_expression), update_expression(_update_expression), true_statement(_true_statement) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			std::string START_label = content.make_label("START_FOR");
			std::string FINISH_label = content.make_label("FINISH_FOR");

			if(initialisation_expression != NULL)
			{
				content.addone_stack();
				initialisation_expression->compile(output, content);
				content.decreaseone_stack();
			}

			
			output << START_label << ":" << std::endl;
			

			content.addone_stack();
			std::string temp_condition_reg = "$8";
			int stack_pointer_2 = content.get_stack_pointer();

			if(condition_expression != NULL)
			{
				condition_expression->compile(output, content);
			}

			content.lw_register(output, temp_condition_reg, stack_pointer_2);

			content.decreaseone_stack();

			output << "\t" << "beq " << "\t" << "$0" << "," << temp_condition_reg << "," << FINISH_label << std::endl;

			true_statement->compile(output, content);

			if(update_expression != NULL)
			{
				content.addone_stack();
				std::string temp_update_reg = "$8";

				update_expression->compile(output, content);
				
				content.decreaseone_stack();
			}

			output << "\t" << 'g ' <<"\t" << "\t" << START_label << std::endl;

			
			output << FINISH_label << ":" << std::endl;
			
		}

};

class Jump_Statement : public Statement
{
	private:
		Expression* expression;

	public:
		Jump_Statement (Expression* _expression = NULL)
		: expression(_expression) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			if(expression != NULL)
			{
				content.addone_stack();

				int dest_int = content.get_stack_pointer();
				expression->compile(output, content);

				content.decreaseone_stack();

				dest_int = content.get_stack_pointer();

				output << "\t" << "b " << "\t"  << "\t" << content.function_return_label() << std::endl;
			}
		}
};

class Break_Statement : public Statement
{
	private:

	public:
		Break_Statement() {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			output << "\t" << "b " << "\t"  << "\t" << content.get_break_label().top() << std::endl;
		}
};

class Continue_Statement : public Statement
{
	private:

	public:
		Continue_Statement() {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			output << "\t" << "b " << "\t"  << "\t" << content.get_continue_label().top() << std::endl;
		}
};


class Switch_Statement : public Statement
{
	private:
		Expression* expression;
		Statement* switch_statements;

	public:
		Switch_Statement (Expression* _expression = NULL, Statement* _switch_statements = NULL)
		: expression(_expression), switch_statements(_switch_statements) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{			
			
			output << "# ------------ Switch statement ------------ #" << std::endl;

			std::string START_label = content.make_label("START_SWITCH");
			std::string FINISH_label = content.make_label("FINISH_SWITCH");

			content.add_break_label(FINISH_label);

			std::stringstream ss; 
			switch_statements->compile(ss, content);

			content.addone_stack();
			std::string switch_register = "$2";
			int switch_stack_pointer = content.get_stack_pointer();
			expression->compile(output, content);

			content.addone_stack();
			std::string case_register = "$8";
			int case_stack_pointer = content.get_stack_pointer();
			
			while(content.get_case_statement_size() != 0)
			{
				Expression* case_statement = content.get_case_statement();
				case_statement->compile(output, content);
				std::string case_label = content.get_case_label();

				content.lw_register(output, switch_register, switch_stack_pointer);
				content.lw_register(output, case_register, case_stack_pointer);

				output << "\t" << "beq" << "\t" << switch_register << "," << case_register << "," << case_label << std::endl;

				content.remove_case_statement();
				content.remove_label_statement();
			}

			if(content.get_case_label_size() != 1)
			{
				output << "\t" << "b " << "\t"  << "\t" << content.get_case_label() << std::endl;
				content.remove_label_statement();
			}

			output << "\t" << "b " << "\t"  << "\t" << FINISH_label << std::endl;

			output << ss.str() << std::endl;

			output << FINISH_label << ":" << std::endl;

			content.decreaseone_stack();
			content.decreaseone_stack();
			content.remove_break_label();
		}
};

class Case_Statement : public Statement
{
	private:
		Expression* expression;
		Statement* switch_statements;

	public:
		Case_Statement( Expression* _expression, Statement* _switch_statements)
		: expression(_expression), switch_statements(_switch_statements) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			output << "# ------------ Case statement ------------ #" << std::endl;

			std::string case_label = content.make_label("CASE");

			content.add_case_statements(expression, case_label);

			output << case_label << ":" << std::endl;

			switch_statements->compile(output, content);
		}
};

class Default_Statement : public Statement
{
	private:
		Statement* default_statements;

	public:
		Default_Statement(Statement* _default_statements)
		: default_statements(_default_statements) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			output << "# ------------ Default statement ------------ #" << std::endl;
			std::string default_label = content.make_label("DEFAULT");

			content.add_case_label(default_label);

			output << default_label << ":" << std::endl;

			default_statements->compile(output, content);
		}
};

#endif