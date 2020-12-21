#pragma once

#include "Definitions.h"
#include "utilities/seal_utilities_extra.h"

#include <seal/seal.h>
#include <vector>
#include <set>
#include <string>
#include <json/json.h>
#include <filesystem>
#include <map>

/// Class of the Database which is used to do encrypted arithmetic and comparisons using SEAL.
///	Table folder organization:
///	
/// / Database\tables
/// |	
/// |	/ Table1			: Table "Table1" folder
/// |	|	
///	|	|	config.json		: Cofiguration file of the table which save a array with the columns names, number of columns, number
/// |	|					: of values and the data files ids (integers starting from 1 and incrementing from highest value)
/// |	|
///	|	|	/ col1			: Saves "col1" column where each file saves an Encrypted_int
/// |	|	|	1.data
/// |	|	|	3.data
/// |	|	|
/// |	|	
///	|	|	/ col2			: Saves "col2" column where each file saves an Encrypted_int
/// |	|	|	1.data
/// |	|	|	3.data
/// |	|	 
///	|	|	/ __RANDOM__	: Saves a random encrypted integer that is also compared in WHERE fields
/// |	|	|	1.data		: and used by the client to know which lines should it print
/// |	|	|	3.data
/// |	|
/// |
/// 
/// / Database\certs
/// |
/// |	CA-cert.crt			: CA's certificate
///	|	database-cert.crt	: Database Certificate	
/// |	private.pem			: Database's private key
/// |	public.pem			: Database's public key
/// |	
///	|	/ SEAL
/// |	|	relins			: SEAL relinearization keys
/// |
///	|	/ clients			: Folder to save public keys of clients (1 per client)
/// |	|	/ client
/// |	|	|  public.pem
/// |	|	
/// |
/// 
/// / Database\session_keys : saves session_keys, this folder is temporary and only exists during execution
/// |
/// 
class SQL_Database {

private:

	// SEAL support variables
	seal::Evaluator evaluator;
	seal::RelinKeys relin_keys;
	seal::SEALContext context;

	seal::Plaintext plain_one = seal::Plaintext("1");

	// Support variables to unpack request and form response
	Encrypted_int val_cond_1, val_cond_2;
	std::vector<Encrypted_int> values;
	seal::Ciphertext random;

	// Saves ack numbers from clients
	std::map<std::string, __int64> ack_numbers{};

	// Response of the Database to a request
	Json::Value response;
	
	/// <summary>
	/// Performs a NOT operation on a encripted bit (saved as int).
	/// </summary>
	/// <param name="encrypted">Ciphetext value to negate.</param>
	/// <param name="destination">Destination of the negated value.</param>
	void not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination);

	/// <summary>
	/// Performs a NOT operation on a encripted bool (saved as int) and saves result in the same Ciphertext.
	/// </summary>
	/// <param name="encrypted">Ciphetext Value to negate.</param>
	void not_inplace(seal::Ciphertext &encrypted);

	/// <summary>
	/// Performs a OR bit wise operation between 2 encripteds bits
	/// </summary>
	/// <param name="encrypted_1">Ciphetext 1 to OR.</param>
	/// <param name="encrypted_2">Ciphetext 2 to OR.</param>
	/// <param name="destination">Destination of the compared value.</param>
	void or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination);

	/// <summary>
	/// Performs a OR bit wise operation between 2 encripteds bits and saves on Ciphertext 1
	/// </summary>
	/// <param name="encrypted_1">Ciphetext 1 to OR. It is also the destination,</param>
	/// <param name="encrypted_2">Ciphetext 2 to OR.</param>
	void or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2);

	/// <summary>
	/// Compares two encrypted binary representations of ints and returns a encripted bool of the result of the desired operation.
	/// </summary>
	/// <param name="x">Vector of Encrypted binary representation of x</param>
	/// <param name="y">Vector of Encrypted binary representation of y</param>
	/// <param name="operation">Char of the operation desired (&lt;|&gt;|=).</param>
	/// <returns></returns>
	seal::Ciphertext compare(std::vector<seal::Ciphertext> x, std::vector<seal::Ciphertext> y, char operation);

	/// <summary>
	/// Performs comparations of a full data column (with paths specified in full_data_paths) with a Encrypted_int saved in to_compare.
	/// </summary>
	/// <param name="full_data_paths">Vector of data paths to the values of the column to compare.</param>
	/// <param name="to_compare">Encripted_int of the value to be compared.</param>
	/// <param name="operation">Char of the operation desired (&lt;|&gt;|=).</param>
	/// <returns></returns>
	std::vector<seal::Ciphertext>  get_compare_vec(std::vector<std::string> full_data_paths, Encrypted_int to_compare, char operation);

	/// <summary>
	/// Saves .json with table configuration
	/// </summary>
	/// <param name="table">Json::Value of the config of table to save.</param>
	/// <param name="path">path to the config to be saved (including filename and extension).</param>
	void save_table(Json::Value table, std::string path);

	/// <summary>
	/// Loads a table from a table configuration file (.json). Returns empty table if it doesn't exist.
	/// </summary>
	/// <param name="path">path to the config to be loaded (including filename and extension).</param>
	/// <returns>Json::Value with a table config file</returns>
	Json::Value load_table(std::string path);

	/// <summary>
	/// Creates a table if it doesn't exist and fills response.
	/// </summary>
	/// <param name="command">Json::Value of the unpacked command.</param>
	void create_table(Json::Value command);
	
	/// <summary>
	/// Inserts values into table, checking for validity of the command.
	/// </summary>
	/// <param name="command">Json::Value of the unpacked command.</param>
	void insert_values(Json::Value command);

	/// <summary>
	/// Deletes single line from the table, checking for validity of the command.
	/// </summary>
	/// <param name="command">Json::Value of the unpacked command.</param>
	void delete_line(Json::Value command);

	/// <summary>
	/// Selects single line from the table, checking for validity of the command.
	/// If response is valid it packs response with needed values and sends it to the client.
	/// </summary>
	/// <param name="command">Json::Value of the unpacked command.</param>
	void select_line(Json::Value command);

	/// <summary>
	/// Performs a SELECT command, checking for validity of the command.
	/// If response is valid it packs response with needed values and sends it to the client.
	/// </summary>
	/// <param name="command">Json::Value of the unpacked command.</param>
	void select(Json::Value command);

	/// <summary>
	/// Packs a simple (no encrypted values) encrypted response and sends it to the client.
	/// </summary>
	/// <param name="client_name">Name of the client</param>
	void pack_simple_response(std::string client_name);

public:

	/// <summary>
	/// Constructs a Dabase, initializing relinearization keys from memory and creating session keys directory.
	/// </summary>
	/// <param name="context"></param>
	SQL_Database(seal::SEALContext context) : context(context), evaluator(context) {

		std::filesystem::create_directory(DATABASE_SESSION);
		if (std::filesystem::exists(DATABASE_CERTS + "SEAL\\relins")) {

			std::ifstream in(DATABASE_CERTS + "SEAL\\relins", std::ios::binary);
			relin_keys.load(context, in);
			in.close();
			
			std::cout << "Database Correctly Initialized!" << std::endl;

		} else { 
			std::cout << "No relinearization keys on Database!" << std::endl;
		}
	};

	// Deletes session keys when ending session
	~SQL_Database() {
		std::filesystem::remove_all(DATABASE_SESSION);
	}

	/// <summary>
	/// Loads a shared encrypted session key sent by the client and saves it on session keys folder.
	/// </summary>
	/// <param name="client_name">Name of the client.</param>
	void load_session_key(std::string client_name);

	/// <summary>
	/// Unpacks a command sent by the client and sends the appropriate encrypted response.
	/// </summary>
	/// <param name="client_name">Name of the client.</param>
	void unpack_command(std::string client_name);
};
