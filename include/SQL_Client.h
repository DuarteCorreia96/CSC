#pragma once

#include "Definitions.h"
#include "utilities/seal_utilities_extra.h"

#include <seal/seal.h>
#include <fstream>
#include <string>
#include <random>
#include <json/json.h>
#include <filesystem>

/// <summary>
/// Client which interfaces with the database by making encrypted requests and processes responses.
/// </summary>
class SQL_Client {

private:

	std::string client_name = "client";
	std::string private_key, public_key, session_key, database_key;

	seal::Evaluator evaluator;
	seal::Encryptor encryptor;
	seal::Decryptor decryptor;

	seal::SEALContext context;

	std::random_device rd;

	__int64 session_ack;

	/// Generates a random session key using Openssl rand, signs it private key, encrypts with database public key.
	/// Sends to the database the encrypted version and saves the decrypted version.
	void generate_session_key();

	/// <summary>
	/// Encrypts a random value to a Ciphertext, to be used in SELECT commands to know when line does not meet conditions in WHERE field.
	/// </summary>
	/// <returns>Ciphertext of a random value.</returns>
	seal::Ciphertext get_random_enc();

	/// <summary>
	/// Encrypts a integer to a Encrypted_int in value and binary representation
	/// </summary>
	/// <param name="x">int to be encrypted.</param>
	/// <returns>Encrypted_int</returns>
	Encrypted_int encrypt_int(__int64 x);

	/// <summary>
	/// Prints table for SELECT command
	/// </summary>
	/// <param name="table_enc">Vector of vectors (Matrix) of the table values.</param>
	/// <param name="columns">Vector of the columns names.</param>
	/// <param name="random_enc">Vector of the random values which are used to print lines (if 0 not print)</param>
	/// <param name="linenum">if SELECT LINE send id of the line to print</param>
	void print_table(std::vector<std::vector<seal::Ciphertext>> table_enc, std::vector<std::string> columns, std::vector<seal::Ciphertext> random_enc, int linenum = 0);

public:

	/// <summary>
	/// Initiliaztion of a client with SEALContext, PublicKey and SecretKey. 
	/// It initializes client SEAL evaluator, ddecryptor and encryptor, and saves a copy of context.
	/// It alsso initializes paths needed for RSA and AES. Generates a random session key and sends it to the database encrypted with RSA.
	/// </summary>
	/// <param name="client_n">Name of the client</param>
	/// <param name="context">seal::SEALContext</param>
	/// <param name="public_k">seal::PublicKey of the client</param>
	/// <param name="secret_k">seal::SecretKey of the client</param>
	SQL_Client(std::string client_n, seal::SEALContext context, seal::PublicKey public_k, seal::SecretKey secret_k) : 
		evaluator(context), decryptor(context, secret_k), encryptor(context, public_k), context(context) {

		client_name = client_n;

		session_key  = CLIENT_FOLDERS + client_name + "\\session.key";
		private_key  = CLIENT_FOLDERS + client_name + "\\certs\\private.pem";
		public_key   = CLIENT_FOLDERS + client_name + "\\certs\\public.pem";
		database_key = CLIENT_FOLDERS + "database\\public.pem";

		generate_session_key();

		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(1, 2 << 16);
		session_ack = uni(rng);
	};

	// Cleans session key
	~SQL_Client() {
		std::filesystem::remove(CLIENT_FOLDERS + client_name + "\\session.key");
	}

	/// <summary>
	/// Pack command to send to the Database. And sends the encrypted request to the database.
	/// </summary>
	/// <param name="command">Json::Value of the parsed command.</param>
	void pack_command(Json::Value command);

	/// <summary>
	/// Unpacks the encrypted response of the Database to the request sent.
	/// </summary>
	void unpack_response();

	/// <summary>
	/// Method to get client name.
	/// </summary>
	/// <returns>String of the Client name</returns>
	std::string get_name() { return client_name; }
};
