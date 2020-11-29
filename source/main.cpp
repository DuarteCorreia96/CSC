#include <iostream>
#include <string>
#include <vector>

#include "SQL_Command.h"
#include "test.h"
#include <seal/seal.h>

#include <fstream>
#include <json/json.h>

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>

#include <openssl/bio.h>

#include "examples.h"

#include <bitset>

int main() {

    seal::EncryptionParameters parms(seal::scheme_type::bfv);

    size_t poly_modulus_degree = 4096;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(1024);

    seal::SEALContext context(parms);
    seal::KeyGenerator keygen(context);
    seal::PublicKey public_key;

    keygen.create_public_key(public_key);
    auto secret_key = keygen.secret_key();
    auto relin_keys = keygen.create_relin_keys();

    seal::Encryptor encryptor(context, public_key);
    seal::Evaluator evaluator(context);
    seal::Decryptor decryptor(context, secret_key);


    seal::Plaintext plain, out_plain;
    seal::Ciphertext encrypted;

    std::string encoded = "testes";
    
    std::vector<int> test_vector;
    std::vector<seal::Ciphertext> test_cipher_vec;

    for (auto character : encoded) {

        seal::Ciphertext aux_encrypted;
        encryptor.encrypt(seal::Plaintext(std::to_string(character)), aux_encrypted);
        test_cipher_vec.push_back(aux_encrypted);
    }

    std::string decoded = "";
    decoded.resize(test_cipher_vec.size());
    for (unsigned __int64 i = 0; i < test_cipher_vec.size(); i++) {

        seal::Plaintext aux_plain;
        decryptor.decrypt(test_cipher_vec[i], aux_plain);
        decoded[i] = atoi(aux_plain.to_string().c_str());
    }
    
    std::cout << "Encoded:\t" << encoded << std::endl;
    std::cout << "Decoded:\t" << decoded << std::endl;

    unsigned __int64 decimal = 1506;
    const std::size_t bitset_size = 32;
    std::string binary = std::bitset<bitset_size>(decimal).to_string(); //to binary
    std::cout << binary << "\n";

    decimal = std::bitset<bitset_size>(binary).to_ulong();
    std::cout << decimal << "\n";



    int x = 10001635443;
    seal::Plaintext x_plain(std::to_string(x));
    std::cout << "Express x = " + std::to_string(x) + " as a plaintext polynomial 0x" + x_plain.to_string() + "." << std::endl;

    seal::Ciphertext x_encrypted_1;
    encryptor.encrypt(x_plain, x_encrypted_1);

    seal::Ciphertext x_encrypted_2;
    encryptor.encrypt(x_plain, x_encrypted_2);

    std::ofstream data_file1("data/test_1.txt", std::ios::binary);
    std::ofstream data_file2("data/test_2.txt", std::ios::binary);

    x_encrypted_1.save(data_file1);
    x_encrypted_2.save(data_file2);

}

//seal::Ciphertext loadCiphertext(std::string filename, seal::EncryptionParameters parms) {
//
//    seal::SEALContext context(parms);
//
//    std::ifstream ct;
//    ct.open(filename, std::ios::binary);
//    seal::Ciphertext result;
//    result.load(context, ct);
//
//    return result;
//};