#include "Definitions.h"
#include "seal_utilities_extra.h"
#include "test.h"
#include "SQL_Database.h"

#include <seal/seal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>
#include <set>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <random> 

using namespace std;
using namespace seal;

int main() {

	//int tot_clients = 0;
	//// create server and CA certificates and keys
	//admistrator(tot_clients);

	//string str;

	//// create client's keys, certificates, ...
	//tot_clients = new_client(tot_clients);
	//tot_clients = new_client(tot_clients);

	////client_signMessage("ola", 1);
	//client_encrypt("ola", 1);

	//string msg = server_decrypt();
	//cout << msg << endl;
	////server_verifySignature("ola", 1);


	////////////////////////////////

	//test_SQL_Command();

	string client_name = "admin";

	//create_keys(client_name);

	seal::SEALContext context  = init_SEAL_Context();
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);
	seal::RelinKeys relin_keys = load_SEAL_relins(context, client_name);

	SQL_Database db(context, relin_keys, secret_key);
	SQL_Client client(client_name, context, public_key, secret_key);

	string tablename = "Table1";
	set<string> columns_set{"col1", "col2", "col3"};
	vector<string> columns{"col1", "col2", "col3"};
	vector<Encrypted_int> values;

	seal::Ciphertext random = client.get_random_enc();
	values.push_back(client.encrypt_int(2));
	values.push_back(client.encrypt_int(1));
	values.push_back(client.encrypt_int(3));

	db.create_table(tablename,  columns_set);
	//db.insert_values(tablename, columns, values, random);
	//db.delete_line(tablename, 2);

	// Saving values to compare encrypted should be done on unpack command after
	{
		std::string filepath_1 = DATABASE_FOLDERS + "command\\cond_1.data";
		std::string filepath_2 = DATABASE_FOLDERS + "command\\cond_2.data";

		std::ofstream out_1(filepath_1, std::ios::binary);
		std::ofstream out_2(filepath_2, std::ios::binary);

		save_encripted(client.encrypt_int(2), out_1);
		save_encripted(client.encrypt_int(1), out_2);

		out_1.close();
		out_2.close();
	}

	// Test command
	std::string select = "SELECT col1, col2, col3 FROM Table1";
	std::cout << select << std::endl;


	SQL_Command function{select};
	db.select(tablename, function.get_command_json(), client);


	select = "SELECT col1, col2, col3 FROM Table1 WHERE col2 > 2";
	std::cout << select << std::endl;

	function.parse(select);
	db.select(tablename, function.get_command_json(), client);

	select = "SELECT col1, col2, col3 FROM Table1 WHERE col1 = 2";
	std::cout << select << std::endl;

	function.parse(select);
	db.select(tablename, function.get_command_json(), client);
}

 