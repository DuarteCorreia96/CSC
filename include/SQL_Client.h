#pragma once

#include "Definitions.h"
#include "seal_utilities_extra.h"

#include <seal/seal.h>
#include <fstream>
#include <string>
#include <random>
#include <json/json.h>
#include <filesystem>

class SQL_Client {

private:

	std::string client_name = "client";
	std::string private_key, public_key, session_key, database_key;

	seal::Evaluator evaluator;
	seal::Encryptor encryptor;
	seal::Decryptor decryptor;

	seal::SEALContext context;

	std::random_device rd;

public:

	SQL_Client(std::string client_n, seal::SEALContext context, seal::PublicKey public_k, seal::SecretKey secret_k) : 
		evaluator(context), decryptor(context, secret_k), encryptor(context, public_k), context(context) {

		client_name = client_n;

		session_key  = CLIENT_FOLDERS + client_name + "\\session.key";
		private_key  = CLIENT_FOLDERS + client_name + "\\certs\\private.pem";
		public_key   = CLIENT_FOLDERS + client_name + "\\certs\\public.pem";
		database_key = CLIENT_FOLDERS + "database\\public.pem";

		generate_session_key();
	};

	~SQL_Client() {
		std::filesystem::remove(CLIENT_FOLDERS + client_name + "\\session.key");
	}

	std::string get_name() { return client_name; }

	seal::Ciphertext get_random_enc();

	Encrypted_int encrypt_int(__int64 x);
	Decrypted_int decrypt_int(Encrypted_int x_enc);

	void print_table(std::vector<std::vector<seal::Ciphertext>> table_enc, std::vector<std::string> columns, std::vector<seal::Ciphertext> random_enc, int linenum = 0);
	void pack_command(Json::Value command);
	void unpack_response();

	void generate_session_key();
};

