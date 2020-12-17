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

	seal::SEALContext context = init_SEAL_Context();
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);
	seal::RelinKeys relin_keys = load_SEAL_relins(context, client_name);

	SQL_Database db(context, relin_keys, secret_key);
	SQL_Client client(client_name, context, public_key, secret_key);

	string tablename = "Table1";
	set<string> columns_set{"col1", "col2"};
	vector<string> columns{"col2", "col1"};

	vector<Encrypted_int> values;

	values.push_back(client.encrypt_int(5));
	values.push_back(client.encrypt_int(10));

	db.create_table(tablename,  columns_set);
	db.insert_values(tablename, columns, values);

}

 