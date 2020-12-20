#include "utilities/SQL_Command.h"

#include <iostream>
#include <stdlib.h>    
#include <string>

#include <sstream>
#include <algorithm>
#include <fstream>


SQL_Command::SQL_Command(std::string command) {
    parse(command);
}

void
SQL_Command::parse(std::string command) {

    command_json["valid"] = false;

    std::string token;
    std::istringstream tokenStream(command);
    std::vector<std::string> command_vector{};
    while (std::getline(tokenStream, token, ' ')) {
        command_vector.push_back(token);
    }

    to_uppercase(command_vector, 0);
    std::string function = command_vector[0];
    sql_map_t::iterator x = funcMap.find(function);

    if (x != funcMap.end()) {
        (this->*(x->second))(command_vector);
    }

    if (command_json["valid"].asBool()) {
        std::cout << "Command correctly parsed!" << std::endl;
    }
    else {
        std::cout << "Command not valid!" << std::endl;
    }
}

void
SQL_Command::to_uppercase(std::vector<std::string>& command_vector, unsigned __int64 index) {
    std::transform(command_vector[index].begin(), command_vector[index].end(), command_vector[index].begin(), ::toupper);
}

void
SQL_Command::to_uppercase_str(std::string &aux_string) {
    std::transform(aux_string.begin(), aux_string.end(), aux_string.begin(), ::toupper);
}

void
SQL_Command::rem_spec_char(std::vector<std::string> &command_vector, unsigned __int64 index) {
    
    std::vector<char> spec_chars{ ',','(',')' };
    for (auto& spec_char : spec_chars) {
        command_vector[index].erase(std::remove(command_vector[index].begin(), command_vector[index].end(), spec_char), command_vector[index].end());
    }
}

void 
SQL_Command::select(std::vector<std::string> command_vector) {

    std::string aux_str;
    unsigned __int64 from_index = 0;
    unsigned __int64 where_index = 0;

    // Check if syntax is correct before parsing
    for (unsigned __int64 i = 0; i < command_vector.size(); i++) {

        aux_str = command_vector[i];
        to_uppercase_str(aux_str);
        if (aux_str.compare("FROM") == 0) {
            from_index = i;
        }

        if (aux_str.compare("WHERE") == 0) {
            where_index = i;
            break;
        }
    }

    if (not from_index) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    aux_str = command_vector[1];
    to_uppercase_str(aux_str);
    if (command_vector[1].size() > 5 && aux_str.compare(0, 4, "SUM(") == 0 && aux_str.back() == ')') {
            
        command_json["columns"][0] = command_vector[1].substr(4, command_vector[1].size() - 5);
        command_json["function"] = "SELECT_SUM";

    } else if (aux_str.compare("LINE") == 0) {

        if (command_vector.size() > 5) {
            return;
        }

        command_json["function"] = "SELECT_LINE";
        try {
            command_json["linenum"] = std::stoi(command_vector[2]);
        }
        catch (std::exception e) { return; }

    } else {

        command_json["function"] = "SELECT";
        for (int i = 1; i < from_index; i++) {

            rem_spec_char(command_vector, i);
            command_json["columns"][i - 1] = command_vector[i];
        }
    }

    if (where_index) {

        Json::Value valid;

        command_json["where"] = parse_conditions(command_vector, where_index);
        command_json["where"].removeMember("valid", &valid);
        if (not valid.asBool()) { return; }
    }

    command_json["table"] = command_vector[from_index + 1];
    command_json["valid"] = true;
}

void
SQL_Command::create_table(std::vector<std::string> command_vector) {

    to_uppercase(command_vector, 1);
    if (command_vector[1].compare("TABLE") != 0 || command_vector.size() < 4 ||
        command_vector[3][0] != '(' || command_vector[command_vector.size()-1].back() != ')') {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    int index = 3;
    while (command_vector.size() > index) {

        rem_spec_char(command_vector, index);
        command_json["columns"][index - 3] = command_vector[index];

        index++;
    }

    command_json["function"] = "CREATE";
    command_json["table"] = command_vector[2];
    command_json["valid"] = true;
}

void
SQL_Command::insert_table(std::vector<std::string> command_vector) {

    to_uppercase(command_vector, 1);
    if (command_vector[1].compare("INTO") != 0 != 0 
        || command_vector.size() < 6 || command_vector.size() % 2 != 0) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    int values_index = 0;
    for (int i = 3; i < command_vector.size(); i++) {

        std::string aux_str = command_vector[i];
        to_uppercase_str(aux_str);
        if (aux_str.compare("VALUES") == 0) {

            values_index = i;
            break;
        }
    }

    int column_size = values_index - 3;
    int values_size = command_vector.size() - values_index - 1;
    if (not values_index || column_size != values_size) {
        std::cout << "Missing Values! Function not valid!" << std::endl;
        return;
    }

    for (int i = 3, j = 0; i < values_index; i++, j++) {

        rem_spec_char(command_vector, i);
        command_json["columns"][j] = command_vector[i];
    }

    for (int i = values_index + 1, j = 0; i < command_vector.size(); i++, j++) {
        try {

            rem_spec_char(command_vector, i);
            command_json["values"][j] = std::stoi(command_vector[i]);
        }
        catch (std::exception e) { return; }
    }

    command_json["function"] = "INSERT";
    command_json["table"] = command_vector[2];
    command_json["valid"] = true;
}

void
SQL_Command::delete_from(std::vector<std::string> command_vector) {
    
    to_uppercase(command_vector, 2);
    if (command_vector.size() != 4 || command_vector[2].compare("FROM") != 0) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    try {
        command_json["linenum"] = std::stoi(command_vector[1]);
    }
    catch (std::exception e) { return; }

    command_json["function"] = "DELETE";
    command_json["table"] = command_vector[3];
    command_json["valid"] = true;
}

Json::Value
SQL_Command::parse_conditions(std::vector<std::string> command_vector, unsigned __int64 where_index) {

    Json::Value conditions;
    conditions["valid"] = false;

    bool cond_1 = command_vector.size() == where_index + 4;
    bool cond_2 = command_vector.size() == where_index + 8;
    if (where_index and not (cond_1 || cond_2)) {
        return conditions;
    }

    if (cond_1 || cond_2) {

        if (command_vector[where_index + 2].size() != 1 || !command_vector[where_index + 2].find_first_not_of("<>=")) {
            return conditions;
        }

        conditions["condition_1"]["variable"]  = command_vector[where_index + 1];
        conditions["condition_1"]["operation"] = command_vector[where_index + 2][0];

        try {
            conditions["condition_1"]["value"] = std::stoi(command_vector[where_index + 3]);
        }
        catch (std::exception e) { return conditions; }
    }

    if (cond_2) {
        to_uppercase(command_vector, where_index + 4);
        if (command_vector[where_index + 4].compare("OR") == 0) {
            conditions["junction"] = "OR";
        }
        else if (command_vector[where_index + 4].compare("AND") == 0) {
            conditions["junction"] = "AND";
        }
        else {
            return conditions;
        }

        if (command_vector[where_index + 6].size() != 1 || !command_vector[where_index + 6].find_first_not_of("<>=")) {
            return conditions;
        }

        conditions["condition_2"]["variable"]  = command_vector[where_index + 5];
        conditions["condition_2"]["operation"] = command_vector[where_index + 6][0];

        try {
            conditions["condition_2"]["value"] = std::stoi(command_vector[where_index + 7]);
        }
        catch (std::exception e) { return conditions; }

    }

    conditions["valid"] = true;
    return conditions;
}
