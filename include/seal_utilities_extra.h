#pragma once

#include "Definitions.h"

#include <seal/seal.h>
#include <iostream>
#include <fstream>

seal::SEALContext init_SEAL_Context(std::string client_name) {

	seal::EncryptionParameters parms(seal::scheme_type::bfv);

	parms.set_poly_modulus_degree(POLY_MOD_DEG);
	parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(POLY_MOD_DEG));
	parms.set_plain_modulus(PLAIN_MOD);

	seal::SEALContext context(parms);

	return context;
}

seal::SecretKey load_SEAL_secret(seal::SEALContext context, std::string client_name = "client") {

	std::ifstream in_secret(CLIENT_FOLDERS + client_name + "\\secret", std::ios::binary);

	seal::SecretKey secret;
	secret.load(context, in_secret);

	return secret;
}

seal::PublicKey load_SEAL_public(seal::SEALContext context, std::string client_name = "client") {

	std::ifstream in_public(CLIENT_FOLDERS + client_name + "\\public", std::ios::binary);

	seal::PublicKey public_key;
	public_key.load(context, in_public);

	return public_key;
}

seal::RelinKeys load_SEAL_relin(seal::SEALContext context, std::string client_name = "client") {

	std::ifstream in_public(CLIENT_FOLDERS + client_name + "\\relins", std::ios::binary);

	seal::RelinKeys relin_keys;
	relin_keys.load(context, in_public);

	return relin_keys;
}