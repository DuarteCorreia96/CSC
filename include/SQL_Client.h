#pragma once

#include "Definitions.h"

#include <seal/seal.h>
#include <vector>

class SQL_Client {

private:

	std::string client_name = "client";

	seal::SecretKey secret_key;
	seal::PublicKey public_key;
	seal::RelinKeys relin_keys;

	seal::Evaluator evaluator;
	seal::Encryptor encryptor;
	seal::Decryptor decryptor;

public:

	SQL_Client(std::string client, seal::SEALContext context, seal::PublicKey public_k, seal::SecretKey secret_k) : 
		evaluator(context), decryptor(context, secret_k), encryptor(context, public_k) {
	
		public_key = public_k;
		secret_key = secret_k;
		client_name = client;
	};

	seal::Ciphertext encrypt_int(__int64 x);
	std::vector<seal::Ciphertext> encrypt_int_bin(__int64 x);
};

