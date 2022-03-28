#ifndef FIVEAST_CONTEXT_HPP
#define FIVEAST_CONTEXT_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <regex>
#include <stack>
#include <cassert>
#include <deque>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <initializer_list>

/*

Number      Name Use
0           $zero       hardwired 0 value
1           $at         used by assembler (pseudo-instructions)
2-3         $v0-1       subroutine return value
4-7         $a0-3       arguments: subroutine parameter value
8-15        $t0-7       temp: can be used by subroutine without saving
16-23       $s0-7       saved: must be saved and restored by subroutine
24-25       $t8-9       temp
26-27       $k0-1       kernel: interrupt/trap handler
28          $gp         global pointer (static or extern variables)
29          $sp         stack pointer
30          $fp         frame pointer
31          $ra         return address for subroutine


 https://people.cs.rutgers.edu/~pxk/419/notes/frames.html frame pointer and stack pointer
 https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture15/C_code.pdf register distribution

*/
class Expression;
class Statement;

/* type def */
	enum context_scope
	{
		GLOBALPOINTER, 
		LOCALPOINTER
	};
	enum declaration_type
	{
		NORMAL,
		FUNCTION,
		ARRAY,
		POINTER,
		STRUCT
	};

	enum storage_type
	{
		WORD,
		HALF,
		BYTE
	};	

	enum type
	{
		VOID,
		INT,
		FLOAT,
		STRING,
		CHAR,
		DOUBLE,
		UNSIGNED_INT,
		NONE
	};


class type_def
{
	private: 

		type variable_type;
		bool pointer_exist=0;

		unsigned int pointer_track = 0;
		unsigned int array_track = 0;

	public:
		type_def(type variable_type) : variable_type(variable_type) {}
		type_def(std::string pointer_flag, type_def pointer_type) 
		{			
			if (pointer_flag == "POINTER")
			{
				pointer_exist = 1;
				variable_type = pointer_type.get_variable_type();
				pointer_track++;
			}
		}

		type get_variable_type() { return variable_type; }
		
		void increase_array_track() { array_track++; };
		void decrease_array_track() { array_track--; };
		
		void increase_pointer_track() { pointer_track++; };
		void decrease_pointer_track()		 
		{ 
			if (pointer_track != 0) { array_track--; }
			else { pointer_track--; }
		};
		
		void set_pointer_space() { pointer_exist = 1; }
		bool get_pointer_space() { return pointer_exist; }
};

class variable
{
	private: 
		int variable_address;
		type_def variable_type;
		bool pointer_exist=0;
		declaration_type variable_declaration;
		context_scope variable_scope = GLOBALPOINTER;

	public:
		variable (int Variable_address, context_scope Variable_scope, declaration_type variable_declaration, type_def variable_type) 
		: variable_address(Variable_address), variable_scope(Variable_scope), variable_declaration(variable_declaration), variable_type(variable_type) {}

		variable (int Variable_address, context_scope Variable_scope, declaration_type variable_declaration, type_def variable_type, bool pointer_exist) 
		: variable_address(Variable_address), variable_scope(Variable_scope), variable_declaration(variable_declaration), variable_type(variable_type), pointer_exist(pointer_exist) {}

		int get_variable_location() { return variable_address; }
		type get_variable_type() { return variable_type.get_variable_type(); }		
		type_def get_type_def() { return variable_type; }	
		bool get_pointer_space() { return pointer_exist; }
		declaration_type get_declaration_type() { return variable_declaration; }			
		context_scope get_variable_space() { return variable_scope; }				
							
};


static context_scope scope_track;
typedef std::map<std::string, variable*> map_type;
typedef std::map<int, variable*> map_variable;

class Context
{
	private:

		int stack_pointer = 0;
		int register_counter = 0;

		int label_counter = 0;
		std::string main_function_return_label; 

		std::stack<std::string> break_stack;
		std::stack<std::string> continue_stack;

		std::stack<int> context_scope_frame_pointer;
		std::stack<map_type*> context_scope_stack_track;

		map_type* context_track = new map_type();
		map_variable* address_track = new map_variable();

		std::deque<std::string> switch_label_track;
		std::deque<Expression*> switch_statements_track;
		
		std::map<std::string, std::string> label_variables;
		std::map<std::string, std::string> label_declarations;

	public:

		bool check_function_declared(std::string NAME)
		{
			if((*context_track).count(NAME)) { return true; }
			else { return false; }
		}

/* switch */
		void add_case_statements(Expression* CASE_STATEMENT, std::string CASE_LABEL) 
		{ 
			switch_statements_track.push_back(CASE_STATEMENT);
			switch_label_track.push_back(CASE_LABEL);
		}
		
		void add_case_label(std::string CASE) 
		{ 
			switch_label_track.push_back(CASE);
		}

		Expression* get_case_statement()
		{
			return switch_statements_track.front();
		}

		std::string get_case_label()
		{
			return switch_label_track.front();
		}

		int get_case_statement_size()
		{
			return switch_statements_track.size();
		}

		int get_case_label_size()
		{
			return switch_label_track.size();
		}

		void remove_case_statement() 
		{
			switch_statements_track.pop_front();
		}

		void remove_label_statement() 
		{
			switch_label_track.pop_front();
		}

/* break and contiune */
		void add_break_label(std::string BREAK_LABEL) { break_stack.push(BREAK_LABEL); }

		void add_continue_label(std::string CONTINUE_LABEL) { continue_stack.push(CONTINUE_LABEL); }

		std::stack<std::string> get_break_label() { return break_stack; }
		
		std::stack<std::string> get_continue_label() { return continue_stack; }

		void remove_break_label() { break_stack.pop(); }

		void remove_continue_label() { continue_stack.pop(); }
		
		std::string function_return_label() { return main_function_return_label; }

/* return label*/
		void set_main_return_label(std::string RETURN_LABEL)
		{
			main_function_return_label = RETURN_LABEL + "_" + std::to_string(++label_counter);
		}

		std::string make_label(std::string RETURN_LABEL)
		{
			return RETURN_LABEL + "_" + std::to_string(++label_counter);
		}

/* scope */
		context_scope get_context_scope() { return scope_track; }

		void set_LOCALPOINTER()  { scope_track = LOCALPOINTER; }

		void set_GLOBALPOINTER() { scope_track = GLOBALPOINTER; }

		void expand_context_scope()
		{
			context_scope_frame_pointer.push(stack_pointer);

			context_scope_stack_track.push(context_track);
			context_track = new map_type(*context_track);
		}

		void shrink_context_scope()
		{
			delete context_track;

			stack_pointer = context_scope_frame_pointer.top();
			context_track = context_scope_stack_track.top();

			context_scope_frame_pointer.pop();
			context_scope_stack_track.pop();
		}

/* stack */
		void addone_stack()
		{
			register_counter++;
			stack_pointer -= 4;
		}

		void decreaseone_stack()
		{
			if (register_counter != 0)
			{
				stack_pointer += 4;
				register_counter--;
			}
		}

		int get_stack_pointer() { return stack_pointer; }

/* register lw and sw*/
		void lw_register(std::ostream& output, std::string NAME, int LOCATION)
		{
			output << "\t" << "lw" << "\t" << "\t" << NAME << "," << LOCATION << "($30)" << std::endl;
		}

		void sw_register(std::ostream& output, std::string NAME, int LOCATION)
		{
			output << "\t" << "sw" << "\t" << "\t" << NAME << "," << LOCATION << "($30)" << std::endl;
		}

		void output_lw_register(std::ostream& output, type TYPE, std::string REG1, std::string REG2, int OFFSET)
		{
			output << "\t" << "lw" << "\t" << "\t" << REG1 << "," << OFFSET << "(" << REG2 << ")" << std::endl;
		}

		void output_sw_register(std::ostream& output, type TYPE, std::string REG1, std::string REG2, int OFFSET)
		{
			output << "\t" << "sw" << "\t" << "\t" << REG1 << "," << OFFSET << "(" << REG2 << ")" << std::endl;
		}

/* variable */		
		variable new_variable(std::string NAME, type_def TYPE, declaration_type DTYPE, bool POINTER_EXIST = 0, int VARIABLE_SIZE = 1)
		{
			if (scope_track == LOCALPOINTER)
			{
				if (DTYPE == ARRAY) { TYPE.decrease_pointer_track(); }
				stack_pointer -= VARIABLE_SIZE*(4);
			}

			if (POINTER_EXIST)
			{
				(*context_track)[NAME] = new variable(stack_pointer, scope_track, DTYPE, TYPE, POINTER_EXIST);
				(*address_track)[stack_pointer] = new variable(stack_pointer, scope_track, DTYPE, TYPE, POINTER_EXIST);
			}
			else
			{
				(*address_track)[stack_pointer] = new variable(stack_pointer, scope_track, DTYPE, TYPE, POINTER_EXIST);
				(*context_track)[NAME] = new variable(stack_pointer, scope_track, DTYPE, TYPE);
			}		
			return *((*context_track)[NAME]);
		}

		variable get_variable(std::string NAME)
		{
			if((*context_track).count(NAME))
			{
				return *((*context_track)[NAME]);
			}
		}

		bool get_pointer_space(std::string NAME)
		{
			return (*context_track)[NAME]->get_pointer_space();
		}

		void pointer_shift(std::ostream& output, type_def TYPE, int STACK_POINTER)
		{
			std::cerr<< TYPE.get_pointer_space() <<std::endl;

			if ((TYPE.get_pointer_space()) && (TYPE.get_variable_type() == INT))
			{
				output << "\t" << "sll" << "\t" << "\t" << "$2" << "," << "$2" << "," << 2 << std::endl;
				sw_register(output, "$2", STACK_POINTER);
			}
			
		}

/*argument*/
		void make_new_argument(std::string NAME, type_def TYPE, declaration_type DECLARATION, int ADDRESS, bool pointer_exist = 0)
		{ 
			(*context_track)[NAME] = new variable(ADDRESS, LOCALPOINTER, DECLARATION, TYPE, pointer_exist);
		}

/* float*/
		void mtc1_reg(std::ostream& output, std::string REG1, std::string REG2)
		{
			output << "\t" << "mtc1"  << "\t" << REG1 << "," << REG2 << std::endl;
		}

		void mfc1_reg(std::ostream& output, std::string REG1, std::string REG2)
		{
			output << "\t" << "mfc1"  << "\t" << REG1 << "," << REG2 << std::endl;
		}

		void lwc1_reg(std::ostream& output, std::string NAME, int POSITION)
		{
			output << "\t" << "lwc1" << "\t" << NAME << "," << POSITION << "($30)" << std::endl;
		}

		void swc1_reg(std::ostream& output, std::string NAME, int POSITION)
		{
			output << "\t" << "swc1" << "\t" << NAME << "," << POSITION << "($30)" << std::endl;
		}

};

/* node */
class Node
{
	public:
		virtual ~Node () {}

		virtual void compile(std::ostream& output, Context& content) const 
		{
			std::cerr << "Ast_node.hpp: 'compile' not implemented" << std::endl;
		}
};

typedef const Node* Node_Ptr;

#endif