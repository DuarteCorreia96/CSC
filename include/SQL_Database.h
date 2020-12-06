#pragma once

#include <seal/seal.h>

class SQL_Database {

private:

	seal::Evaluator evaluator;
	seal::RelinKeys relin_keys;

	seal::Plaintext plain_one;

	void not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination);
	void not_inplace(seal::Ciphertext &encrypted);

	void or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination);
	void or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2);

	seal::Ciphertext great, lesst, equal;

	void relinearize_(seal::Ciphertext& encrypted);

	// JUST FOR TESTING PURPOSES
	seal::Decryptor decryptor;

public:

	void compare(
		std::vector<seal::Ciphertext> x,
		std::vector<seal::Ciphertext> y,
		unsigned __int64 n_bits);

	SQL_Database(seal::SEALContext context, seal::RelinKeys relin, seal::SecretKey secret) : evaluator(context), decryptor(context, secret) {

		relin_keys = relin;
		plain_one  = seal::Plaintext("1");
	};

	seal::Ciphertext get_great() { return great; }
	seal::Ciphertext get_lesst() { return lesst; }
	seal::Ciphertext get_equal() { return equal; }
};