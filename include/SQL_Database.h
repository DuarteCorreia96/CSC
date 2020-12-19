#pragma once

#include "Definitions.h"
#include "seal_utilities_extra.h"

#include <seal/seal.h>
#include <vector>
#include <set>
#include <string>
#include <json/json.h>
#include <filesystem>

class SQL_Database {

private:

	seal::Evaluator evaluator;
	seal::RelinKeys relin_keys;
	seal::SEALContext context;

	seal::Plaintext plain_one = seal::Plaintext("1");
	
	void not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination);
	void not_inplace(seal::Ciphertext &encrypted);

	void or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination);
	void or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2);

	Encrypted_int val_cond_1, val_cond_2;
	std::vector<Encrypted_int> values;
	seal::Ciphertext random;

	Json::Value response;

	// JUST FOR TESTING PURPOSES
	seal::Decryptor decryptor;

public:

	SQL_Database(seal::SEALContext context, seal::SecretKey secret) 
		: context(context), evaluator(context), decryptor(context, secret) {

		if (std::filesystem::exists(DATABASE_CERTS + "SEAL\\relins")) {

			std::ifstream in(DATABASE_CERTS + "SEAL\\relins", std::ios::binary);
			relin_keys.load(context, in);
			in.close();
			
			std::cout << "Database Correctly Initialized!" << std::endl;

		} else { 
			std::cout << "No relinearization keys on Database!" << std::endl;
		}
	};


	seal::Ciphertext compare(std::vector<seal::Ciphertext> x, std::vector<seal::Ciphertext> y, char operation);

	void save_table(Json::Value table, std::string path);
	void insert_values(Json::Value command);
	void create_table(Json::Value command);
	void delete_line(Json::Value command);
	
	void select(Json::Value command);

	Json::Value load_table(std::string path);

	std::vector<seal::Ciphertext>  get_compare_vec(std::vector<std::string> full_data_paths, Encrypted_int to_compare, char operation);

	void unpack_command();
	void pack_simple_response();
};