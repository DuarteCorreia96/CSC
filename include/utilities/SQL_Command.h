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

	/// <summary>
	/// <par> Parses the field "WHERE" of a SQL command returning a Json::Value with result. </par>
	/// <par> It returns with a "valid" key to check if it was correctly parsed. </par>
	/// </summary>
	/// <param name="command_vector"> Vector of the full comand with 1 word per position. </param>
	/// <param name="where_index"> Index of the keyword "WHERE" in the command vector. </param>
	/// <returns></returns>
	Json::Value parse_conditions(std::vector<std::string> command_vector, unsigned __int64 where_index);
	
	/// <summary>
	/// Transforms a string to uppercase.
	/// </summary>
	/// <param name="aux_string"> string to be transformed. </param>
	void to_uppercase_str(std::string& aux_string);

	/// <summary>
	/// Transforms a position of a vector of strings to uppercase.
	/// </summary>
	/// <param name="command_vector"> Vector containing the string. </param>
	/// <param name="index"> Index of the string to be transformed. </param>
	void to_uppercase( std::vector<std::string> &command_vector, unsigned __int64 index);

	/// <summary>
	/// Removes special characters from a position of a vector of strings.
	/// </summary>
	/// <param name="command_vector"> Vector containing the string. </param>
	/// <param name="index"> Index of the string to be transformed. </param>
	void rem_spec_char(std::vector<std::string> &command_vector, unsigned __int64 index);	

public:

	SQL_Command(std::string command);

	/// <summary>
	/// Parse a SQL Command.
	/// </summary>
	/// <param name="command"> String of the command to be parsed. </param>
	void parse(std::string command);


	void select(std::vector<std::string> command_vector);
	void create_table(std::vector<std::string> command_vector);
	void insert_table(std::vector<std::string> command_vector);
	void delete_from (std::vector<std::string> command_vector);
	
	Json::Value get_command_json() { return command_json; };
};
