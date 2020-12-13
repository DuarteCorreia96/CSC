#include "Definitions.h"
#include "seal_utilities_extra.h"
#include "test.h"

#include <seal/seal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>

using namespace std;
using namespace seal;

int main() {

	test_SQL_Command();

	//test_func();
	/*seal::EncryptionParameters parms(seal::scheme_type::bfv);

	parms.set_poly_modulus_degree(POLY_MOD_DEG);
	parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(POLY_MOD_DEG));
	parms.set_plain_modulus(PLAIN_MOD);

	std::string client_name = "admin";

	seal::SEALContext context(parms);
	seal::PublicKey public_key = load_SEAL_public(context, client_name);
	seal::SecretKey secret_key = load_SEAL_secret(context, client_name);
	seal::RelinKeys relin_keys = load_SEAL_relin(context, client_name);

	SQL_Client client("admin", context, public_key, secret_key);

	seal::Encryptor encryptor(context, public_key);
	seal::Decryptor decryptor(context, secret_key);
	seal::Evaluator evaluator(context);

	seal::Ciphertext x1, x2;
	seal::Ciphertext y1, y2;

	encryptor.encrypt(seal::Plaintext("1"), x1);
	encryptor.encrypt(seal::Plaintext("2"), x2);
	
	ofstream out("data\\enc.txt", ios::binary);

	Json::Value root, function;
	Json::StreamWriterBuilder builder;
	root["function"]["pri"] = "select";
	root["function"]["sub"] = "sum";

	root["columns"][0] = "col1";
	root["columns"][1] = "col2";
	root["columns"][2] = "col3";

	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(root, &out);
	out << endl;

	out << "Values below:" << endl;
	x1.save(out);
	x2.save(out);

	out.close();

	cout << "saved" << endl;

	std::string test;

	ifstream in("data\\enc.txt", ios::binary);

	std::string json_string;
	while (test.compare("Values below:") != 0) {

		std::getline(in, test);
		json_string.append(test);
		json_string.append("\n");
	}

	JSONCPP_STRING err;
	Json::Value data;
	Json::CharReaderBuilder rbuilder;
	std::stringstream json_parse(json_string);
	Json::parseFromStream(rbuilder, json_parse, &data, &err);

	cout << data["function"]["pri"].asString() << endl;
	cout << data["function"]["sub"].asString() << endl;
	for (auto &col : data["columns"]) {
		cout << col.asString() << endl;
	}

	y1.load(context, in);
	y2.load(context, in);

	seal::Plaintext p1, p2;
	decryptor.decrypt(y1, p1);
	decryptor.decrypt(y2, p2);

	cout << p1.to_string() << endl;
	cout << p2.to_string() << endl;*/
}

 