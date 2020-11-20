#include <iostream>
#include <string>
#include <vector>

#include "SQL_Command.h"


int main() {

    std::string command = "SELECT mult(coluna123) FROM table1 WHERE col1 > 10;";
	SQL_Command function{ command };

	std::cout << command << std::endl;
	
	//std::cout << "Colunas:" << std::endl;
	//for (auto& element : function.get_columns()) {
	//	std::cout << "\t" << element << std::endl;
	//}

}