#pragma once

#include <seal/seal.h>
#include <vector>
#include <bitset>
#include "Definitions.h"

class SQL_Client
{

private:

	seal::Evaluator evaluator;
	seal::Encryptor encryptor;
	seal::Decryptor decryptor;

	seal::SecretKey secret_key;
	seal::PublicKey public_key;

public:

	SQL_Client(seal::SEALContext context, seal::PublicKey public_k, seal::SecretKey secret_k) : 
		evaluator(context), decryptor(context, secret_k), encryptor(context, public_k) {
	
		public_key = public_k;
		secret_key = secret_k;
	
	};


	seal::Ciphertext encrypt_int(__int64 x);
	std::vector<seal::Ciphertext> encrypt_int_bin(__int64 x);
};

