#pragma once


#include "Definitions.h"

#include "SQL_Command.h"
#include "SQL_Database.h"
#include "SQL_Client.h"

#include <iostream>
#include <string>
#include <seal/seal.h>

#include <json/json.h>


void test_seal() {

	using namespace std;
	using namespace seal;

	std::string client_name = "admin";

	// If you need to generate new keys
	// create_keys(client_name);

	seal::SEALContext context = init_SEAL_Context();
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);

	SQL_Client client(client_name, context, public_key, secret_key);

	ofstream out("data\\enc.txt", ios::binary);

	Json::Value root, function;
	Json::StreamWriterBuilder builder;
	root["function"]["pri"] = "select";
	root["function"]["sub"] = "sum";

	root["columns"][0] = "col1";
	root["columns"][1] = "col2";
	root["columns"][2] = "col3";

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(root, &out);
	out << endl;

	out << "Values below:" << endl;

	Encrypted_int x_enc = client.encrypt_int(5);
	save_encripted(x_enc, out);

	out.close();

	cout << "saved" << endl;

	std::string test;

	ifstream in("data\\enc.txt", ios::binary);

	std::string json_string;
	while (test.compare("Values below:") != 0) {

		std::getline(in, test);
		json_string.append(test);
		json_string.append("\n");
	}

	JSONCPP_STRING err;
	Json::Value data;
	Json::CharReaderBuilder rbuilder;
	std::stringstream json_parse(json_string);
	Json::parseFromStream(rbuilder, json_parse, &data, &err);

	cout << data["function"]["pri"].asString() << endl;
	cout << data["function"]["sub"].asString() << endl;
	for (auto& col : data["columns"]) {
		cout << col.asString() << endl;
	}

	Encrypted_int y_enc = load_enc_int(context, in);
	Decrypted_int y = client.decrypt_int(y_enc);

	cout << y.value << endl;
	for (int bit : y.bin_vec) {
		cout << bit;
	} cout << endl;
}

void test_func() {

	int x = 30;
	int y = 46;
	char operation = '>';

	const unsigned __int64 n_bits = N_BITS;

	using namespace std;
	cout << "start" << endl;
	

	uint64_t plain_modulus = PLAIN_MOD;
	seal::EncryptionParameters parms(seal::scheme_type::bfv);

	size_t poly_modulus_degree = POLY_MOD_DEG;
	parms.set_poly_modulus_degree(poly_modulus_degree);
	parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
	parms.set_plain_modulus(plain_modulus);

	std::string client_name = "admin";

	seal::SEALContext context(parms);
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);

	seal::Encryptor encryptor(context, public_key);
	seal::Evaluator evaluator(context);
	seal::Decryptor decryptor(context, secret_key);

	SQL_Client client("client", context, public_key, secret_key);

	std::vector<seal::Ciphertext> x_vec_enc = client.encrypt_int(x).bin_vec;
	std::vector<seal::Ciphertext> y_vec_enc = client.encrypt_int(y).bin_vec;

	SQL_Database database(context);

	seal::Ciphertext op_encry;
	seal::Plaintext  op_plain;

	auto time_start = chrono::high_resolution_clock::now();
	op_encry = database.compare(x_vec_enc, y_vec_enc, operation);

	auto time_end = chrono::high_resolution_clock::now();
	auto time_diff = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);
	cout << "Done [" << time_diff.count() << " miliseconds]" << endl;

	decryptor.decrypt(op_encry, op_plain);
	cout << x << operation << y << " is: ";
	if      (op_plain.to_string().compare("1") == 0) { cout << "TRUE";  }
	else if (op_plain.to_string().compare("0") == 0) { cout << "FALSE"; }
	else { cout << "Something went terribly wrong"; }
	cout << endl;
}

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

    std::string command  = "SELECT sum(coluna123) FROM table1 WHERE col1 > 10";
	std::string select_c = "SELECT col1, col2, col3 FROM tablename WHERE col1 > 2 AND col3 < 1";
	std::string select_l = "SELECT LINE 3 FROM tablename";
	std::string command2 = "CREATE TABLE tablename (col1name TEXT, col2name INT, col3name TEXT)";
	std::string delete_c = "DELETE 35 FROM tablename";
	std::string insert_c = "INSERT INTO TABLE tablename (col1, col2) VALUES (1, 2)";
	std::string create_c = "CREATE TABLE tablename (col1, col2, col3)";

	std::string command_used = select_c;
	SQL_Command function{ command_used };

	Json::Value command_json = function.get_command_json();
	Json::Value valid;

	command_json.removeMember("valid", &valid);
	if (not valid.asBool()) {
		return -1;
	}

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::ofstream out("data\\test_j1.json");

	writer->write(command_json, &out);
	out << std::endl;
	out.close();

	std::cout << std::endl <<command_used << std::endl;

	return 0;
}

void test_relin() {

	using namespace std;
	using namespace seal;
	uint64_t plain_modulus = 1024;
	seal::EncryptionParameters parms(seal::scheme_type::bfv);

	size_t poly_modulus_degree = 8192;
	parms.set_poly_modulus_degree(poly_modulus_degree);
	parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
	parms.set_plain_modulus(plain_modulus);

	seal::SEALContext context(parms);
	seal::KeyGenerator keygen(context);
	seal::PublicKey public_key;
	seal::RelinKeys relin_keys;

	keygen.create_public_key(public_key);
	seal::SecretKey secret_key = keygen.secret_key();
	keygen.create_relin_keys(relin_keys);

	seal::Encryptor encryptor(context, public_key);
	seal::Evaluator evaluator(context);
	seal::Decryptor decryptor(context, secret_key);

	seal::Ciphertext x_enc, y_enc;
	seal::Plaintext  x("3"), y("5");

	encryptor.encrypt(x, x_enc);
	cout << "Com relinearizacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.add_inplace(x_enc, x_enc);
	cout << "  + Soma:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.relinearize_inplace(x_enc, relin_keys);
	cout << "  + Re-linearizacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.relinearize_inplace(x_enc, relin_keys);
	cout << "  + Re-linearizacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.relinearize_inplace(x_enc, relin_keys);
	cout << "  + Re-linearizacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	encryptor.encrypt(x, x_enc);
	cout << "\nSem relinearizacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;

	evaluator.multiply_inplace(x_enc, x_enc);
	cout << "  + Multiplicacao:" << endl;
	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(x_enc) << " bits " << endl;
	cout << "    + size of encrypted_result: " << x_enc.size() << endl;
}

