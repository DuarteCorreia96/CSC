#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <seal/seal.h>
#include <iomanip>

#include "SQL_Command.h"
#include <seal/seal.h>

#include <fstream>
#include <json/json.h>
#include <bitset>

#include "SQL_Database.h"
#include "SQL_Client.h"

void test_func() {

	int x = 30;
	int y = 46;
	char operation = '>';

	const unsigned __int64 n_bits = 12;

	uint64_t plain_modulus = 4096;
	seal::EncryptionParameters parms(seal::scheme_type::bfv);

	size_t poly_modulus_degree = 32768;
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


	SQL_Client client(context, public_key, secret_key);

	std::vector<seal::Ciphertext> x_vec_enc = client.encrypt_int_bin(x);
	std::vector<seal::Ciphertext> y_vec_enc = client.encrypt_int_bin(y);

	SQL_Database database(context, relin_keys, secret_key);

	seal::Ciphertext op_encry;
	seal::Plaintext  op_plain;
	op_encry = database.compare(x_vec_enc, y_vec_enc, operation);

	using namespace std;

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

    std::string command  = "SELECT mult(coluna123) FROM table1 WHERE col1 > 10;";
	std::string select_c = "SELECT col1, col2, col3 FROM tablename WHERE col1 > 2 AND col3 < 1";
	std::string command2 = "CREATE TABLE tablename (col1name TEXT, col2name INT, col3name TEXT)";
	std::string delete_c = "DELETE FROM tablename WHERE col1name = value1 OR col2name > value2";
	std::string insert_c = "INSERT INTO TABLE tablename (col1, col2) VALUES (1, 2)";

	std::string command_used = insert_c;
	SQL_Command function{ command_used };

	if (not function.check_command()) {
		return -1;
	}

	std::cout << "Function:\t" << function.get_function() << std::endl;
	std::cout << "Table:   \t" << function.get_table()    << std::endl;
	std::cout << "Operator:\t" << function.get_operator() << std::endl;

	function.get_condition1().print();
	function.get_condition2().print();

	std::vector<std::string> columns = function.get_columns();
	std::vector<std::string> columns_v = function.get_columns_values();

	std::cout << "Colunas:" << std::endl;

	if (columns.size() == columns_v.size()) {
		for (unsigned __int64 i = 0; i < columns.size(); i++) {
			std::cout << "\t\t" << columns[i] << "\t <- \t" << columns_v[i] << std::endl;
		}
	} else {
		for (unsigned __int64 i = 0; i < columns.size(); i++) {
			std::cout << "\t\t" << columns[i] << std::endl;
		}
	}

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

