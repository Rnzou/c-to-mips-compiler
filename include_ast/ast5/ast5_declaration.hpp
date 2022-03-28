#ifndef FIVEAST_DECLARATION_HPP
#define FIVEAST_DECLARATION_HPP


#include <iomanip>

#include "ast5_expression.hpp"

// https://stackoverflow.com/questions/7748726/declarators-in-c
// https://stackoverflow.com/questions/54721000/c-array-indexing-in-mips-assembly
// https://cgi.cse.unsw.edu.au/~cs1521/20T2/lec/mips_data/notes
// https://courses.cs.washington.edu/courses/cse378/02sp/sections/section3-1.html
// https://stackoverflow.com/questions/2298838/mips-function-call-with-more-than-four-arguments
// https://stackoverflow.com/questions/8597426/enum-type-check-in-c-gcc
// https://stackoverflow.com/questions/62437717/how-does-c-compiler-treat-enum



class Statement : public Node {};

class Program : public Node
{
	private:
		Node_Ptr left;
		Node_Ptr right;

	public:
		Program(Node_Ptr Left, Node_Ptr Right) : left(Left), right(Right) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			left ->compile(output, content);
			right->compile(output, content);
		}
};

/* declarator */

/* base class */
class External_Declaration : public Node {};

class Declarator : public External_Declaration 
{
	public:
		virtual std::string get_variable_name() { return ""; }
		virtual int get_variable_size() { return 0; }

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const {}
		virtual void compile_declaration_initialisation(std::ostream &output, Context& content, type_def declarator_type, Expression* expressions, bool pointer_exist) const {}
		virtual void compile_declaration_array_initialisation(std::ostream &output, Context& content, type_def declarator_type, std::vector<Expression*>* expression_vector) const {}
};

class Declaration : public External_Declaration
{
	private:
		type_def TYPE;
		bool pointer_exist;
		std::vector<Declarator*>* declaration_list;

	public:
		Declaration(type_def _TYPE, std::vector<Declarator*>* _declaration_list = NULL) 
		: TYPE(_TYPE), pointer_exist(_TYPE.get_pointer_space()), declaration_list(_declaration_list) { }

		virtual std::string get_parameter() { return (*declaration_list)[0]->get_variable_name(); }

		type get_type() { return TYPE.get_variable_type(); }

		bool get_pointer_capable() {return pointer_exist; }

		virtual void compile(std::ostream &output, Context& content) const override
		{
			if (declaration_list != NULL)
			{				
				for (int i = 0; i < declaration_list->size(); i++)
				{					
					Declarator* temp_declarator = declaration_list->at(i);

					(*temp_declarator).compile_declaration(output, content, TYPE, pointer_exist);					
				}
			}
		}
};

class Function_def : public External_Declaration 
{
	private:
		type_def* TYPE;
		std::string ID;
		std::vector<Declaration*>*	parameter_list;
		Statement* statements;


	public:
		Function_def (type_def* _TYPE, std::string _ID, std::vector<Declaration*>* _parameter_list, Statement* _statements) 
		: TYPE(_TYPE), ID(_ID), parameter_list(_parameter_list), statements(_statements) {}

		virtual void compile(std::ostream& output, Context& content) const override
		{
			content.expand_context_scope();

			content.set_LOCALPOINTER();

			content.set_main_return_label("RETURN");

			output << "# ------------ Opening directives ------------ #" << std::endl;

			output << "\t" << ".text"  << std::endl;
			output << "\t" << ".globl" << "\t" << ID << std::endl;
			output << "\t" << ".ent"   << "\t" << ID << std::endl;
			output << "\t" << ".type"  << "\t" << ID <<", @function" << std::endl;

			output << "# ------------ Function call ------------ #" << std::endl;

			output <<  ID  << ":" << std::endl;

			output << "# ------------ add one stack frame ------------ #" << std::endl;

			output << "\t" << "sw"    << "\t" << "\t" << "$31,"	   << "-4"  << "($29)" << std::endl;
			output << "\t" << "sw"    << "\t" << "\t" << "$30,"	   << "-8" << "($29)" << std::endl;
			output << "\t" << "addiu" << "\t" << "$29,$29,"   		   << "-8" << std::endl; 
			output << "\t" << "move"  << "\t" << "$30,$29"    		   << std::endl;

			output << "# ------------ Program Assembly ------------ #" << std::endl;


			if(parameter_list != NULL)
			{
				int argument_stack_pointer = 4; 

				std::string argument_integer_registers[4]  = {"$4", "$5", "$6", "$7"};
				std::string argument_float_registers[4]    = {"$f12", "$f14"};
				std::string argument_double_registers[4]   = {"$f12", "$f13", "$f14", "$f15"};


				for(int i = 0; i < parameter_list->size(); i++)
				{
					argument_stack_pointer += 4;
					type argument_type = (*parameter_list)[i]->get_type();

					if ((argument_type == FLOAT) && (i < 2)) 
					{
						content.swc1_reg(output, argument_float_registers[i], argument_stack_pointer);
					}
					else if ((argument_type == DOUBLE) && (i < 2))
					{
						int register_index = (i + 1)*2;

						argument_stack_pointer += 4;
						content.swc1_reg(output, argument_double_registers[register_index - 2], argument_stack_pointer);
						
						argument_stack_pointer -= 4;
						content.swc1_reg(output, argument_double_registers[register_index - 1], argument_stack_pointer);

						argument_stack_pointer += 4;
					}
					else if (((argument_type == FLOAT) && (i < 4)) || ((argument_type == INT || argument_type == UNSIGNED_INT) && (i < 4)))
					{
						if (argument_type == FLOAT)
						{
							content.output_sw_register(output, argument_type, argument_integer_registers[i], "$30", argument_stack_pointer);	
						}
						else
						{
							content.output_sw_register(output, argument_type, argument_integer_registers[i], "$30", argument_stack_pointer);	
						}
					}

					content.make_new_argument((*parameter_list)[i]->get_parameter(), (*parameter_list)[i]->get_type(), NORMAL, argument_stack_pointer, (*parameter_list)[i]->get_pointer_capable());
				}

			}
			
			if(statements != NULL)
			{ 
				statements->compile(output, content);
			}
			
			if (ID == "main")
			{
            	output << "\t" << "move" << "\t" << "$2, $0" << std::endl; 
        	}
			
			output << content.function_return_label() << ":" << std::endl; 
					
			output << "# ------------ add stack ------------ #" << std::endl;

			output << "\t" << "move"  << "\t" << "$29, $30"  << std::endl; 
			output << "\t" << "addiu" << "\t" << "$29, $29," << "8" << std::endl; 
			output << "\t" << "lw"    << "\t" << "\t" << "$30," << "-8" << "($29)" << std::endl;
			output << "\t" << "lw"    << "\t" << "\t" << "$31," << "-4" << "($29)" << std::endl;
			output << "\t" << "j" 	   << "\t" << "\t" << "$31"  << std::endl;

			output << "# ------------ Closing directives ------------ #" << std::endl;

			output << "\t" << ".end" << "\t" << ID << std::endl;

			content.decreaseone_stack();
			content.shrink_context_scope();
			content.set_GLOBALPOINTER();
		}
};

class Variable_Declarator : public Declarator
{
	private:
		std::string variable_name;
		bool pointer_exist;

	public:
		Variable_Declarator(std::string _variable_name, bool pointer_exist) 
		: variable_name(_variable_name), pointer_exist(pointer_exist) {}

		Variable_Declarator(std::string _variable_name) 
		: variable_name(_variable_name) {}

		virtual std::string get_variable_name() { return variable_name; }
		virtual int get_variable_size() { return 4; }

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override 
		{
			type variable_type;
			int frame_pointer_1;

			if (pointer_exist)
			{
				variable compile_variable = content.new_variable(variable_name, declarator_type, NORMAL, pointer_exist);
				variable_type = compile_variable.get_variable_type();
				frame_pointer_1 = compile_variable.get_variable_location();
			}
			else
			{
				variable compile_variable = content.new_variable(variable_name, declarator_type, NORMAL);
				variable_type = compile_variable.get_variable_type();
				frame_pointer_1 = compile_variable.get_variable_location();
			}

			content.output_sw_register(output, variable_type, "$0", "$30", frame_pointer_1);
		}

		virtual void compile_declaration_initialisation(std::ostream &output, Context& content, type_def declarator_type, Expression* expressions, bool pointer_exist) const override 
		{
			variable declared_variable = content.new_variable(variable_name, declarator_type, NORMAL, pointer_exist);

			if(content.get_context_scope() == LOCALPOINTER)
			{
				content.addone_stack();
				int frame_pointer_1 = content.get_stack_pointer();
				std::string temp_register_1 = "$8";
				
				expressions->compile(output, content);

				content.decreaseone_stack();

				content.lw_register(output, temp_register_1, frame_pointer_1);
				content.output_sw_register(output, declarator_type.get_variable_type(), temp_register_1, "$30", declared_variable.get_variable_location());
			}
			else if(content.get_context_scope() == GLOBALPOINTER)
			{
				int expression = 0;

				if (expressions != NULL)
				{
					expression = expressions->evaluate();
				}
			
				output << "# ------------ Global declaration ------------ #" << std::endl;
				output << "\t" << ".globl" << "\t" << variable_name << std::endl;
				output << "\t" << ".data" << std::endl;
				output <<  variable_name  << ":" << std::endl;
				output << "\t" << "." << "word" << " " << expression << std::endl;
			}
		}
};


class Array_Declarator : public Declarator 
{
	private: 
		std::string variable_name;
		int array_size;

	public:
		Array_Declarator(std::string _variable_name, Expression *_array_size_expression) 
		{
			variable_name = _variable_name;

			if(_array_size_expression != NULL) { array_size = _array_size_expression->evaluate(); }
			else { array_size = -1; }
		}

		virtual std::string get_variable_name() { return variable_name; }
		virtual int get_variable_size() { return array_size; }

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int array_frame_pointer = 0;

			variable array_variable = content.get_variable(variable_name);

			if(content.get_context_scope() == LOCALPOINTER)
			{
				output << "\t" << "# Store array content locally" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{				
					array_frame_pointer =  array_size + (i*4);
					content.output_sw_register(output, array_variable.get_variable_type(), "$0", "$30", array_frame_pointer);
				}
			}
			else
			{
				output << "\t" << "# Store array content globally" << std::endl; 
				output << "\t" << ".globl" << "\t" << variable_name << std::endl;
				output << "\t" << ".data"  << std::endl;

				output <<  variable_name 	<< ":" << std::endl;

				for(int i = 0; i < array_size; i++)
				{				
					output << "\t" << ".space " << array_size*4 << std::endl;
				}
			}

			
		}

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override 
		{
			declarator_type.increase_array_track();

			variable array_variable = content.new_variable(variable_name, declarator_type, ARRAY, 0, array_size);
			
			int array_frame_pointer = 0;

			if(content.get_context_scope() == LOCALPOINTER)
			{
				output << "\t" << "# Store array content" << std::endl; 

				for(int i = 0; i < array_size; i++)
				{
					array_frame_pointer = array_variable.get_variable_location() + (i*4);
					content.output_sw_register(output, declarator_type.get_variable_type(), "$0", "$30", array_frame_pointer);
				}
			}
			else
			{
				output << "\t" << "# Store array content globally" << std::endl; 
				output << "\t" << ".comm " << variable_name << " " << array_size*4 << std::endl;
			}
		}	

		virtual void compile_declaration_array_initialisation(std::ostream &output, Context& content, type_def declarator_type, std::vector<Expression*>* expression_vector) const 
		{
			int initialisation_vector_size = expression_vector->size();

			variable array_variable = content.new_variable(variable_name, declarator_type, ARRAY, initialisation_vector_size);

			for(int i = 0; i < array_size; i++)
			{
				if (i < initialisation_vector_size)
				{
					int array_offset = array_variable.get_variable_location() + (i*4);

					content.addone_stack();
					int stack_pointer = content.get_stack_pointer();
					std::string init_register = "$8";

					(*expression_vector)[i]->compile(output, content);

					content.decreaseone_stack();

					content.lw_register(output, init_register, stack_pointer);
					content.output_sw_register(output, declarator_type.get_variable_type(), init_register, "$30", array_offset);
				}
				else
				{
					int array_offset = array_variable.get_variable_location() + (i*4);
					content.output_sw_register(output, declarator_type.get_variable_type(), "$0", "$30", array_offset);
				}
			}
		}
};

/* initilialisation */

class Initialisation_Variable_Declarator : public Declarator 
{
	private: 
		Declarator* initialisation_declarator;
		Expression* initialisation_expressions;

	public: 
		Initialisation_Variable_Declarator(Declarator* _initialisation_declarator, Expression* _initialisation_expressions)
		: initialisation_declarator(_initialisation_declarator), initialisation_expressions(_initialisation_expressions) {}

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override
		{
			initialisation_declarator->compile_declaration_initialisation(output, content, declarator_type, initialisation_expressions, pointer_exist);
		}
};

class Initialisation_Array_Declarator : public Declarator 
{
	private: 
		Declarator* initialisation_declarator;
		std::vector<Expression*>* initialisation_vector;

	public: 
		Initialisation_Array_Declarator(Declarator* _initialisation_declarator, std::vector<Expression*>* _initialisation_vector)
		: initialisation_declarator(_initialisation_declarator), initialisation_vector(_initialisation_vector) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			std::string initialisation_variable_name = initialisation_declarator->get_variable_name();
			int initialisation_array_size = initialisation_declarator->get_variable_size();

			variable array_variable = content.get_variable(initialisation_variable_name);

			for(int i = 0; i < initialisation_array_size; i++)
			{				
				int array_offset = array_variable.get_variable_location() + (i*4);

				content.addone_stack();
				int stack_pointer = content.get_stack_pointer();
				std::string init_register = "$8";

				(*initialisation_vector)[i]->compile(output, content);

				content.decreaseone_stack();

				content.lw_register(output, init_register, stack_pointer);
				content.output_sw_register(output, array_variable.get_variable_type(), init_register, "$30", array_offset);
			}
		}

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override
		{
			initialisation_declarator->compile_declaration_array_initialisation(output, content, declarator_type, initialisation_vector);
		}
};

/* declaration */



class Function_Prototype_Declaration : public Declarator
{
	private:
		std::string function_name;
		std::vector<Declaration*>* parameter_list;
	
	public:
		Function_Prototype_Declaration(std::string _function_name, std::vector<Declaration*>* _parameter_list)
		: function_name(_function_name), parameter_list(_parameter_list) {}

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override
		{ content.new_variable(function_name, declarator_type, FUNCTION); }
};

/* enumeration */

class Initialisation_Enum_Declarator : public Declarator 
{
	private: 
		Declarator* initialisation_declarator;
		Expression* initialisation_expressions = NULL;

	public: 
		Initialisation_Enum_Declarator(Declarator* _initialisation_declarator, Expression* _initialisation_expressions)
		: initialisation_declarator(_initialisation_declarator), initialisation_expressions(_initialisation_expressions) {}

		Initialisation_Enum_Declarator(Declarator* _initialisation_declarator)
		: initialisation_declarator(_initialisation_declarator) {}

		virtual void compile_declaration(std::ostream &output, Context& content, type_def declarator_type, bool pointer_exist) const override
		{ initialisation_declarator->compile_declaration_initialisation(output, content, declarator_type, initialisation_expressions, false); }
};

#endif