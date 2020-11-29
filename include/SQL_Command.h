#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map> 
#include <cctype>
#include <regex>

/// <summary>
/// Struct used to save a SQL condition.
/// </summary>
/// <member name="operation"> </member>
struct SQL_Condition {

	char operation = '\0';
	std::string variable;
	std::string value;

	void print() { std::cout << "Condition: \t" << variable << " " << operation << " " << value << std::endl; };
};

/// <summary> 
/// <para> <c>SQL_Command</c> is used to parse a SQL command! </para> 
/// <para> Checks the validity of the command and dvides tokens into vectors or strings. </para>
/// </summary>
/// <param name="command"> String of the command to be parsed. </param>
class SQL_Command {

private:

	std::string function, table;
	std::vector<std::string> columns{};
	std::vector<std::string> columns_type{};
	std::vector<std::string> columns_values{};

	SQL_Condition condition1;
	SQL_Condition condition2;
	SQL_Condition parse_condition(std::vector<std::string> command_vector, unsigned __int64 start_index);

	char operator_cond = '\0';
	char select_function = '\0';
	
	void to_uppercase_str(std::string& aux_string);
	void to_uppercase( std::vector<std::string>& command_vector, unsigned __int64 index);
	void rem_spec_char(std::vector<std::string> &command_vector, unsigned __int64 index);	

	bool all_ok = false;

public:

	SQL_Command(std::string command);

	void parse(std::string command);
	void select(std::vector<std::string> command_vector);
	void create_table(std::vector<std::string> command_vector);
	void insert_table(std::vector<std::string> command_vector);
	void delete_from (std::vector<std::string> command_vector);

	std::vector <std::string> get_columns() { return columns; };
	std::vector <std::string> get_columns_values() { return columns_values; };
	std::string get_function() { return function; }
	std::string get_table() { return table; }
	char get_operator() { return operator_cond; };

	SQL_Condition get_condition1() { return condition1; };
	SQL_Condition get_condition2() { return condition2; };
	
	bool check_command() { return all_ok; }
};

typedef void (SQL_Command::* sql_t)(std::vector<std::string> command_vector);
typedef std::map<std::string, sql_t> sql_map_t;

