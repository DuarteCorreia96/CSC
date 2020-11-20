#include <iostream>
#include <string>
#include <vector>

#include "SQL_Command.h"


int main() {

    std::string command  = "SELECT mult(coluna123) FROM table1 WHERE col1 > 10;";
	std::string command2 = "CREATE TABLE tablename (col1name TEXT, col2name INT, col3name TEXT)";

	std::string command_used = command2;
	SQL_Command function{ command_used };

	std::cout << std::endl <<command_used << std::endl;
	
	//std::cout << "Colunas:" << std::endl;
	//for (auto& element : function.get_columns()) {
	//	std::cout << "\t" << element << std::endl;
	//}

}