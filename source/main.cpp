#include <iostream>
#include <string>
#include <vector>

#include "SQL_Command.h"


int main() {

    std::string command  = "SELECT mult(coluna123) FROM table1 WHERE col1 > 10;";
	std::string select_c = "SELECT col1, col2, col3 FROM tablename WHERE col1 > 2 AND col3 < 1";
	std::string command2 = "CREATE TABLE tablename (col1name TEXT, col2name INT, col3name TEXT)";
	std::string delete_c = "DELETE FROM tablename WHERE col1name = value1 OR col2name > value2";
	std::string insert_c = "INSERT INTO TABLE tablename (col1, col2) VALUES (1, 2)";

	std::string command_used = select_c;
	SQL_Command function{ command_used };

	if (not function.check_command()) {
		return -1;
	}

	std::cout << "Function:\t" << function.get_function() << std::endl;
	std::cout << "Table:   \t" << function.get_table()    << std::endl;
	std::cout << "Operator:\t" << function.get_operator() << std::endl;

	function.get_condition1().print();
	function.get_condition2().print();

	std::vector<std::string> columns = function.get_columns();
	std::vector<std::string> columns_v = function.get_columns_values();

	std::cout << "Colunas:" << std::endl;
	for (unsigned __int64 i = 0; i < columns.size(); i++) {
		std::cout << "Coluna:\t\t" << columns[i] << "\t Value:\t" << columns_v[i] << std::endl;
	}

	std::cout << std::endl <<command_used << std::endl;
}