#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map> 
#include <cctype>
#include <regex>

class SQL_Command {

private:
	std::string function, table;
	std::vector<std::string> columns{};
	
	void parse_columns(std::vector<std::string> command_vector);

public:

	SQL_Command(std::string command);

	std::vector <std::string> get_columns() { return columns; };
	std::string get_function() { return function; }
	std::string get_table() { return table; }
	
	void select(std::vector<std::string> command_vector);
	void create_table(std::vector<std::string> command_vector);
	void insert_table(std::vector<std::string> command_vector);
	void delete_from (std::vector<std::string> command_vector);
};

typedef void (SQL_Command::* sql_t)(std::vector<std::string> command_vector);
typedef std::map<std::string, sql_t> sql_map_t;