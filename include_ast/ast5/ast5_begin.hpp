#ifndef FIVEAST_beginS_HPP
#define FIVEAST_beginS_HPP

#include <iomanip>
#include <string>

/* base class */
class begin : public Expression {};

/* numberr */
class Unsigned_Integer : public begin
{
	private:
		unsigned int number;

	public:
		Unsigned_Integer (int Number) : number(Number) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$2";

			output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << number << std::endl;

			content.sw_register(output, dest_reg, stack_pointer);	
		}

		virtual type get_value_type(Context& content) const override { return type(UNSIGNED_INT); };

		virtual double evaluate() const override { return number; };
};

class Double : public begin
{
	private:
		double number;

	public: 
		Double (float Number) : number(Number) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$f0";

			output << "\t" << "li.d" << "\t" << "\t" << dest_reg << ", " << number << std::endl;

			content.swc1_reg(output, dest_reg, stack_pointer);	
		}

		virtual type get_value_type(Context& content) const override { return type(DOUBLE); };

		virtual double evaluate() const override { return number; };
};

class Integer : public begin
{
	private:
		int number;

	public:
		Integer (int Number) : number(Number) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$2";

			output << "\t" << "li" << "\t" << "\t" << "$2" << ", " << number << std::endl;

			content.sw_register(output, dest_reg, stack_pointer);	
		}

		virtual type get_value_type(Context& content) const override { return type(INT); };

		virtual double evaluate() const override { return number; };
};

class Float : public begin
{
	private:
		float number;

	public: 
		Float (float Number) : number(Number) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{
			int stack_pointer = content.get_stack_pointer();
			std::string dest_reg = "$f0";

			output << "\t" << "li.s" << "\t" << "\t" << dest_reg << ", " << number << std::endl;

			content.swc1_reg(output, dest_reg, stack_pointer);	
		}

		virtual type get_value_type(Context& content) const override { return type(FLOAT); };

		virtual double evaluate() const override { return number; };
};

/* not number */
class Identifier : public begin
{
	private:
		std::string variable_name;

	public:
		Identifier (std::string _variable_name) : variable_name (_variable_name) {}

		virtual void compile(std::ostream &output, Context& content) const override
		{	
			int dest_int = content.get_stack_pointer();

			output << "\t" << "# Access variable" << std::endl;
			
			std::string dest_reg = "$2";

			load_variable_address(output, content);

			variable compile_variable = content.get_variable(variable_name);

			if(compile_variable.get_declaration_type() != ARRAY)
			{
				content.output_lw_register(output, INT, dest_reg, dest_reg, 0);
				content.sw_register(output, dest_reg, dest_int);
			}
		}

		virtual std::string get_variable_name() const override { return variable_name; }; 

		virtual void load_variable_address(std::ostream &output, Context& content) const override
		{
			output << "\t" << "# Load variable" << std::endl;

			int dest_int = content.get_stack_pointer();
			std::string dest_reg = "$2";
			variable compile_variable = content.get_variable(variable_name);

			if (compile_variable.get_variable_space() == GLOBALPOINTER)
			{
				output << "\t" << "la" << "\t" << "\t" << dest_reg << "," << variable_name << std::endl;
			}
			else 
			{
				output << "\t" << "addiu" << "\t" << dest_reg << ",$30," << compile_variable.get_variable_location() << std::endl;
			}

			content.sw_register(output, dest_reg, dest_int);
		}

		virtual type get_value_type(Context& content) const { return content.get_variable(variable_name).get_variable_type(); };

		virtual type_def get_type_def(Context& content) const override { return content.get_variable(variable_name).get_type_def(); };

		virtual bool get_pointer_space(Context& content) const { return content.get_variable(variable_name).get_pointer_space(); };
};


#endif