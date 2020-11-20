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
}

void
SQL_Command::to_uppercase(std::vector<std::string>& command_vector, unsigned __int64 index) {
    std::transform(command_vector[index].begin(), command_vector[index].end(), command_vector[index].begin(), ::toupper);
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
        return;
    }
}

void
SQL_Command::create_table(std::vector<std::string> command_vector) {

    to_uppercase(command_vector, 1);
    if (command_vector[1].compare("TABLE") != 0 || command_vector.size() < 5 || command_vector.size() % 2 != 1) {
        std::cout << "Function not valid!" << std::endl;
        return;
    }

    table = command_vector[3];

    unsigned __int64 index = 3;
    while (command_vector.size() > index) {

        to_uppercase( command_vector, index + 1);
        rem_spec_char(command_vector, index + 1);
        if (command_vector[index + 1].compare("INT") != 0 && command_vector[index + 1].compare("TEXT") != 0) {
            std::cout << "Column type not valid!" << std::endl;
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
}

void
SQL_Command::insert_table(std::vector<std::string> command_vector) {
    std::cout << "Hello from insert" << std::endl;
}

void
SQL_Command::delete_from(std::vector<std::string> command_vector) {
    std::cout << "Hello from delete" << std::endl;
}