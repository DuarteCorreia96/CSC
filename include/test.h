#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include "SQL_Command.h"
#include <seal/seal.h>

#include <fstream>
#include <json/json.h>


void test_json() {
    JSONCPP_STRING err;
    Json::Value root;
    Json::CharReaderBuilder rbuilder;

    std::ifstream config_doc("data/test.json");
    Json::parseFromStream(rbuilder, config_doc, &root, &err);
    config_doc.close();

    std::cout << "Book: " << root["book"].asString() << std::endl;
    std::cout << "Year: " << root["year"].asUInt() << std::endl;
    const Json::Value& characters2 = root["characters"]; // array of characters
    for (unsigned int i = 0; i < characters2.size(); i++) {
        std::cout << "    name: " << characters2[i]["name"].asString();
        std::cout << " chapter: " << characters2[i]["chapter"].asUInt();
        std::cout << std::endl;
    }

    //Json::StreamWriterBuilder wbuilder;
    //// Configure the Builder, then ...
    //std::string outputConfig = Json::writeString(wbuilder, root);
}

int test_SQL_Command() {

    std::string command  = "SELECT mult(coluna123) FROM table1 WHERE col1 > 10;";
	std::string select_c = "SELECT col1, col2, col3 FROM tablename WHERE col1 > 2 AND col3 < 1";
	std::string command2 = "CREATE TABLE tablename (col1name TEXT, col2name INT, col3name TEXT)";
	std::string delete_c = "DELETE FROM tablename WHERE col1name = value1 OR col2name > value2";
	std::string insert_c = "INSERT INTO TABLE tablename (col1, col2) VALUES (1, 2)";

	std::string command_used = insert_c;
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

	if (columns.size() == columns_v.size()) {
		for (unsigned __int64 i = 0; i < columns.size(); i++) {
			std::cout << "\t\t" << columns[i] << "\t <- \t" << columns_v[i] << std::endl;
		}
	} else {
		for (unsigned __int64 i = 0; i < columns.size(); i++) {
			std::cout << "\t\t" << columns[i] << std::endl;
		}
	}

	std::cout << std::endl <<command_used << std::endl;

	return 0;
}

