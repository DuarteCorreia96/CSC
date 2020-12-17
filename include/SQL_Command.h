#pragma once

#include "Definitions.h"

#include <string>
#include <vector>
#include <map>
#include <json/json.h>

class SQL_Command;
typedef void (SQL_Command::* sql_t)(std::vector<std::string> command_vector);
typedef std::map<std::string, sql_t> sql_map_t;


/// <summary> 
/// <para> <c>SQL_Command</c> is used to parse a SQL command! </para> 
/// <para> Checks the validity of the command and dvides tokens into vectors or strings. </para>
/// </summary>
/// <param name="command"> String of the command to be parsed. </param>
class SQL_Command {

private:
	
	Json::Value command_json;

	sql_map_t funcMap = {
		{ "CREATE", &SQL_Command::create_table },
		{ "INSERT", &SQL_Command::insert_table },
		{ "DELETE", &SQL_Command::delete_from  },
		{ "SELECT", &SQL_Command::select       }
	};

	Json::Value parse_conditions(std::vector<std::string> command_vector, unsigned __int64 start_index);
	
	void to_uppercase_str(std::string& aux_string);
	void to_uppercase( std::vector<std::string> &command_vector, unsigned __int64 index);
	void rem_spec_char(std::vector<std::string> &command_vector, unsigned __int64 index);	

public:

	SQL_Command(std::string command);

	void parse(std::string command);
	void select(std::vector<std::string> command_vector);
	void create_table(std::vector<std::string> command_vector);
	void insert_table(std::vector<std::string> command_vector);
	void delete_from (std::vector<std::string> command_vector);
	
	Json::Value get_command_json() { return command_json; };
};



