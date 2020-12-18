// Admin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <seal/seal.h>
#include "seal_utilities_extra.h"

int main(){
     
	std::cout << std::endl;
	if (std::filesystem::exists(ADMIN_SEAL + "secret"))
		std::cout << "Overwriting secret key for ADMIN! " << std::endl;

	if (std::filesystem::exists(ADMIN_SEAL + "public"))
		std::cout << "Overwriting public key for ADMIN! " << std::endl;

	if (std::filesystem::exists(ADMIN_SEAL + "relins"))
		std::cout << "Overwriting relin keys for ADMIN! " << std::endl;

	seal::SEALContext context = init_SEAL_Context();
	seal::KeyGenerator keygen(context);

	std::cout << std::endl << "Generating new keys ....";
	seal::SecretKey secret_key = keygen.secret_key();
	seal::PublicKey public_key;
	seal::RelinKeys relin_keys;

	std::cout << "....";
	keygen.create_public_key(public_key);
	std::cout << "....";
	keygen.create_relin_keys(relin_keys);
	std::cout << " Done" << std::endl;

	std::cout << "Saving new keys ....";
	std::ofstream secret_out(ADMIN_SEAL + "secret", std::ios::binary);
	std::ofstream public_out(ADMIN_SEAL + "public", std::ios::binary);
	std::ofstream relins_out(ADMIN_SEAL + "relins", std::ios::binary);

	secret_key.save(secret_out);
	std::cout << "....";
	public_key.save(public_out);
	std::cout << "....";
	relin_keys.save(relins_out);
	std::cout << " Done" << std::endl << std::endl;

	secret_out.close();
	public_out.close();
	relins_out.close();

	std::cout << "Coyping relineariztion keys to Database" << std::endl << std::endl;
	std::filesystem::copy(ADMIN_SEAL + "relins", DATABASE_CERTS + "\\SEAL\\relins" , std::filesystem::copy_options::overwrite_existing);

	// Just for now 
	std::filesystem::copy(ADMIN_SEAL + "public", CLIENT_FOLDERS + "admin\\SEAL\\public", std::filesystem::copy_options::overwrite_existing);
	std::filesystem::copy(ADMIN_SEAL + "secret", CLIENT_FOLDERS + "admin\\SEAL\\secret", std::filesystem::copy_options::overwrite_existing);
}