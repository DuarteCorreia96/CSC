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

void process_command(std::string command, SQL_Database &db, SQL_Client &client) {

	std::cout << command << std::endl;
	SQL_Command function(command);
	client.pack_command(function.get_command_json());
	db.unpack_command();
	client.unpack_response();
}

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

	SQL_Database db(context, secret_key);
	SQL_Client client(client_name, context, public_key, secret_key);

	std::string create = "CREATE TABLE Table1 (col1, col2, col3)";
	std::string insert_1 = "INSERT INTO TABLE Table1 (col1, col2, col3) VALUES (1, 2, 3)";
	std::string insert_2 = "INSERT INTO TABLE Table1 (col1, col2, col3) VALUES (2, 3, 4)";
	std::string insert_3 = "INSERT INTO TABLE Table1 (col1, col2, col3) VALUES (2, 1, 4)";
	std::string select;


	//process_command(create, db, client);
	//process_command(insert_1, db, client);
	//process_command(insert_2, db, client);
	//process_command(insert_3, db, client);

	select = "SELECT col1, col2, col3 FROM Table1";
	process_command(select, db, client);

	select = "SELECT SUM(col1) FROM Table1 WHERE col1 < 2 OR col2 > 2";
	process_command(select, db, client);

	select = "SELECT col1, col2, col3 FROM Table1 WHERE col1 < 2 OR col2 > 2";
	process_command(select, db, client);

	select = "SELECT col1, col2, col3 FROM Table1 WHERE col1 = 2 AND col2 > 2";
	process_command(select, db, client);

	//select = "DELETE 2 FROM Table1";
	//process_command(select, db, client);

	//select = "SELECT col1, col2, col3 FROM Table1";
	//process_command(select, db, client);
}

 