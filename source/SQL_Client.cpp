#include "SQL_Client.h"

#include <vector>
#include <bitset>
#include <string>
#include <sstream>

Encrypted_int SQL_Client::encrypt_int(__int64 x) {

	Encrypted_int x_enc{};

	std::vector<seal::Ciphertext> x_vec_enc{};

	auto x_bin = std::bitset<N_BITS>(x);
	auto x_str = x_bin.to_string();

	for (size_t i = 0; i < x_str.size(); i++) {

		auto character = std::string(1, x_str.c_str()[i]);

		seal::Ciphertext aux_encrypted;
		encryptor.encrypt(seal::Plaintext(character), aux_encrypted);
		x_vec_enc.push_back(aux_encrypted);
	}

	seal::Ciphertext x_enc_value{};
	encryptor.encrypt(seal::Plaintext(std::to_string(x)), x_enc_value);

	x_enc.bin_vec = x_vec_enc;
	x_enc.value   = x_enc_value;

	return x_enc;
}

Decrypted_int SQL_Client::decrypt_int(Encrypted_int x_enc) {

	Decrypted_int x_dec;

	std::vector<int> x_vec_enc{};

	x_dec.bin_vec.resize(N_BITS);

	seal::Plaintext aux_plain;
	for (size_t i = 0; i < N_BITS; i++) {

		decryptor.decrypt(x_enc.bin_vec[i], aux_plain);
		x_dec.bin_vec[i] = std::atoi(aux_plain.to_string().c_str());
	}

	decryptor.decrypt(x_enc.value, aux_plain);
	x_dec.value = std::atoi(aux_plain.to_string().c_str());

	return x_dec;
}
