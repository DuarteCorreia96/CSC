#include "SQL_Command.h"

SQL_Command::SQL_Command(std::string command) {

    sql_map_t funcMap = {
        { "create", &SQL_Command::create_table },
        { "insert", &SQL_Command::insert_table },
        { "delete", &SQL_Command::delete_from  },
        { "select", &SQL_Command::select       }
    };

    std::string token;
    std::istringstream tokenStream(command);
    std::vector<std::string> command_vector{};
    while (std::getline(tokenStream, token, ' ')) {
        command_vector.push_back(token);
    }
    
    function = command_vector[0];
    std::transform(function.begin(), function.end(), function.begin(), ::tolower);
    sql_map_t::iterator x = funcMap.find(function);

    if (x != funcMap.end()) {
        (this->*(x->second))(command_vector);
    }
}

void 
SQL_Command::select(std::vector<std::string> command_vector) {

    std::transform(command_vector[1].begin(), command_vector[1].end(), command_vector[1].begin(), ::tolower);
    if (command_vector[1].size() > 5 && command_vector[1].back() == ')') {  

        if (command_vector[1].compare(0, 5, "mult(") == 0) {

            columns.push_back(command_vector[1].substr(5, command_vector[1].size() - 6));
            std::cout << "Function:\tmult" << std::endl << "Column:\t\t" << columns[0] << std::endl;
        
        }
        else if (command_vector[1].compare(0, 4, "sum(") == 0) {
            
            columns.push_back(command_vector[1].substr(4, command_vector[1].size() - 5));
            std::cout << "Function:\tsum" << std::endl << "Column:\t\t" << columns[0] << std::endl;
        }
    }
    else {
        return;
    }
}

void
SQL_Command::create_table(std::vector<std::string> command_vector) {
    std::cout << "Hello from create" << std::endl;
}

void
SQL_Command::insert_table(std::vector<std::string> command_vector) {
    std::cout << "Hello from insert" << std::endl;
}

void
SQL_Command::delete_from(std::vector<std::string> command_vector) {
    std::cout << "Hello from delete" << std::endl;
}

void
SQL_Command::parse_columns(std::vector<std::string> command_vector) {

    std::string column;
    std::vector<char> spec_chars{ ',','(',')' };

    //int i = 1;
    //while (command_vector[i].compare("FROM")) {
    //
    //}
    //    for (auto& spec_char : spec_chars) {
    //        column.erase(std::remove(column.begin(), column.end(), spec_char), column.end());
    //    }

    //    columns.push_back(column);
    //    iter >> column;
    //} while (column.compare("FROM") || !iter);
}