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

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */



using namespace std;
using namespace seal;

int main() {

	test_func();
	//uint64_t plain_modulus = 16;
	//seal::EncryptionParameters parms(seal::scheme_type::bfv);

	//size_t poly_modulus_degree = 8192;
	//parms.set_poly_modulus_degree(poly_modulus_degree);
	//parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
	//parms.set_plain_modulus(plain_modulus);

	//seal::SEALContext context(parms);
	//seal::KeyGenerator keygen(context);
	//seal::PublicKey public_key;
	//seal::RelinKeys relin_keys;

	//keygen.create_public_key(public_key);
	//keygen.create_relin_keys(relin_keys);
	//seal::SecretKey secret_key = keygen.secret_key();

	//seal::Encryptor encryptor(context, public_key);
	//seal::Decryptor decryptor(context, secret_key);
	//seal::Evaluator evaluator(context);

	//seal::Ciphertext x1, x2;
	//seal::Ciphertext y1, y2;

	//encryptor.encrypt(seal::Plaintext("1"), x1);
	//encryptor.encrypt(seal::Plaintext("2"), x2);
	//
	//ofstream out("data\\enc.txt", ios::binary);
	//x1.save(out);
	//x2.save(out);

	//out.close();

	//cout << "saved" << endl;

	//ifstream in("data\\enc.txt", ios::binary);
	//y1.load(context, in);
	//y2.load(context, in);

	//seal::Plaintext p1, p2;
	//decryptor.decrypt(y1, p1);
	//decryptor.decrypt(y2, p2);

	//cout << p1.to_string() << endl;
	//cout << p2.to_string() << endl;
}

