#include "SQL_Database.h"
#include "seal_utilities_extra.h"

#include <iostream>
#include <filesystem>
#include <json/json.h>
#include <fstream>

void SQL_Database::not_inplace(seal::Ciphertext &encrypted) {

	evaluator.negate_inplace(encrypted);
	evaluator.add_plain_inplace(encrypted, plain_one);
}

void SQL_Database::not_(seal::Ciphertext &encrypted, seal::Ciphertext &destination) {

	evaluator.negate(encrypted, destination);
	evaluator.add_plain_inplace(destination, plain_one);
}

void SQL_Database::or_(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2, seal::Ciphertext &destination) {

	seal::Ciphertext aux_1, aux_2;

	not_(encrypted_1, aux_1);
	not_(encrypted_2, aux_2);

	evaluator.multiply(aux_1, aux_2, destination);
	evaluator.relinearize_inplace(destination, relin_keys);
	not_inplace(destination);
}

void SQL_Database::or_inplace(seal::Ciphertext &encrypted_1, seal::Ciphertext &encrypted_2) {

	seal::Ciphertext aux_1, aux_2;

	not_(encrypted_1, aux_1);
	not_(encrypted_2, aux_2);

	evaluator.multiply(aux_1, aux_2, encrypted_1);
	evaluator.relinearize_inplace(encrypted_1, relin_keys);
	not_inplace(encrypted_1);

}

seal::Ciphertext SQL_Database::compare(std::vector<seal::Ciphertext> x, std::vector<seal::Ciphertext> y, char operation) {

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

	if (PRINT_BUGET) {
		cout << "    + Noise Budget in equal: " << decryptor.invariant_noise_budget(equal) << " bits" << endl;
	}

	if (     operation == '>') { return great; }
	else if (operation == '<') { return lesst; }
	else if (operation == '=') { return equal; }

	return blank;
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
		std::cout << "Table does not exist!" << std::endl;
		return table;
	}

	std::ifstream in(path, std::ios::binary);

	in >> table;
	in.close();
}

void SQL_Database::create_table(std::string tablename, std::set<std::string> columns) {

	if (std::filesystem::exists(DATABASE_TABLES + tablename)) {
		std::cout << "Table already exists!" << std::endl;
		return;
	}

	if (columns.size() == 0) {
		std::cout << "Column size should be atleast 1!" << std::endl;
		return;
	}

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

}

void SQL_Database::insert_values(std::string tablename, std::vector<std::string> columns, std::vector<Encrypted_int> values, seal::Ciphertext random) {

	Json::Value table = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table.empty()) {
		return;
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
		std::cout << "Columns not correct!" << std::endl;
		return;
	}

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
}

void SQL_Database::delete_line(std::string tablename, int linenum) {

	Json::Value table = load_table(DATABASE_TABLES + tablename + "\\" + "config.json");
	if (table.empty()) {
		return;
	}

	if (table["n_values"] < linenum) {
		std::cout << "Line number too high!" << std::endl;
		return;
	}

	Json::Value file_ID;
	table["files"].removeIndex(linenum - 1, &file_ID);
	table["n_values"] = table["n_values"].asInt() - 1;

	std::string filename = std::to_string(file_ID.asInt()) + ".data";
	for (Json::Value col : table["columns"]) {

		std::string filepath = DATABASE_TABLES + tablename + "\\" + col.asString() + "\\" + filename;
		std::filesystem::remove(filepath);
	}

	save_table(table, DATABASE_TABLES + tablename + "\\" + "config.json");
}


std::vector<seal::Ciphertext> SQL_Database::get_compare_vec(std::vector<std::string> full_data_paths, std::string path_compare, char operation) {

	std::vector<seal::Ciphertext> compare_vec(full_data_paths.size());

	Encrypted_int to_compare = load_enc_int(context, path_compare);

	for (int id = 0; id < compare_vec.size(); id++) {
		compare_vec[id] = compare(load_enc_int(context, full_data_paths[id]).bin_vec, to_compare.bin_vec, operation);
	}

	return compare_vec;
}


void SQL_Database::select(std::string tablename, Json::Value command, SQL_Client &client) {

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
			std::cout << "Table does not contain column " << col.asString() << std::endl;
			return;
		}

		columns[index++] = col.asString();
	}

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
	if (command["where"]["condition_1"] != Json::nullValue) {

		Json::Value condition = command["where"]["condition_1"];

		std::string column_name = condition["variable"].asString();
		if (not (columns_set.find(column_name) != columns_set.end())) {
			std::cout << "Table does not contain column " << column_name << std::endl;
			return;
		}

		std::vector<std::string> full_data_paths(n_values);
		for (int id = 0; id < n_values; id++) {
			full_data_paths[id] = DATABASE_TABLES + tablename + "\\" + column_name + "\\" + data_files[id];
		}

		std::vector<seal::Ciphertext> compare_vec = get_compare_vec(full_data_paths, DATABASE_FOLDERS + "command\\cond_1.data", condition["operation"].asInt());

		if (command["where"]["condition_2"] != Json::nullValue) {

			condition   = command["where"]["condition_2"];
			column_name = condition["variable"].asString();
			if (not (columns_set.find(column_name) != columns_set.end())) {
				std::cout << "Table does not contain column " << column_name << std::endl;
				return;
			}

			for (int id = 0; id < n_values; id++) {
				full_data_paths[id] = DATABASE_TABLES + tablename + "\\" + column_name + "\\" + data_files[id];
			}

			std::vector<seal::Ciphertext> compare_vec2 = get_compare_vec(full_data_paths, DATABASE_FOLDERS + "command\\cond_2.data", condition["operation"].asInt());
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

		for (int id = 0; id < n_values; id++) {
			for (int col = 0; col < n_column; col++) {

				evaluator.multiply_inplace(table_enc[id][col].value, compare_vec[id]);
				evaluator.multiply_inplace(random_enc[id], compare_vec[id]);

				evaluator.relinearize_inplace(table_enc[id][col].value, relin_keys);
				evaluator.relinearize_inplace(random_enc[id], relin_keys);
			}
		}
	}

	client.print_table(table_enc, columns, random_enc);
}