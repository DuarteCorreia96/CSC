#pragma once

#include "Definitions.h"

#include <seal/seal.h>
#include <vector>

class SQL_Database {

private:

	seal::Evaluator evaluator;
	seal::RelinKeys relin_keys;

	seal::Plaintext plain_one;

	void not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination);
	void not_inplace(seal::Ciphertext &encrypted);

	void or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination);
	void or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2);

	// JUST FOR TESTING PURPOSES
	seal::Decryptor decryptor;

public:

	seal::Ciphertext compare(std::vector<seal::Ciphertext> x, std::vector<seal::Ciphertext> y, char operation);

	SQL_Database(seal::SEALContext context, seal::RelinKeys relin, seal::SecretKey secret) 
		: evaluator(context), decryptor(context, secret) {

		relin_keys = relin;
		plain_one  = seal::Plaintext("1");
	};
};