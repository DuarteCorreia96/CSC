#include "Definitions.h"
#include "seal_utilities_extra.h"
#include "SQL_Command.h"
#include "SQL_Client.h"
#include "SQL_Database.h"

#include <seal/seal.h>
#include <iostream>
#include <string>
#include <json/json.h>


void process_command(std::string command, SQL_Database &db, SQL_Client &client) {

	SQL_Command function(command);
	Json::Value command_json = function.get_command_json();
	if (command_json["valid"].asBool()) {

		client.pack_command(command_json);
		db.unpack_command(client.get_name());
		client.unpack_response();
	}
}

int main() {

	std::string client_name = "admin";
	
	while (true) {
		std::cout << "Client name: " ;
		std::getline(std::cin, client_name);
		if (std::filesystem::exists(CLIENT_FOLDERS + client_name)) { break; }

		std::cout << "No client with name <" + client_name + "> created! Contact Admin." << std::endl;
	}

	seal::SEALContext context  = init_SEAL_Context();
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);

	SQL_Database db(context);
	SQL_Client client(client_name, context, public_key, secret_key);
	db.load_session_key(client.get_name());

	std::string command = "";
	while (true) {
		std::cout << ">> ";
		std::getline(std::cin, command);
		if (command.compare("exit") == 0)
			break;

		process_command(command, db, client);
	}
}