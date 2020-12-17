#pragma once

#include "Definitions.h"

#include <seal/seal.h>
#include <vector>
#include <string>
#include <fstream>

typedef struct encrypted_int {

	seal::Ciphertext value;
	std::vector<seal::Ciphertext> bin_vec;

} Encrypted_int;

typedef struct Decrypted_int {

	int value;
	std::vector<int> bin_vec;

} Decrypted_int;

seal::SEALContext init_SEAL_Context();

void create_keys(std::string client_name);

seal::SecretKey load_SEAL_secret(seal::SEALContext context, std::string client_name = "client");
seal::PublicKey load_SEAL_public(seal::SEALContext context, std::string client_name = "client");
seal::RelinKeys load_SEAL_relins(seal::SEALContext context, std::string client_name = "client");


void save_encripted(Encrypted_int x_enc, std::ofstream& out);
Encrypted_int load_enc_int(seal::SEALContext context, std::ifstream& in);