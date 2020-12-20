#include "SQL_Database.h"
#include "utilities/seal_utilities_extra.h"

#include <iostream>
#include <filesystem>
#include <json/json.h>
#include <fstream>

void SQL_Database::load_session_key(std::string client_name) {

	// Decrypt session key using private key
	std::string command = "openssl rsautl -raw -decrypt -inkey " + DATABASE_CERTS + "private.pem" + " -in " + SWAP_FOLDER + client_name + "_session.enc" + " -out " + DATABASE_CERTS + "tmp_session.to_dec";
	system(command.c_str());

	// Verify and save session key using client's public key
	command = "openssl rsautl -verify -pubin -inkey " + DATABASE_CERTS + "Clients\\" + client_name + "\\public.pem ";
	command += "-in " + DATABASE_CERTS + "tmp_session.to_dec -out " + DATABASE_SESSION + client_name + "_session.key";
	command += " > " + TMP_FOLDER + "output.txt";
	system(command.c_str());

	// Get string to check verification (must be empty if all ok)
	std::ifstream in(TMP_FOLDER + "output.txt");
	std::string test;
	std::getline(in, test);
	in.close();

	// Remove extra files
	std::filesystem::remove(SWAP_FOLDER + client_name + "_session.enc");
	std::filesystem::remove(DATABASE_CERTS + "tmp_session.to_dec");
	std::filesystem::remove(TMP_FOLDER + "output.txt");

	// Check if verify was good and exits if not
	if (test.size() != 0) {
		std::cout << "Client is not certified!" << std::endl;
		exit(-1);
	}
}

void SQL_Database::not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination) {

	evaluator.negate(encrypted, destination);
	evaluator.add_plain_inplace(destination, plain_one);
}

void SQL_Database::not_inplace(seal::Ciphertext &encrypted) {

	evaluator.negate_inplace(encrypted);
	evaluator.add_plain_inplace(encrypted, plain_one);
}

void SQL_Database::or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination) {

	// a OR b = NOT (NOT a AND NOT b)
	seal::Ciphertext aux_1, aux_2;

	not_(encrypted_1, aux_1);
	not_(encrypted_2, aux_2);

	evaluator.multiply(aux_1, aux_2, destination);
	evaluator.relinearize_inplace(destination, relin_keys);
	not_inplace(destination);
}

void SQL_Database::or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2) {

	// a OR b = NOT (NOT a AND NOT b)
	seal::Ciphertext aux_1, aux_2;

	not_(encrypted_1, aux_1);
	not_(encrypted_2, aux_2);

	evaluator.multiply(aux_1, aux_2, encrypted_1);
	evaluator.relinearize_inplace(encrypted_1, relin_keys);
	not_inplace(encrypted_1);

}

seal::Ciphertext SQL_Database::compare(std::vector<seal::Ciphertext> x, std::vector<seal::Ciphertext> y, char operation) {

	// For every bit it checks if it's > or <  and saves a global bit for > and < if this bit is true it cannot be overriden
	// If global > is true then global < cannot be true. At the end if it's not < or > then it's =

	using namespace std;
	seal::Ciphertext great, lesst, equal;
	seal::Ciphertext not_x_i, not_y_i, not_lesst, not_great;
	seal::Ciphertext lesst_i, great_i;
	seal::Ciphertext blank;

	not_(x[0], not_x_i);
	not_(y[0], not_y_i);

	evaluator.multiply(y[0], not_x_i, lesst);
	evaluator.multiply(x[0], not_y_i, great);

	evaluator.relinearize_inplace(great, relin_keys);
	evaluator.relinearize_inplace(lesst, relin_keys);

	if (x.size() != y.size()) {	return blank; }
	for (size_t i = 1; i < x.size(); i++) {
		
		not_(x[i], not_x_i);
		not_(y[i], not_y_i);

		not_(great, not_great);
		not_(lesst, not_lesst);

		evaluator.multiply(not_x_i, y[i], lesst_i);
		evaluator.multiply(x[i], not_y_i, great_i);

		evaluator.relinearize_inplace(great_i, relin_keys);
		evaluator.relinearize_inplace(lesst_i, relin_keys);

		evaluator.multiply_inplace(great_i, not_lesst);
		evaluator.multiply_inplace(lesst_i, not_great);

		evaluator.relinearize_inplace(great_i, relin_keys);
		evaluator.relinearize_inplace(lesst_i, relin_keys);

		or_inplace(great, great_i);
		or_inplace(lesst, lesst_i);
	}

	not_(great, not_great);
	not_(lesst, not_lesst);

	evaluator.multiply(not_great, not_lesst, equal);
	evaluator.relinearize_inplace(equal, relin_keys);

	if (     operation == '>') { return great; }
	else if (operation == '<') { return lesst; }
	else if (operation == '=') { return equal; }

	return blank;
}

std::vector<seal::Ciphertext> SQL_Database::get_compare_vec(std::vector<std::string> full_data_paths, Encrypted_int to_compare, char operation) {

	std::vector<seal::Ciphertext> compare_vec(full_data_paths.size());

	std::cout << "Starting Comparisons ";
	for (int id = 0; id < compare_vec.size(); id++) {
		compare_vec[id] = compare(load_enc_int(context, full_data_paths[id]).bin_vec, to_compare.bin_vec, operation);
		std::cout << "..";
	} std::cout << " Done!" << std::endl;

	return compare_vec;
}

void SQL_Database::save_table(Json::Value table, std::string path) {

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ofstream out(path, std::ios::binary);

	writer->write(table, &out);
	out << std::endl;
	out.close();
}

Json::Value SQL_Database::load_table(std::string path) {

	Json::Value table;
	if (not std::filesystem::exists(path)) {
		response["response"] = "Table does not exist!";
		return table;
	}

	std::ifstream in(path, std::ios::binary);

	in >> table;
	in.close();
	return table;
}

void SQL_Database::create_table(Json::Value command) {

	// Checks for validity
	std::string tablename = command["table"].asString();
	if (std::filesystem::exists(DATABASE_TABLES + tablename)) {
		response["response"] = "Table already exists!";
		return;
	}

	std::set<std::string> columns;
	for (Json::Value column : command["columns"]) {
		columns.insert(column.asString());
	}

	if (columns.size() == 0) {
		response["response"] = "Column size should be atleast 1!";
		return;
	}

	// Creates table and directories
	Json::Value table;
	std::filesystem::create_directory(std::filesystem::path(DATABASE_TABLES + tablename));
	std::filesystem::create_directory(std::filesystem::path(DATABASE_TABLES + tablename + "\\__RANDOM__\\"));
	table["n_cols"] = columns.size();
	table["n_values"] = 0;
	table["files"] = Json::arrayValue;

	int index = 0;
	for (std::string column : columns) {
		
		std::filesystem::create_directory(std::filesystem::path(DATABASE_TABLES + tablename + "\\" + column));
		table["columns"][index++] = column;
	}

	save_table(table, DATABASE_TABLES + tablename + "\\" + "config.json");
	response["response"] = "Table was created correctly!";
	response["valid"] = true;
}

void SQL_Database::insert_values(Json::Value command) {

	std::string tablename = command["table"].asString();
	Json::Value table = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table.empty()) {
		return;
	}

	std::vector<std::string> columns(command["columns"].size());
	for (int col = 0; col < (int) command["columns"].size(); col++) {
		columns[col] = command["columns"][col].asString();
	}

	// Verify if all columns have values to fill
	std::set<std::string> columns_insert;
	for (std::string col : columns) {
		columns_insert.insert(col);
	}

	std::set<std::string> columns_table;
	for (Json::Value col : table["columns"]) {
		columns_table.insert(col.asString());
	}

	if (not (columns_insert == columns_table) &&			// If both sets are not equal
		not (columns.size() == columns_insert.size()) &&	// If input vector is not a set
		not (columns.size() == values.size()) ) {			// If no values missing
		response["response"] = "Columns not correct!";
		return;
	}

	// Starts insertion
	int n_values = table["n_values"].asInt();
	int new_id   = n_values == 0 ? 1 : table["files"][n_values - 1].asInt() + 1;

	std::string filename = std::to_string(new_id) + ".data";
	for (int i = 0; i < columns.size(); i++) {

		std::ofstream out(DATABASE_TABLES + tablename + "\\" + columns[i] + "\\" + filename, std::ios::binary);
		save_encripted(values[i], out);
		out.close();
	}

	std::ofstream out(DATABASE_TABLES + tablename + "\\__RANDOM__\\" + filename, std::ios::binary);
	random.save(out);
	out.close();
	
	table["files"].append(new_id);
	table["n_values"] = table["n_values"].asInt() + 1;

	save_table(table, DATABASE_TABLES + tablename + "\\" + "config.json");
	response["response"] = "Values were inserted correctly!";
	response["valid"] = true;
}

void SQL_Database::delete_line(Json::Value command) {

	std::string tablename = command["table"].asString();
	Json::Value table = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table.empty()) {
		return;
	}

	int linenum = command["linenum"].asInt();
	if (table["n_values"] < linenum) {
		response["response"] = "Line number too high!";
		return;
	}

	// Deletes line
	Json::Value file_ID;
	table["files"].removeIndex(linenum - 1, &file_ID);
	table["n_values"] = table["n_values"].asInt() - 1;

	std::string filename = std::to_string(file_ID.asInt()) + ".data";
	for (Json::Value col : table["columns"]) {

		std::string filepath = DATABASE_TABLES + tablename + "\\" + col.asString() + "\\" + filename;
		std::filesystem::remove(filepath);
	}

	save_table(table, DATABASE_TABLES + tablename + "\\" + "config.json");

	response["response"] = "Line " + std::to_string(linenum) + " was deleted!";
	response["valid"] = true;
}

void SQL_Database::select_line(Json::Value command) {

	std::string tablename = command["table"].asString();
	Json::Value table = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table.empty()) {
		return;
	}

	int linenum = command["linenum"].asInt();
	if (table["n_values"] < linenum) {
		response["response"] = "Line number too high!";
		return;
	}

	// Starts line select
	std::string filename = std::to_string(table["files"][linenum - 1].asInt()) + ".data";

	// Loads random
	seal::Ciphertext random;
	std::ifstream random_in(DATABASE_TABLES + tablename + "\\__RANDOM__\\" + filename, std::ios::binary);
	random.load(context, random_in);
	random_in.close();

	// Loads line
	int n_columns = table["n_cols"].asInt();
	std::vector<seal::Ciphertext> vector_enc(n_columns);
	for (int col = 0; col < n_columns; col++) {

		std::string filepath = DATABASE_TABLES + tablename + "\\" + table["columns"][col].asString() + "\\" + filename;
		std::ifstream in(filepath, std::ios::binary);
		vector_enc[col].load(context, in);
		in.close();
	}

	// Writes and encrypts response
	response["linenum"]  = linenum;
	response["n_column"] = n_columns;
	response["n_values"] = 1;
	response["columns"]  = table["columns"];
	response["valid"]    = true;

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::ofstream out(TMP_FOLDER + "response.txt", std::ios::binary);
	writer->write(response, &out);
	out << std::endl;

	out << " ====== Values below: ====== " << std::endl;
	random.save(out);
	for (int col = 0; col < n_columns; col++) {
		vector_enc[col].save(out);

	} out << std::endl;
	out.close();

	AES_crypt(TMP_FOLDER + "response.txt", SWAP_FOLDER + "response.aes", DATABASE_SESSION + command["client"].asString() + "_session.key");
	std::filesystem::remove(TMP_FOLDER + "response.txt");
}

void SQL_Database::select(Json::Value command) {

	// Verifies validity of command
	std::string tablename = command["table"].asString();
	Json::Value table_json = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table_json.empty()) {
		return;
	}

	int n_values = table_json["n_values"].asInt();
	int n_column = command["columns"].size();

	std::set<std::string> columns_set;
	for (Json::Value col : table_json["columns"]) {
		columns_set.insert(col.asString());
	}

	int index = 0;
	std::vector<std::string> columns(n_column);
	for (Json::Value col : command["columns"]) {

		if (not (columns_set.find(col.asString()) != columns_set.end())) {
			response["response"] = "Table does not contain column " + col.asString();
			return;
		}
		columns[index++] = col.asString();
	}

	// Loads all necessary values besides WHERE fields values
	std::vector<std::string> data_files(n_values);
	std::vector<seal::Ciphertext> random_enc(n_values);
	std::vector<std::vector<Encrypted_int>> table_enc(n_values, std::vector<Encrypted_int>(n_column));
	for (int id = 0; id < n_values; id++) {

		data_files[id] = std::to_string(table_json["files"][id].asInt()) + ".data";
		random_enc[id] = load_single_enc(context, DATABASE_TABLES + tablename + "\\__RANDOM__\\" + data_files[id]);
		
		int index = 0;
		for (std::string col : columns) {
			table_enc[id][index++] = load_enc_int(context, DATABASE_TABLES + tablename + "\\" + col + "\\" + data_files[id]);
		}
	}

	// Comparing function
	if (command["where"].isMember("condition_1")) {

		Json::Value condition = command["where"]["condition_1"];

		std::string column_name = condition["variable"].asString();
		if (not (columns_set.find(column_name) != columns_set.end())) {
			response["response"] = "Table does not contain column " + column_name;
			return;
		}

		std::vector<std::string> full_data_paths(n_values);
		for (int id = 0; id < n_values; id++) {
			full_data_paths[id] = DATABASE_TABLES + tablename + "\\" + column_name + "\\" + data_files[id];
		}

		std::vector<seal::Ciphertext> compare_vec = get_compare_vec(full_data_paths, val_cond_1, condition["operation"].asInt());

		// Condition 2
		if (command["where"].isMember("condition_2")) {

			condition   = command["where"]["condition_2"];
			column_name = condition["variable"].asString();
			if (not (columns_set.find(column_name) != columns_set.end())) {
				response["response"] = "Table does not contain column " + column_name;
				return;
			}

			for (int id = 0; id < n_values; id++) {
				full_data_paths[id] = DATABASE_TABLES + tablename + "\\" + column_name + "\\" + data_files[id];
			}

			// Apply junction of 2 conditions
			std::vector<seal::Ciphertext> compare_vec2 = get_compare_vec(full_data_paths, val_cond_2, condition["operation"].asInt());
			if (command["where"]["junction"].compare("AND") == 0) {
				for (int id = 0; id < n_values; id++) {
					evaluator.multiply_inplace(compare_vec[id], compare_vec2[id]);
					evaluator.relinearize_inplace(compare_vec[id], relin_keys);
				} 
			}
			else {
				for (int id = 0; id < n_values; id++) {
					or_inplace(compare_vec[id], compare_vec2[id]);
				}
			}
		}

		// Apply compare vector to values encrypted
		for (int id = 0; id < n_values; id++) {
			for (int col = 0; col < n_column; col++) {

				evaluator.multiply_inplace(table_enc[id][col].value, compare_vec[id]);
				evaluator.multiply_inplace(random_enc[id], compare_vec[id]);

				evaluator.relinearize_inplace(table_enc[id][col].value, relin_keys);
				evaluator.relinearize_inplace(random_enc[id], relin_keys);
			}
		}
	} 

	// Pack response
	bool select_col = command["function"].asString().compare("SELECT") == 0;
	bool select_sum = not select_col;
	
	seal::Ciphertext sum = table_enc[0][0].value;
	if (select_sum) {
		for (int id = 1; id < n_values; id++) {
			evaluator.add_inplace(sum, table_enc[id][0].value);
		}
	}

	if (select_col) {
		response["n_column"] = n_column;
		response["n_values"] = n_values;
		for (int col = 0; col < n_column; col++) {
			response["columns"][col] = columns[col];
		}
	}

	response["valid"] = true;

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	
	std::ofstream out(TMP_FOLDER + "response.txt", std::ios::binary);
	writer->write(response, &out);
	out << std::endl;

	out << " ====== Values below: ====== " << std::endl;

	if (select_col) {
		for (seal::Ciphertext random : random_enc) {
			random.save(out);
		}

		for (int id = 0; id < n_values; id++) {
			for (int col = 0; col < n_column; col++) {
				table_enc[id][col].value.save(out);
			}
		} out << std::endl;
	}

	if (select_sum) {
		sum.save(out);
	}

	out.close();

	// Encrypt response and send it to client
	AES_crypt(TMP_FOLDER + "response.txt", SWAP_FOLDER + "response.aes", DATABASE_SESSION + command["client"].asString() + "_session.key");
	std::filesystem::remove(TMP_FOLDER + "response.txt");
}

void SQL_Database::pack_simple_response(std::string client_name) {

	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::ofstream out(TMP_FOLDER + "response.txt", std::ios::binary);
	writer->write(response, &out);
	out << std::endl;
	out.close();

	// Encrypt response and send it to client
	AES_crypt(TMP_FOLDER + "response.txt", SWAP_FOLDER + "response.aes", DATABASE_SESSION + client_name + "_session.key");
	std::filesystem::remove(TMP_FOLDER + "response.txt");
}

void SQL_Database::unpack_command(std::string client_name) {
	
	// Decrypt request
	AES_crypt(SWAP_FOLDER + "request.aes", TMP_FOLDER + "request.txt", DATABASE_SESSION + client_name + "_session.key", true);
	std::filesystem::remove(SWAP_FOLDER + "request.aes");

	// Load command
	std::ifstream in(TMP_FOLDER + "request.txt", std::ios::binary);

	std::string aux, json_string;
	while (aux.compare(" ====== Values below: ====== ") != 0 && in.peek() != EOF) {

		std::getline(in, aux);
		json_string.append(aux);
		json_string.append("\n");
	}

	JSONCPP_STRING err;
	Json::CharReaderBuilder rbuilder;
	std::stringstream json_parse(json_string);

	Json::Value command;
	Json::parseFromStream(rbuilder, json_parse, &command, &err);

	// Load values if needed
	if (command.isMember("where")) {
		if (command["where"].isMember("condition_1")) {
			val_cond_1 = load_enc_int(context, in);
		}

		if (command["where"].isMember("condition_2")) {
			val_cond_2 = load_enc_int(context, in);
		}
	}

	if (command.isMember("values")) {

		random.load(context, in);
		values.resize(command["values"].asInt());
		for (int i = 0; i < command["values"].asInt(); i++) {
			values[i] = load_enc_int(context, in);
		}
	}
	in.close();
	std::filesystem::remove(TMP_FOLDER + "request.txt");

	// Pack appropriate response and send it to client
	response.clear();
	response["valid"] = false;

	std::string function = command["function"].asString();
	response["function"] = function;
	if (function.compare("SELECT") == 0 || function.compare("SELECT_SUM") == 0) {
		select(command);
	}
	else if (function.compare("CREATE") == 0) {
		create_table(command);
	}
	else if (function.compare("INSERT") == 0) {
		insert_values(command);
	}
	else if (function.compare("DELETE") == 0) {
		delete_line(command);
	}
	else if (function.compare("SELECT_LINE") == 0) {
		select_line(command);
	}
	
	// If response is simple and still not packed
	if (not (function.compare("SELECT") == 0 || function.compare("SELECT_SUM") == 0 || function.compare("SELECT_LINE") == 0)
		|| not response["valid"].asBool() ) {
		pack_simple_response(client_name);
	}
}
