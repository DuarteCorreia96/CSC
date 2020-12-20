#include "Definitions.h"
#include "utilities/seal_utilities_extra.h"
#include "utilities/SQL_Command.h"
#include "SQL_Client.h"
#include "SQL_Database.h"

#include <seal/seal.h>
#include <iostream>
#include <string>
#include <json/json.h>

/// <summary>
/// Process a command. Mimics "Internet" interaction of client sending the request
/// and database responding.
/// </summary>
/// <param name="command">Issued command</param>
/// <param name="db">Database address</param>
/// <param name="client">Client address</param>
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

	// Initialize Client
	std::string client_name;
	while (true) {
		std::cout << "Client name: " ;
		std::getline(std::cin, client_name);
		if (std::filesystem::exists(CLIENT_FOLDERS + client_name)) { break; }

		std::cout << "No client with name <" + client_name + "> created! Contact Admin." << std::endl;
	}

	// Initialize SEAL variables (must be made outside since SEAL doesn't permit empty initializations inside classes)
	seal::SEALContext context  = init_SEAL_Context();
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);

	// Initialize Database
	SQL_Database db(context);

	// Initialize Client and share session key with database
	SQL_Client client(client_name, context, public_key, secret_key);
	db.load_session_key(client.get_name());

	// Read and process terminal commands
	std::string command = "";
	while (true) {
		std::cout << ">> ";
		std::getline(std::cin, command);
		if (command.compare("exit") == 0)
			break;

		process_command(command, db, client);
	}
}
