// Admin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <seal/seal.h>
#include "seal_utilities_extra.h"

void generate_SEAL();
void generate_OpenSSL();
void generate_Clients(std::string certs_folder, std::string cert_name, std::string client_name = "client", bool database = false);
int verify_cert(std::string certs_folder, std::string cert_name);

int main(){

	generate_SEAL();
	generate_OpenSSL();
}



void generate_OpenSSL() {

	// Generate CA Private key
	std::string command{ "openssl genrsa -out " + ADMIN_CA + "private.pem " + std::to_string(RSA_SIZE) };
	system(command.c_str());

	// Generate CA public key
	command  = "openssl req -new -key " + ADMIN_CA + "private.pem -x509 -days 1000 -out " + ADMIN_CA + CA_CERTIFICATE;
	command += " -subj \"/C=PT/ST=Lisbon/L=Lisbon/O=Global Security/OU=IT/CN=ist.pt\" ";
	system(command.c_str());

	generate_Clients(DATABASE_CERTS, "database-cert.crt", "Database", true);

	std::string client_name = "client";
	generate_Clients(CLIENT_FOLDERS + client_name + "\\certs\\", "client-cert.crt", client_name);

	client_name = "duarte";
	generate_Clients(CLIENT_FOLDERS + client_name + "\\certs\\", "client-cert.crt", client_name);
}

int verify_cert(std::string certs_folder, std::string cert_name) {

	std::string verify_file = "data\\tmp\\verify.txt";
	std::string command = "openssl verify -CAfile " + certs_folder + CA_CERTIFICATE + " " + certs_folder + cert_name + " > " + verify_file;
	system(command.c_str());

	std::string test;
	std::ifstream in(verify_file);
	std::getline(in, test);
	in.close();
	std::filesystem::remove(verify_file);
	
	if (test.compare(certs_folder + cert_name + ": OK") == 0) {
		return -1;
	}

	return 0;
}

void generate_Clients(std::string certs_folder, std::string cert_name, std::string client_name, bool database) {

	if (not std::filesystem::exists(certs_folder)) {
		std::filesystem::create_directories(certs_folder);
	}

	// Copy CA Certificate to Database/Client
	std::filesystem::copy_file(ADMIN_CA + CA_CERTIFICATE, certs_folder + CA_CERTIFICATE, std::filesystem::copy_options::overwrite_existing);

	// Generate database/clients private key
	std::string command = "openssl genrsa -out " + certs_folder + "private.pem " + std::to_string(RSA_SIZE);
	system(command.c_str());

	// Create certificate to be signed and send it to CA
	command = "openssl req -new -key " + certs_folder + "private.pem" + " -out " + ADMIN_CA + "signingReq.csr";
	command += " -subj \"/C=PT/ST=Lisbon/L=Lisbon/O=Global Security/OU=IT/CN=ist.pt\" ";
	system(command.c_str());

	// Sign certificate and send it to database/client
	command  = "openssl x509 -req -days 365 -in " + ADMIN_CA + "signingReq.csr -CA " + ADMIN_CA + CA_CERTIFICATE + " -CAkey " + ADMIN_CA  + "private.pem -CAcreateserial -out " + certs_folder + cert_name;
	system(command.c_str());

	// Verify certificate
	if (verify_cert(certs_folder, cert_name) != 0) {
		std::cout << "Certificate not correct!" << std::endl;
		exit(-1);
	}

	// Generate Client public key
	command = "openssl x509 -pubkey -in " + certs_folder + cert_name + " -out " + certs_folder + "public.pem";
	system(command.c_str());

	if (not database) {
		std::filesystem::create_directory(DATABASE_CERTS + "Clients\\" + client_name);
		std::filesystem::copy(certs_folder + "\\public.pem", DATABASE_CERTS + "Clients\\" + client_name + "\\public.pem" , std::filesystem::copy_options::overwrite_existing);
	}
}


void generate_SEAL() {
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
	std::filesystem::copy(ADMIN_SEAL + "relins", DATABASE_CERTS + "\\SEAL\\relins", std::filesystem::copy_options::overwrite_existing);

	// Just for now 
	std::filesystem::copy(ADMIN_SEAL + "public", CLIENT_FOLDERS + "admin\\SEAL\\public", std::filesystem::copy_options::overwrite_existing);
	std::filesystem::copy(ADMIN_SEAL + "secret", CLIENT_FOLDERS + "admin\\SEAL\\secret", std::filesystem::copy_options::overwrite_existing);
}