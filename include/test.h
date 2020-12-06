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

void test_func() {

	int x = 4;
	int y = 4;

	const unsigned __int64 n_bits = 4;

	auto x_bin = std::bitset<n_bits>(x);
	auto y_bin = std::bitset<n_bits>(y);

	bool lesst, great, equal;

	// Version of only NOT and AND
	lesst = !x_bin[n_bits - 1] &&  y_bin[n_bits - 1];
	great =  x_bin[n_bits - 1] && !y_bin[n_bits - 1];
	for (__int64 i = n_bits - 2; i >= 0; i--) {

		lesst = !(!(!great && (!x_bin[i] &&  y_bin[i])) && !lesst);
		great = !(!(!lesst && ( x_bin[i] && !y_bin[i])) && !great);
	}
	equal = !lesst && !great;

	// Version of only NOT and OR
	lesst = !( x_bin[n_bits - 1] || !y_bin[n_bits - 1]);
	great = !(!x_bin[n_bits - 1] ||  y_bin[n_bits - 1]);
	for (__int64 i = n_bits - 2; i >= 0; i--) {

		lesst = !(great || ( x_bin[i] || !y_bin[i])) || lesst;
		great = !(lesst || (!x_bin[i] ||  y_bin[i])) || great;
	}
	equal = !(lesst || great);

	using namespace std;
	cout << x;

	if      (lesst) { cout << " < "; } 
	else if (great) { cout << " > "; } 
	else if (equal) { cout << " = "; }

	cout << y << endl;

	uint64_t plain_modulus = 16;
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

	std::vector<seal::Ciphertext> x_vec_enc{};
	std::vector<seal::Ciphertext> y_vec_enc{};

	auto  x_str = x_bin.to_string();
	for (size_t i = 0; i < x_str.size(); i++) {

		auto character = std::string(1, x_str.c_str()[i]);

		cout << character;
		seal::Ciphertext aux_encrypted;
		encryptor.encrypt(seal::Plaintext(character), aux_encrypted);
		x_vec_enc.push_back(aux_encrypted);
	}
	cout << endl;

	auto  y_str = y_bin.to_string();
	for (size_t i = 0; i < y_str.size(); i++) {

		auto character = std::string(1, y_str.c_str()[i]);

		cout << character;
		seal::Ciphertext aux_encrypted;
		encryptor.encrypt(seal::Plaintext(character), aux_encrypted);
		y_vec_enc.push_back(aux_encrypted);
	}
	cout << endl;

	SQL_Database database(context, relin_keys, secret_key);

	database.compare(x_vec_enc, y_vec_enc, n_bits);

	seal::Plaintext great_p, lesst_p, equal_p;

	decryptor.decrypt(database.get_great(), great_p);
	decryptor.decrypt(database.get_lesst(), lesst_p);
	decryptor.decrypt(database.get_equal(), equal_p);

	if (decryptor.invariant_noise_budget(database.get_great()) > 0)
		cout << "Greater   :\t" << great_p.to_string() << endl;

	if (decryptor.invariant_noise_budget(database.get_lesst()) > 0) 
		cout << "Less than :\t" << lesst_p.to_string() << endl;

	if (decryptor.invariant_noise_budget(database.get_equal()) > 0) 
		cout << "Equal     :\t" << equal_p.to_string() << endl;

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

