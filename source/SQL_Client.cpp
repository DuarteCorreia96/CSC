#include "SQL_Client.h"

#include <vector>
#include <seal/seal.h>
#include <bitset>
#include <string>

std::vector<seal::Ciphertext> SQL_Client::encrypt_int_bin(__int64 x) {

	std::vector<seal::Ciphertext> x_vec_enc{};

	auto x_bin = std::bitset<N_BITS>(x);
	auto x_str = x_bin.to_string();

	for (size_t i = 0; i < x_str.size(); i++) {

		auto character = std::string(1, x_str.c_str()[i]);

		seal::Ciphertext aux_encrypted;
		encryptor.encrypt(seal::Plaintext(character), aux_encrypted);
		x_vec_enc.push_back(aux_encrypted);
	}

	return x_vec_enc;
}

seal::Ciphertext SQL_Client::encrypt_int(__int64 x) {

	seal::Ciphertext x_enc{};
	encryptor.encrypt(seal::Plaintext(std::to_string(x)), x_enc);

	return x_enc;
}

