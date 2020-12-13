#include "SQL_Database.h"

#include <iostream>

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