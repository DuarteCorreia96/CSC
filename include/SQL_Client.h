#pragma once

#include "Definitions.h"
#include "seal_utilities_extra.h"

#include <seal/seal.h>
#include <fstream>
#include <string>
#include <random>

class SQL_Client {

private:

	std::string client_name = "client";

	seal::Evaluator evaluator;
	seal::Encryptor encryptor;
	seal::Decryptor decryptor;

	seal::SEALContext context;

	std::random_device rd;

public:

	SQL_Client(std::string client, seal::SEALContext context, seal::PublicKey public_k, seal::SecretKey secret_k) : 
		evaluator(context), decryptor(context, secret_k), encryptor(context, public_k), context(context) {
	};

	seal::Ciphertext get_random_enc();

	Encrypted_int encrypt_int(__int64 x);
	Decrypted_int decrypt_int(Encrypted_int x_enc);

	void print_table(std::vector<std::vector<Encrypted_int>> table_enc, std::vector<std::string> columns, std::vector<seal::Ciphertext> random_enc);
};

