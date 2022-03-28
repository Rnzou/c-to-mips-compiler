#ifndef FIVEAST_EXPRESSION_HPP
#define FIVEAST_EXPRESSION_HPP

#include <string>
#include <iostream>
#include <map>

#include "ast5_declaration.hpp"
/*
 https://people.cs.pitt.edu/~childers/CS0447/lectures/SlidesLab92Up.pdf
 https://stackoverflow.com/questions/54721000/c-array-indexing-in-mips-assembly
 https://courses.cs.washington.edu/courses/cse378/09wi/lectures/lec03.pdf
 https://chortle.ccsu.edu/AssemblyTutorial/Chapter-31/ass31_2.html
 http://ww2.cs.fsu.edu/~dennis/teaching/2013_summer_cda3100/week5/week5-day2.pdf
 https://people.cs.pitt.edu/~childers/CS0447/lectures/SlidesLab92Up.pdf
 https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division
 https://www.cs.tufts.edu/comp/140/lectures/Day_3/mips_summary.pdf
 https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division
 https://stackoverflow.com/questions/16050338/mips-integer-multiplication-and-division
*/

/*
--Node
	--expression
		--Unary_Expression
		--Assignment_Expression
		--Operator

*/


class Expression : public Node 
{
	public:
		virtual std::string get_variable_name() const { return 0; };
		virtual void load_variable_address(std::ostream &output, Context& content) const {};
		virtual type get_value_type(Context& content) const { return type(INT); };
		virtual bool get_pointer_space(Context& content) const { return 0; };
		virtual type_def get_type_def(Context& content) const {};

		virtual double evaluate() const { return 0; };
};

typedef const Expression *Expression_Ptr;

/* SIZE */
class Sizeof_Type_Expression : public Expression
{
	private:
		type sizeof_type;		
	public:
		Sizeof_Type_Expression(type_def Sizeof_type) 
		: sizeof_type(Sizeof_type.get_variable_type()) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$2";
/* find size of sellected type */
			if( sizeof_type == VOID)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(sizeof_type == INT || sizeof_type == FLOAT)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 4 << std::endl;
			}
			else if(sizeof_type == FLOAT)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(sizeof_type == CHAR)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(sizeof_type == DOUBLE)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}

			else if(sizeof_type == UNSIGNED_INT)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
/* store the size */
			content.sw_register(output, dest_reg, stack_pointer);	
		}
};

class Sizeof_Variable_Expression : public Expression
{
	private:
		std::string variable_name;

	public:
		Sizeof_Variable_Expression(std::string _variable_name) 
		: variable_name(_variable_name) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
/* find the variable type*/
			type variable_type = content.get_variable(variable_name).get_variable_type();

			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$2";

/* find the size of selected type */
			if(variable_type == VOID)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 0 << std::endl;
			}
			else if(variable_type == INT || variable_type == FLOAT )
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 4 << std::endl;
			}
			else if(variable_type == FLOAT)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(variable_type == CHAR)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 1 << std::endl;
			}
			else if(variable_type == DOUBLE)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
			else if(variable_type == UNSIGNED_INT)
			{
				output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << 8 << std::endl;
			}
/* store the size */
			content.sw_register(output, dest_reg, stack_pointer);	
		}
};


/* normal decrement*/
class Decrement_Expression : public Expression
{
	protected:
		Expression* left=NULL;
		Expression* right=NULL;

	public:
		Decrement_Expression (Expression* Left, Expression* Right) : left (Left), right (Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string dest_reg, std::string temprorary_register) const {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			type operator_type = INT;

			int frame_pointer_1 = content.get_stack_pointer(); 
			std::string temp_register = "$2";
			left->compile(output, content);

			content.addone_stack();
			int frame_pointer_2 = content.get_stack_pointer();
			std::string dest_reg = "$8";
			right->compile(output, content);

			content.decreaseone_stack(); 

			content.lw_register(output, temp_register, frame_pointer_1);
			content.lw_register(output, dest_reg, frame_pointer_2);

			decrement(output, content, operator_type, temp_register, dest_reg);

			content.sw_register(output, temp_register, frame_pointer_1);
		}

		void decrement(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const
		{
			output << "\t" << "subu" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}	

};




class Assignment_Expression : public Expression 
{
	protected:
		Expression* left_number;
		Expression* expression;
			
	public:
		Assignment_Expression (Expression* Left_number, Expression* _expression) : left_number (Left_number), expression (_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			output << "Triggered 4" << std::endl;
		}
};

class Direct_Assignment : public Assignment_Expression
{
	public:
		Direct_Assignment (Expression* Left_number, Expression* _expression) : Assignment_Expression (Left_number, _expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			type operator_type = INT; 

			int frame_pointer_1 = content.get_stack_pointer();

			std::string dest_reg = "$2";
			left_number->load_variable_address(output, content);		

			output << "\t" << "# Compile Direct Assignment Expression" << std::endl; 

			content.addone_stack();

			int frame_pointer_2 = content.get_stack_pointer(); 
			std::string temp_register_1 = "$8";
			expression->compile(output, content);

			content.decreaseone_stack();

			content.lw_register(output, dest_reg, frame_pointer_1);
			content.lw_register(output, temp_register_1, frame_pointer_2);
			content.output_sw_register(output, operator_type, temp_register_1, dest_reg, 0);

			content.sw_register(output, temp_register_1, frame_pointer_1);
		}

		virtual type get_value_type(Context& content) const override 
		{ 
			left_number->get_value_type(content); 
		};
};


class Unary_Expression : public Expression
{
	protected:
		Expression* expression;

	public:
		Unary_Expression(Expression* _expression) : expression(_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{ output << "Triggered 1" << std::endl; }
};

/* -- and ++ at head of end*/

class Pre_Negative_Expression : public Unary_Expression
{
	private:

	public:
		Pre_Negative_Expression (Expression* _expression) 
		: Unary_Expression(_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{

			std::string dest_reg = "$2";
			int stack_pointer = content.get_stack_pointer();

			expression->compile(output, content);

			content.lw_register(output, dest_reg, stack_pointer);

			if (expression->get_value_type(content) == FLOAT)
			{
				content.mtc1_reg(output, dest_reg, "$f0");
				content.mtc1_reg(output, "$0", "$f2");

				output << "\t" << "sub.s"  << "\t" << "$f0" << "," << "$f2" << "," << "$f0" << std::endl; 

				content.mfc1_reg(output, dest_reg, "$f0");
			}
			else if (expression->get_value_type(content) == INT)
			{
				output << "\t" << "sub"  << "\t" << "\t" << dest_reg << "," << "$0" << "," << dest_reg << std::endl; 
			}

			content.sw_register(output, dest_reg, stack_pointer);
		}
};

class Post_Increment_Expression : public Unary_Expression
{
	private:
		Expression* increment_expression;
	public:
		Post_Increment_Expression (Expression* _expression, Expression* _increment_expression) 
		: Unary_Expression(_expression), increment_expression(_increment_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			expression->compile(output, content);

			content.addone_stack();
			increment_expression->compile(output, content);
			content.decreaseone_stack();
		}

		virtual type get_value_type(Context& content) const override { return increment_expression->get_value_type(content); };
};

class Post_Decrement_Expression : public Unary_Expression
{
	private:
		Expression* decrement_expression;
	public:
		Post_Decrement_Expression (Expression* _expression, Expression* _decrement_expression) 
		: Unary_Expression(_expression), decrement_expression(_decrement_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			expression->compile(output, content);

			content.addone_stack();
			decrement_expression->compile(output, content);
			content.decreaseone_stack();
		}

		virtual type get_value_type(Context& content) const override 
		{ 
			decrement_expression->get_value_type(content); 
		};
};


/* CAST */
class Cast_Expression : public Unary_Expression
{
	private:
		type casting_type;
	
	public:
		Cast_Expression(type_def _casting_type, Expression* _expression) 
		: casting_type( _casting_type.get_variable_type()), Unary_Expression(_expression) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			std::string cast_register = "$2";
			int cast_address = content.get_stack_pointer();

			expression->compile(output, content);

			content.sw_register(output, cast_register, cast_address);
		}

		virtual type get_value_type(Context& content) const override { return casting_type; };
};

class Function_Call_Expression : public Unary_Expression
{
	private:
		std::vector<Expression*>* argument_list;

	public:

		Function_Call_Expression(Expression *_expression, std::vector<Expression*>* _argument_list = NULL)
		: Unary_Expression(_expression), argument_list(_argument_list) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			
			std::string variable_ID = expression->get_variable_name();
			int argument_size;

			int function_register_address = content.get_stack_pointer();
			std::string function_register = "$2";
			
			if(argument_list != NULL)
			{
				argument_size = argument_list->size();

				for(int i = 0; i < argument_size; i++) { content.addone_stack(); }
				
				int function_stack_pointer = content.get_stack_pointer();
				std::string argument_registers[4]  = {"$4", "$5", "$6", "$7"};

				int argument_stack_pointer = 0;
				for (int i = 0; i < argument_size; i++)
				{
					argument_stack_pointer += 4;

					content.addone_stack();
					std::string temp_register = "$8";
					int temp_register_address = content.get_stack_pointer();

					(*argument_list)[i]->compile(output, content);

					content.decreaseone_stack();

					content.lw_register(output, temp_register, temp_register_address);
					output << "\t" << "sw" << "\t" << "\t" << temp_register << "," << function_stack_pointer + argument_stack_pointer << "($30)" <<  std::endl; 
				}

				int argument_load_pointer = 0;
				int temp_register = 0;
				for(int i = 0; i < argument_size; i++)
				{
					argument_load_pointer += 4;

					if(i < 4)
					{
						output << "\t" << "lw" << "\t" << "\t" << argument_registers[i] << "," << function_stack_pointer + argument_load_pointer << "($30)" <<  std::endl; 
					}
					else
					{
						temp_register = 4 + i;
						std::string temp_register_string = "$" + std::to_string(temp_register);
						output << "\t" << "sw" << "\t" << "\t" << temp_register_string << "," << function_stack_pointer + argument_load_pointer - 4 << "($30)" <<  std::endl; 
					}
				}
			}
			
			output << "\t" << "addiu" << "\t" << "$29, $29," << content.get_stack_pointer() << std::endl; 
			output << "\t" << "la"    << "\t" << "\t" << "$2," << variable_ID << std::endl;
			output << "\t" << "jalr"  << "\t" << "$2" << std::endl;
			output << "\t" << "addiu" << "\t" << "$29, $29," << -content.get_stack_pointer() << std::endl; 

			for(int i = 0; i < argument_size; i++) { content.decreaseone_stack(); }

			content.sw_register(output, function_register, function_register_address);
		}

		virtual type get_value_type(Context& content) const override
		{
			std::string variable_ID = expression->get_variable_name();

			if (content.check_function_declared(variable_ID)) { return (content.get_variable(variable_ID)).get_variable_type(); }
			else { return type(INT); }
		}
};


class Array_Access_Expression : public Unary_Expression
{
	private:
		Expression* array_expression;

	public:
		Array_Access_Expression(Expression* _expression, Expression* _array_expression)
		: Unary_Expression(_expression), array_expression(_array_expression) {}

		virtual void load_variable_address(std::ostream &output, Context& content) const 
		{
			int array_frame_pointer = content.get_stack_pointer();
			std::string array_register = "$2";
			expression->compile(output, content);

			content.addone_stack();
			int temp_array_stack_pointer = content.get_stack_pointer();
			std::string temp_array_register = "$8";

			array_expression->compile(output, content);

			content.decreaseone_stack();

			content.lw_register(output, array_register, array_frame_pointer);
			content.lw_register(output, temp_array_register, temp_array_stack_pointer);

			output << "\t" << "sll" << "\t" << "\t" << temp_array_register << "," << temp_array_register << "," << 2 << std::endl;
			output << "\t" << "addu" << "\t" << array_register << "," << array_register << "," << temp_array_register << std::endl;

			content.sw_register(output, array_register, array_frame_pointer);
		};

		virtual void compile(std::ostream &output, Context& content) const override
		{
			type array_type = INT;

			int array_frame_pointer = content.get_stack_pointer();
			std::string array_register = "$2";

			load_variable_address(output, content);

			output << "\t" << "# Compile Array Access" << std::endl; 

			content.lw_register(output, array_register, array_frame_pointer);
			content.output_lw_register(output, array_type, array_register, array_register, 0);

			content.sw_register(output, array_register, array_frame_pointer);
		}

		virtual type get_value_type(Context& content) const override { return array_expression->get_value_type(content); }
};


class Reference_Expression : public Unary_Expression
{
	public:
		Reference_Expression(Expression* _expression) : Unary_Expression(_expression) {} 

		virtual void compile(std::ostream &output, Context& content) const override
		{
			expression->load_variable_address(output, content);
		}

		virtual type get_value_type(Context& content) const override 
		{ 
			return expression->get_value_type(content); 
		}
};


class Dereference_Expression : public Unary_Expression
{
	public:
		Dereference_Expression(Expression* _expression) : Unary_Expression(_expression) {} 

		virtual void compile(std::ostream &output, Context& content) const override
		{
			std::string pointer_register = "$2";
			int pointer_address = content.get_stack_pointer();

			expression->compile(output, content);

			content.lw_register(output, pointer_register, pointer_address);
			content.output_lw_register(output, INT, pointer_register, pointer_register, 0);
			content.sw_register(output, pointer_register, pointer_address);
		}

		virtual type get_value_type(Context& content) const override 
		{ 
			return expression->get_value_type(content); 
		}
};

/* operator */
class Operator : public Expression
{
	protected:
		Expression* left=NULL;
		Expression* right=NULL;

	public:
		Operator (Expression* Left, Expression* Right) : left (Left), right (Right) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			type operator_type = left->get_value_type(content);

			int frame_pointer_1 = content.get_stack_pointer(); 

			std::string dest_reg = "$2";
			left->compile(output, content);

			content.addone_stack(); 
			int frame_pointer_2 = content.get_stack_pointer();
			std::string temp_register = "$8";
			right->compile(output, content);

			content.decreaseone_stack(); 

			content.lw_register(output, dest_reg, frame_pointer_1);
			content.lw_register(output, temp_register, frame_pointer_2);

			execute(output, content, operator_type, dest_reg, temp_register);

			content.sw_register(output, dest_reg, frame_pointer_1);

			if (operator_type == FLOAT || operator_type == DOUBLE)
			{
				content.lwc1_reg(output, "$f0", frame_pointer_1);
			}
			
		}

		virtual void execute(std::ostream &output, Context& content, type type, std::string dest_reg, std::string temprorary_register) const {}

		virtual type get_value_type(Context& content) const override { return type(left->get_value_type(content)); };

		virtual bool get_pointer_space(Context& content) const { return left->get_pointer_space(content); };
};

class Add_Expression : public Operator
{
	public:
		Add_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			if (type == INT || type == UNSIGNED_INT)
			{
				output << "\t" << "addu" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			}
			else if (type == FLOAT)
			{
				std::string reg1 = "$f0";
				std::string reg2 = "$f1";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "add.s" << "\t" << reg1 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg1);
			}
			else if (type == DOUBLE)
			{
				std::string reg1 = "$f2";
				std::string reg2 = "$f0";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "add.d" << "\t" << reg2 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg2);
			}
			else
			{
				std::cerr<<"not supported value types"<<std::endl;
				exit(1);
			}
		}

		virtual double evaluate() const override { return left->evaluate() + right->evaluate(); };
};

class Sub_Expression : public Operator
{
	public:
		Sub_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				output << "\t" << "subu" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			}
			else if (type == FLOAT)
			{
				std::string reg1 = "$f0";
				std::string reg2 = "$f1";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "sub.s" << "\t" << reg1 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg1);
			}
			else if (type == DOUBLE)
			{
				std::string reg1 = "$f2";
				std::string reg2 = "$f0";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "sub.d" << "\t" << reg2 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg2);
			}
			else
			{
				std::cerr<<"not supported value types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() - right->evaluate(); };
};

class Multiply_Expression : public Operator
{
	public:
		Multiply_Expression (Expression* Left, Expression* Right) : Operator (Left,Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			// Check types
			if (type == INT || type == UNSIGNED_INT)
			{
				output << "\t" << "multu" << "\t"  << REG1 << "," << REG2 << std::endl;
				output << "\t" << "mflo"  << "\t"  << REG1 << std::endl;
			}
			else if (type == FLOAT)
			{
				std::string reg1 = "$f0";
				std::string reg2 = "$f1";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "mul.s" << "\t" << reg1 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg1);
			}
			else if (type == DOUBLE)
			{
				std::string reg1 = "$f2";
				std::string reg2 = "$f0";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "mul.d" << "\t" << reg2 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg2);
			}
			else
			{
				std::cerr<<"not supported value types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() * right->evaluate(); };
};

class Divide_Expression : public Operator
{
	public:
		Divide_Expression (Expression* Left, Expression* Right) : Operator (Left,Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			if (type == INT || type == UNSIGNED_INT)
			{
				output << "\t" << "divu" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
				output << "\t" << "mfhi" << "\t" << REG1 << std::endl;
				output << "\t" << "mflo" << "\t" << REG1 << std::endl;
			}
			else if (type == FLOAT)
			{
				std::string reg1 = "$f0";
				std::string reg2 = "$f1";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "div.s" << "\t" << reg1 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg1);
			}
			else if (type == DOUBLE)
			{
				std::string reg1 = "$f2";
				std::string reg2 = "$f0";

				content.mtc1_reg(output, REG1, reg1);
				content.mtc1_reg(output, REG2, reg2);

				output << "\t" << "div.d" << "\t" << reg2 << "," << reg1 << "," << reg2 << std::endl;

				content.mfc1_reg(output, REG1, reg2);
			}
			else
			{
				std::cerr<<"not supported value types"<<std::endl;
				exit(1);
			}
		}	

		virtual double evaluate() const override { return left->evaluate() / right->evaluate(); };
};

class Less_Than_Expression : public Operator
{
	public:
		Less_Than_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}
	
		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "slt " << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}	
};

class More_Than_Expression : public Operator
{
	public:
		More_Than_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "slt " << "\t" << REG1 << "," << REG2 << "," << REG1 << std::endl;
		}
};

class Less_Than_Equal_Expression : public Operator
{
	public:
		Less_Than_Equal_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "slt "  << "\t" << REG1 << "," << REG2 << "," << REG1 << std::endl;
			output << "\t" << "xori " << "\t" << "\t" << REG1 << "," << REG1 << "," << 0x1 << std::endl;
			output << "\t" << "andi " << "\t" << "\t" << REG1 << "," << REG1 << "," << 0x00ff << std::endl;
		}	
};

class More_Than_Equal_Expression : public Operator
{
	public:
		More_Than_Equal_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "slt "  << "\t" << REG1 << "," << REG2 << "," << REG1 << std::endl;
			output << "\t" << "xori " << "\t" << "\t" << REG1 << "," << REG1 << "," << 0x1 << std::endl;
		}
};

class Equal_Expression : public Operator
{
	public:
		Equal_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "xor " << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			output << "\t" << "sltiu " << "\t" << REG1 << "," << REG1 << "," << 1 << std::endl;
		}	
};

class Not_Equal_Expression : public Operator
{
	public:
		Not_Equal_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			
			output << "\t" << "xor " << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			output << "\t" << "sltu " << "\t" << REG1 << ",$0" << "," << REG1 << std::endl;
		}	
};

class Bitwise_AND_Expression : public Operator
{
	public:
		Bitwise_AND_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			// 

			output << "\t" << "and " << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}	
};

class Bitwise_OR_Expression : public Operator
{
	public:
		Bitwise_OR_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{

			output << "\t" << "or " << "\t" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}	
};

class Bitwise_XOR_Expression : public Operator
{
	public:
		Bitwise_XOR_Expression (Expression* Left, Expression* Right) : Operator (Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			

			output << "\t" << "xor " << "\t" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}	
};


class Logical_AND_Expression : public Operator
{
	public:
		Logical_AND_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			content.make_label("LOGICAL_AND");
			std::string return_label = content.function_return_label();

			std::string REGNAME = "$2";
			int dest_int = content.get_stack_pointer();

			left->compile(output, content);
			content.lw_register(output, REGNAME, dest_int);

			output << "\t" << "beq " << "\t" << REG1 << "," << "$0" << "," << return_label << std::endl;
			
			content.addone_stack();
			int frame_pointer_temp_reg = content.get_stack_pointer();

			std::string temp_register_1 = "$8";
			right->compile(output, content);

			content.lw_register(output, temp_register_1, frame_pointer_temp_reg);
			content.decreaseone_stack();

			output << "\t" << "beq " << "\t" << temp_register_1 << "," << "$0" << "," << return_label << std::endl;

			output << "\t" << "addiu " << "\t" << "\t" << REG1 << "," << "$0" << "," << 1 << std::endl;

			content.sw_register(output, REGNAME, dest_int);
		}

};

class Logical_OR_Expression : public Operator
{
	public:
		Logical_OR_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			std::string return_label_1 = content.make_label("LOGICAL_OR");
			std::string return_label_2 = content.make_label("LOGICAL_OR");

			std::string REGNAME = "$2";
			int dest_int = content.get_stack_pointer();

			left->compile(output, content);
			content.lw_register(output, REGNAME, dest_int);

			output << "\t" << "bne " << "\t" << "\t" << REG1 << "," << "$0" << "," << return_label_1 << std::endl;
			
			content.addone_stack();
			int frame_pointer_temp_reg = content.get_stack_pointer();

			std::string temp_register_1 = "$8";
			right->compile(output, content);

			content.lw_register(output, temp_register_1, frame_pointer_temp_reg);
			content.decreaseone_stack();

			output << "\t" << "beq " << "\t" << temp_register_1 << "," << "$0" << "," << return_label_2 << std::endl;

			output << "\t" << "addiu " << "\t" << REG1 << "," << "$0" << "," << 1 << std::endl;

			output << "\t" << return_label_1 << ":" << std::endl; 
			output << "\t" << "addiu " << "\t" << REG1 << "," << "$0" << "," << 1 << std::endl;
			output << "\t" << return_label_2 << ":" << std::endl; 

			content.sw_register(output, REGNAME, dest_int);
		}
};



class Left_Bitwise_Shift_Expression : public Operator
{
	public:
		Left_Bitwise_Shift_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			output << "\t" << "sllv " << "\t" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
		}
};

class Right_Bitwise_Shift_Expression : public Operator
{
	public:
		Right_Bitwise_Shift_Expression(Expression* Left, Expression* Right) : Operator(Left, Right) {}

		virtual void execute(std::ostream &output, Context& content, type type, std::string REG1, std::string REG2) const override
		{
			if (type == INT)
			{
				output << "\t" << "srav " << "\t" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			}
			else
			{
				output << "\t" << "sral " << "\t" << "\t" << REG1 << "," << REG1 << "," << REG2 << std::endl;
			}
		}
};

#endif