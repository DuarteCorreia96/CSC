#include "seal_utilities_extra.h"

#include <iostream>
#include <fstream>
#include <filesystem>

seal::SEALContext init_SEAL_Context() {

	seal::EncryptionParameters parms(seal::scheme_type::bfv);

	parms.set_poly_modulus_degree(POLY_MOD_DEG);
	parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(POLY_MOD_DEG));
	parms.set_plain_modulus(PLAIN_MOD);

	seal::SEALContext context(parms);

	return context;
}

void create_keys(std::string client_name) {

	if (std::filesystem::exists(CLIENT_FOLDERS + client_name + "\\secret"))
		std::cout << "Overwriting secret key for client: " << client_name << std::endl;

	if (std::filesystem::exists(CLIENT_FOLDERS + client_name + "\\public"))
		std::cout << "Overwriting public key for client: " << client_name << std::endl;

	if (std::filesystem::exists(CLIENT_FOLDERS + client_name + "\\relins"))
		std::cout << "Overwriting relin keys for client: " << client_name << std::endl;

	seal::SEALContext context = init_SEAL_Context();
	seal::KeyGenerator keygen(context);

	seal::SecretKey secret_key = keygen.secret_key();
	seal::PublicKey public_key;
	seal::RelinKeys relin_keys;

	keygen.create_public_key(public_key);
	keygen.create_relin_keys(relin_keys);

	std::ofstream secret_out(CLIENT_FOLDERS + client_name + "\\secret", std::ios::binary);
	std::ofstream public_out(CLIENT_FOLDERS + client_name + "\\public", std::ios::binary);
	std::ofstream relins_out(CLIENT_FOLDERS + client_name + "\\relins", std::ios::binary);

	secret_key.save(secret_out);
	public_key.save(public_out);
	relin_keys.save(relins_out);
}

seal::SecretKey load_SEAL_secret(seal::SEALContext context, std::string client_name) {

	std::ifstream in_secret(CLIENT_FOLDERS + client_name + "\\secret", std::ios::binary);

	seal::SecretKey secret;
	secret.load(context, in_secret);

	return secret;
}

seal::PublicKey load_SEAL_public(seal::SEALContext context, std::string client_name) {

	std::ifstream in_public(CLIENT_FOLDERS + client_name + "\\public", std::ios::binary);

	seal::PublicKey public_key;
	public_key.load(context, in_public);

	return public_key;
}

seal::RelinKeys load_SEAL_relins(seal::SEALContext context, std::string client_name) {

	std::ifstream in_public(CLIENT_FOLDERS + client_name + "\\relins", std::ios::binary);

	seal::RelinKeys relin_keys;
	relin_keys.load(context, in_public);

	return relin_keys;
}

void save_encripted(Encrypted_int x_enc, std::ofstream& out) {

	x_enc.value.save(out);
	for (seal::Ciphertext bit_enc : x_enc.bin_vec) {

		bit_enc.save(out);
	}
}

seal::Ciphertext load_single_enc(seal::SEALContext context, std::string filepath) {

	seal::Ciphertext x_enc{};

	std::ifstream in(filepath, std::ios::binary);
	x_enc.load(context, in);

	in.close();
	return x_enc;
}

seal::Ciphertext load_single_enc(seal::SEALContext context, std::ifstream& in) {

	seal::Ciphertext x_enc{};
	x_enc.load(context, in);

	return x_enc;
}


Encrypted_int load_enc_int(seal::SEALContext context, std::string filepath) {

	Encrypted_int x_enc{};

	std::ifstream in(filepath, std::ios::binary);
	x_enc.value.load(context, in);

	x_enc.bin_vec.resize(N_BITS);
	for (int i = 0; i < N_BITS; i++) {
		x_enc.bin_vec[i].load(context, in);
	}

	in.close();
	return x_enc;
}

Encrypted_int load_enc_int(seal::SEALContext context, std::ifstream& in) {

	Encrypted_int x_enc{};

	x_enc.value.load(context, in);

	x_enc.bin_vec.resize(N_BITS);
	for (int i = 0; i < N_BITS; i++) {
		x_enc.bin_vec[i].load(context, in);
	}

	return x_enc;
}

std::string column_string(std::string data, int width) {

	int in_len = data.size();
	int pad_left  = (width - in_len) / 2;
	int pad_right = width  - pad_left - in_len;

	return std::string(" ", pad_left) + data + std::string(" ", pad_right);
}
