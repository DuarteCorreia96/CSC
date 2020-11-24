#include "SQL_Command.h"


SQL_Command::SQL_Command(std::string command) {

    sql_map_t funcMap = {
        { "CREATE", &SQL_Command::create_table },
        { "INSERT", &SQL_Command::insert_table },
        { "DELETE", &SQL_Command::delete_from  },
        { "SELECT", &SQL_Command::select       }
    };

    std::string token;
    std::istringstream tokenStream(command);
    std::vector<std::string> command_vector{};
    while (std::getline(tokenStream, token, ' ')) {
        command_vector.push_back(token);
    }
    
    to_uppercase(command_vector, 0);
    function = command_vector[0];    
    sql_map_t::iterator x = funcMap.find(function);

    if (x != funcMap.end()) {
        (this->*(x->second))(command_vector);
    }

    if (all_ok) {
        std::cout << "\nCommand correctly parsed!\n" << std::endl;
    } 
    else {
        std::cout << "\nCommand not valid!\n" << std::endl;
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

    to_uppercase(command_vector, 1);
    if (command_vector[1].size() > 5 && command_vector[1].back() == ')') {  

        if (command_vector[1].compare(0, 5, "MULT(") == 0) {

            columns.push_back(command_vector[1].substr(5, command_vector[1].size() - 6));
            std::cout << "Function:\t MULT" << std::endl << "Column:\t\t" << columns[0] << std::endl;
        
        }
        else if (command_vector[1].compare(0, 4, "SUM(") == 0) {
            
            columns.push_back(command_vector[1].substr(4, command_vector[1].size() - 5));
            std::cout << "Function:\t SUM" << std::endl << "Column:\t\t" << columns[0] << std::endl;
        }
    }
    else {
        std::cout << "Normal select function" << std::endl;
    }

    std::string aux_str;
    unsigned __int64 from_index = 0;
    unsigned __int64 where_index = 0;
    for (unsigned __int64 i = 0; i < command_vector.size(); i++) {

        aux_str = command_vector[i];
        if (aux_str.compare("FROM") == 0) {
            from_index = i;
        }

        if (aux_str.compare("WHERE") == 0) {
            where_index = i;
            break;
        }
    }

    if (not from_index || (where_index and not (command_vector.size() == where_index + 4 || command_vector.size() == where_index + 8))) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    to_uppercase(command_vector, where_index + 4);
    if (command_vector[where_index + 4].compare("OR") == 0) {
        operator_cond = '|';
    }
    else if (command_vector[where_index + 4].compare("AND") == 0) {
        operator_cond = '&';
    }
    else {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    table = command_vector[from_index + 1];
    condition1 = parse_condition(command_vector, where_index + 1);
    condition2 = parse_condition(command_vector, where_index + 5);
    all_ok = true;
}

void
SQL_Command::create_table(std::vector<std::string> command_vector) {

    to_uppercase(command_vector, 1);
    if (command_vector[1].compare("TABLE") != 0 || command_vector.size() < 5 || command_vector.size() % 2 != 1) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    table = command_vector[2];

    unsigned __int64 index = 3;
    while (command_vector.size() > index) {

        to_uppercase( command_vector, index + 1);
        rem_spec_char(command_vector, index + 1);
        if (command_vector[index + 1].compare("INT") != 0 && command_vector[index + 1].compare("TEXT") != 0) {
            std::cout << "Column type not valid! MUST be 'INT' or 'TEXT'!" << std::endl;
            return;
        }

        rem_spec_char(command_vector, index);

        columns.push_back(command_vector[index]);
        columns_type.push_back(command_vector[index + 1]);

        index += 2;
    }

    for (unsigned __int64 i = 0; i < columns.size(); i++) {
        std::cout << "Coluna:\t" << columns[i] << "\t Type:\t" << columns_type[i] << std::endl;
    }

    all_ok = true;
}

void
SQL_Command::insert_table(std::vector<std::string> command_vector) {

    to_uppercase(command_vector, 1);
    to_uppercase(command_vector, 2);
    if (command_vector[1].compare("INTO") != 0 || command_vector[2].compare("TABLE") != 0 
        || command_vector.size() < 7 || command_vector.size() % 2 != 1) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    table = command_vector[3];

    std::string aux_str = "";
    bool values = false;
    unsigned __int64 index = 4;
    while (command_vector.size() > index) {

        aux_str = command_vector[index];
        to_uppercase_str(aux_str);
        if (aux_str.compare("VALUES") == 0) {

            values = true;
            index++;
            continue;
        }
        
        rem_spec_char(command_vector, index);
        if (not values) {
            columns.push_back(command_vector[index]);
        } 
        else {
            columns_values.push_back(command_vector[index]);
        }

        index++;
    }

    if (not values || columns.size() != columns_values.size()) {
        std::cout << "Missing Values! Function not valid!" << std::endl;
        return;
    }

    all_ok = true;
}

void
SQL_Command::delete_from(std::vector<std::string> command_vector) {
    
    to_uppercase(command_vector, 1);
    to_uppercase(command_vector, 3);
    if ((command_vector.size() != 7 && command_vector.size() != 11) ||
        command_vector[1].compare("FROM") != 0 || command_vector[3].compare("WHERE")) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }
    
    to_uppercase(command_vector, 7);
    if (command_vector[7].compare("OR") == 0) {
        operator_cond = '|';
    }
    else if (command_vector[7].compare("AND") == 0) {
        operator_cond = '&';
    }
    else {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    table = command_vector[2];
    condition1 = parse_condition(command_vector, 4);
    condition2 = parse_condition(command_vector, 8);
    all_ok = true;
}

SQL_Condition
SQL_Command::parse_condition(std::vector<std::string> command_vector, unsigned __int64 start_index) {

    SQL_Condition new_condition;
    if (command_vector.size() < start_index + 2 || command_vector[start_index + 1].size() != 1 || 
        !command_vector[start_index + 1].find_first_not_of("<>=")) {
        std::cout << "Conditon not valid" << std::endl;
        return new_condition;
    }

    new_condition.variable  = command_vector[start_index];
    new_condition.operation = command_vector[start_index + 1][0];
    new_condition.value     = command_vector[start_index + 2];

    return new_condition;
}