#include "SQL_Database.h"

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

	cout << "    + Noise Budget in equal: " << decryptor.invariant_noise_budget(equal) << " bits" << endl;
	cout << "    + Noise Budget in great: " << decryptor.invariant_noise_budget(equal) << " bits" << endl;

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

	Json::Value table;
	std::filesystem::create_directory(std::filesystem::path(DATABASE_FOLDERS + tablename));
	table["n_cols"] = columns.size();
	table["n_values"] = 0;
	table["files"] = Json::arrayValue;

	int index = 0;
	for (std::string column : columns) {
		
		std::filesystem::create_directory(std::filesystem::path(DATABASE_FOLDERS + tablename + "\\" + column));
		table["columns"][index++] = column;
	}

	save_table(table, DATABASE_FOLDERS + tablename + "\\" + "config.json");

}

void SQL_Database::insert_values(std::string tablename, std::vector<std::string> columns, std::vector<Encrypted_int> values) {

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