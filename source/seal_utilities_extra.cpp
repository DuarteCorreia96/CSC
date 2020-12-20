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

seal::SecretKey load_SEAL_secret(seal::SEALContext context, std::string client_name) {

	std::ifstream in_secret(CLIENT_FOLDERS + client_name + "\\SEAL\\secret", std::ios::binary);

	seal::SecretKey secret;
	secret.load(context, in_secret);

	return secret;
}

seal::PublicKey load_SEAL_public(seal::SEALContext context, std::string client_name) {

	std::ifstream in_public(CLIENT_FOLDERS + client_name + "\\SEAL\\public", std::ios::binary);

	seal::PublicKey public_key;
	public_key.load(context, in_public);

	return public_key;
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

void AES_crypt(std::string file_in, std::string file_out, std::string session_key, bool decrypt) {

	std::string command = "openssl enc -aes-256-cbc -md sha512 -pbkdf2 -nosalt -in " + file_in + " -out " + file_out + " -pass file:" + session_key;
	if (decrypt) {
		command += " -d";
	}
	system(command.c_str());
}