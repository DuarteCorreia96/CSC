#pragma once

#include "Definitions.h"

#include <seal/seal.h>
#include <vector>
#include <string>
#include <fstream>


/// Structure used to save a encrypted int value and his encrypted binary representation.
/// Fields:
///		Value   - Integer value encryption
///		bin_vec - Vector of Binary representation encryption
typedef struct encrypted_int {

	seal::Ciphertext value;
	std::vector<seal::Ciphertext> bin_vec;

} Encrypted_int;

/// Structure used to save a int value and his binary representation.
/// Fields:
///		Value   - Integer value 
///		bin_vec - Vector of Binary representation
typedef struct Decrypted_int {

	int value;
	std::vector<int> bin_vec;

} Decrypted_int;

/// <summary>
/// Initializes SEAL context based on settings on Definitions.h and BFV Scheme
/// </summary>
/// <returns></returns>
seal::SEALContext init_SEAL_Context();

/// <summary>
/// Loads Secret SEAL key from client folder using context provided.
/// </summary>
/// <param name="context">seal::SEALContext of the key.</param>
/// <param name="client_name">client name of the key to load.</param>
/// <returns></returns>
seal::SecretKey load_SEAL_secret(seal::SEALContext context, std::string client_name = "client");

/// <summary>
/// Loads Private SEAL key from client folder using context provided.
/// </summary>
/// <param name="context">seal::SEALContext of the key.</param>
/// <param name="client_name">client name of the key to load.</param>
/// <returns></returns>
seal::PublicKey load_SEAL_public(seal::SEALContext context, std::string client_name = "client");

/// <summary>
/// Saves a Encrypted_int to a ofstream. First the value then the binary representation
/// </summary>
/// <param name="x_enc">Encrypted_int to save.</param>
/// <param name="out">ofstream on which to save.</param>
void save_encripted(Encrypted_int x_enc, std::ofstream& out);

/// <summary>
/// Loads a Encrypted_int from a flie. Can receive an opened stream (which is kept open) or a filepath which
/// is opened and closed.
/// </summary>
/// <param name="context">seal::SEALContext to be used in load,</param>
/// <param name="filepath">Filepath of the file to be loaded.</param>
/// <returns>Loaded Encrypted_int</returns>
Encrypted_int load_enc_int(seal::SEALContext context, std::string filepath);

/// <summary>
/// Loads a Encrypted_int from a flie. Can receive an opened stream (which is kept open) or a filepath which
/// is opened and closed.
/// </summary>
/// <param name="context">seal::SEALContext to be used in load,</param>
/// <param name="in">ifstream of the file to be loaded.</param>
/// <returns>Loaded Encrypted_int</returns>
Encrypted_int load_enc_int(seal::SEALContext context, std::ifstream& in);

/// <summary>
/// Loads a seal::Ciphertext from a flie. Can receive an opened stream (which is kept open) or a filepath which
/// is opened and closed.
/// </summary>
/// <param name="context">seal::SEALContext to be used in load,</param>
/// <param name="filepath">Filepath of the file to be loaded.</param>
/// <returns>Loaded Ciphertext</returns>
seal::Ciphertext load_single_enc(seal::SEALContext context, std::string filepath);

/// <summary>
/// Loads a seal::Ciphertext from a flie. Can receive an opened stream (which is kept open) or a filepath which
/// is opened and closed.
/// </summary>
/// <param name="context">seal::SEALContext to be used in load,</param>
/// <param name="in">ifstream of the file to be loaded.</param>
/// <returns>Loaded Ciphertext</returns>
seal::Ciphertext load_single_enc(seal::SEALContext context, std::ifstream& in);

/// <summary>
/// Support function of print_table used to set width of columns.
/// </summary>
/// <param name="data">value to print.</param>
/// <param name="width">desired width.</param>
/// <returns>configured column string.</returns>
std::string column_string(std::string data, int width);

/// <summary>
/// Encrypts/Decrypts a file using AES
/// </summary>
/// <param name="file_in">File to be encrypted/decrypted</param>
/// <param name="file_out">Output file</param>
/// <param name="session_key">Path to the session key file to be used in encryption/decryption</param>
/// <param name="decrypt">Decrypt if true else encrypt, default is false</param>
void AES_crypt(std::string file_in, std::string file_out, std::string session_key, bool decrypt = false);
