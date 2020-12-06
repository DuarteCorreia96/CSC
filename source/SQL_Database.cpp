#include "SQL_Database.h"
#include <seal/seal.h>

void SQL_Database::relinearize_(seal::Ciphertext& encrypted) {

	evaluator.relinearize_inplace(encrypted, relin_keys);
}


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
	relinearize_(encrypted_1);
	not_inplace(encrypted_1);

}

void SQL_Database::compare(
	std::vector<seal::Ciphertext> x, 
	std::vector<seal::Ciphertext> y, 
	unsigned __int64 n_bits) {

	using namespace std;
	seal::Ciphertext not_x_i, not_y_i, not_lesst, not_great;
	seal::Ciphertext lesst_i, great_i;

	not_(x[n_bits - 1], not_x_i);
	not_(y[n_bits - 1], not_y_i);

	evaluator.multiply(y[n_bits - 1], not_x_i, lesst);
	evaluator.multiply(x[n_bits - 1], not_y_i, great);

	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(great) << " bits" << endl;

	relinearize_(great);
	relinearize_(lesst);

	cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(great) << " bits" << endl;

	seal::Plaintext great_p, lesst_p;
	decryptor.decrypt(great, great_p);
	decryptor.decrypt(lesst, lesst_p);

	//cout << "Greater   :\t" << great_p.to_string() << endl;
	//cout << "Less than :\t" << lesst_p.to_string() << endl;

	for (__int64 i = n_bits - 2; i >= 0; i--) {

		cout << "Bit: \t" << i << endl;
		
		not_(x[i], not_x_i);
		not_(y[i], not_y_i);

		not_(great, not_great);
		not_(lesst, not_lesst);

		evaluator.multiply(not_x_i, y[i], lesst_i);
		evaluator.multiply(x[i], not_y_i, great_i);

		relinearize_(great_i);
		relinearize_(lesst_i);

		evaluator.multiply_inplace(great_i, not_lesst);
		evaluator.multiply_inplace(lesst_i, not_great);

		relinearize_(great_i);
		relinearize_(lesst_i);

		or_inplace(great, great_i);
		or_inplace(lesst, lesst_i);
		cout << "    + noise budget in encrypted_result: " << decryptor.invariant_noise_budget(great) << " bits" << endl;

		seal::Plaintext great_p, lesst_p;
		decryptor.decrypt(great, great_p);
		decryptor.decrypt(lesst, lesst_p);


		//cout << "Greater   :\t" << great_p.to_string() << endl;
		//cout << "Less than :\t" << lesst_p.to_string() << endl;
	}

	evaluator.multiply(great, lesst, equal);
	evaluator.relinearize_inplace(equal, relin_keys);
}