#include "SQL_Client.h"

#include <vector>
#include <bitset>
#include <string>
#include <sstream>

seal::Ciphertext SQL_Client::get_random_enc() {

	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(1, PLAIN_MOD);

	int x = uni(rng);
	seal::Ciphertext x_enc{};
	encryptor.encrypt(seal::Plaintext(std::to_string(x)), x_enc);

	return x_enc;
}


Encrypted_int SQL_Client::encrypt_int(__int64 x) {

	Encrypted_int x_enc{};

	std::vector<seal::Ciphertext> x_vec_enc{};

	auto x_bin = std::bitset<N_BITS>(x);
	auto x_str = x_bin.to_string();

	for (size_t i = 0; i < x_str.size(); i++) {

		auto character = std::string(1, x_str.c_str()[i]);

		seal::Ciphertext aux_encrypted;
		encryptor.encrypt(seal::Plaintext(character), aux_encrypted);
		x_vec_enc.push_back(aux_encrypted);
	}

	seal::Ciphertext x_enc_value{};
	encryptor.encrypt(seal::Plaintext(std::to_string(x)), x_enc_value);

	x_enc.bin_vec = x_vec_enc;
	x_enc.value   = x_enc_value;

	return x_enc;
}

Decrypted_int SQL_Client::decrypt_int(Encrypted_int x_enc) {

	Decrypted_int x_dec;

	std::vector<int> x_vec_enc{};

	x_dec.bin_vec.resize(N_BITS);

	seal::Plaintext aux_plain;
	for (size_t i = 0; i < N_BITS; i++) {

		decryptor.decrypt(x_enc.bin_vec[i], aux_plain);
		x_dec.bin_vec[i] = std::atoi(aux_plain.to_string().c_str());
	}

	decryptor.decrypt(x_enc.value, aux_plain);
	x_dec.value = std::atoi(aux_plain.to_string().c_str());

	return x_dec;
}

void SQL_Client::print_table(std::vector<std::vector<seal::Ciphertext>> table_enc, 
	std::vector<std::string> columns, std::vector<seal::Ciphertext> random_enc) {

	int id_width = 6;
	int column_width = 15;

	std::cout << std::endl;
	std::cout << column_string("ID", id_width) << "|";
	for (int col = 0; col < table_enc[0].size(); col++) {
		std::cout << column_string(columns[col], column_width) << "|";
	} std::cout << std::endl;

	seal::Plaintext plain;
	for (int id = 0; id < table_enc.size(); id++) {

		decryptor.decrypt(random_enc[id], plain);
		if (plain.to_string().compare("0") == 0) {
			continue;
		}

		std::cout << column_string(std::to_string(id + 1), id_width) << "|";
		for (int col = 0; col < table_enc[0].size(); col++) {

			decryptor.decrypt(table_enc[id][col], plain);
			std::cout << column_string(plain.to_string(), column_width) << "|";

		} std::cout << std::endl;
	}
	std::cout << std::endl;
}

void SQL_Client::pack_command(Json::Value command) {

	std::ofstream out(SWAP_FOLDER + "request.txt", std::ios::binary);

	Json::Value value_cond_1;
	Json::Value value_cond_2;

	if (command.isMember("where")) {
		if (command["where"].isMember("condition_1")) {
			command["where"]["condition_1"].removeMember("value", &value_cond_1);
		}

		if (command["where"].isMember("condition_2")) {
			command["where"]["condition_2"].removeMember("value", &value_cond_2);
		}
	}

	Json::Value values;
	if (command.isMember("values")) {

		values = command["values"];
		command["values"]   = values.size();
	}

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	writer->write(command, &out);
	out << std::endl;

	out << " ====== Values below: ====== " << std::endl;
	if (value_cond_1 != Json::nullValue) {
		save_encripted(encrypt_int(value_cond_1.asInt()), out);
	}

	if (value_cond_2 != Json::nullValue) {
		save_encripted(encrypt_int(value_cond_1.asInt()), out);
	}

	if (values != Json::nullValue) {
		seal::Ciphertext random = get_random_enc();
		random.save(out);
		for (Json::Value value : values) {
			save_encripted(encrypt_int(value.asInt()), out);
		}
	} out << std::endl;
	out.close();
}

void SQL_Client::unpack_response() {

	std::ifstream in(SWAP_FOLDER + "response.txt", std::ios::binary);

	std::string aux, json_string;
	while (aux.compare(" ====== Values below: ====== ") != 0 && in.peek() != EOF) {

		std::getline(in, aux);
		json_string.append(aux);
		json_string.append("\n");
	}

	JSONCPP_STRING err;
	Json::CharReaderBuilder rbuilder;
	std::stringstream json_parse(json_string);

	Json::Value response;
	Json::parseFromStream(rbuilder, json_parse, &response, &err);

	std::cout << response["response"].asString() << std::endl;
	if (not response["valid"].asBool()) { return; }
	if (response["function"].asString().compare("SELECT") != 0) { return; }

	int n_values = response["n_values"].asInt();
	int n_column = response["n_column"].asInt();

	std::vector<std::string> columns(n_column);
	for (int col = 0; col < n_column; col++) {
		columns[col] = response["columns"][col].asString();
	}

	std::vector<seal::Ciphertext> random_enc(n_values);
	std::vector<std::vector<seal::Ciphertext>> table_enc(n_values, std::vector<seal::Ciphertext>(n_column));

	for (int id = 0; id < n_values; id++) {
		random_enc[id].load(context, in);
	}

	for (int id = 0; id < n_values; id++) {
		for (int col = 0; col < n_column; col++) {
			table_enc[id][col].load(context, in);
		} 
	}
	in.close();

	print_table(table_enc, columns, random_enc);
}