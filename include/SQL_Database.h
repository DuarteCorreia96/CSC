#pragma once

#include "Definitions.h"
#include "seal_utilities_extra.h"
#include "SQL_Client.h"

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
	void insert_values(std::string tablename, std::vector<std::string> columns, std::vector<Encrypted_int> values, seal::Ciphertext random);
	void create_table(std::string tablename, std::set<std::string> columns);
	void delete_line(std::string tablename, int linenum);
	
	void select(std::string tablename, Json::Value command, SQL_Client &client);

	Json::Value load_table(std::string path);

	std::vector<seal::Ciphertext>  get_compare_vec(std::vector<std::string> full_data_paths, std::string path_compare, char operation);
};