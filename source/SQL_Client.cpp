#include "SQL_Client.h"

#include <vector>
#include <bitset>
#include <string>
#include <sstream>

seal::Ciphertext SQL_Client::get_random_enc() {

	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(1, PLAIN_MOD);

	int x = uni(rng);
	seal::Ciphertext x_enc{};
	encryptor.encrypt(seal::Plaintext(std::to_string(x)), x_enc);

	return x_enc;
}


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

void SQL_Client::print_table(std::vector<std::vector<Encrypted_int>> table_enc, 
	std::vector<std::string> columns, std::vector<seal::Ciphertext> random_enc) {

	int id_width = 6;
	int column_width = 15;

	std::cout << std::endl;
	std::cout << column_string("ID", id_width) << "|";
	for (int col = 0; col < table_enc[0].size(); col++) {
		std::cout << column_string(columns[col], column_width) << "|";
	} std::cout << std::endl;

	for (int id = 0; id < table_enc.size(); id++) {

		seal::Plaintext plain;
		decryptor.decrypt(random_enc[id], plain);
		if (plain.to_string().compare("0") == 0) {
			continue;
		}

		std::cout << column_string(std::to_string(id + 1), id_width) << "|";
		for (int col = 0; col < table_enc[0].size(); col++) {

			Decrypted_int aux = decrypt_int(table_enc[id][col]);
			std::cout << column_string(std::to_string(aux.value), column_width) << "|";

		} std::cout << std::endl;
	}
	std::cout << std::endl;
}