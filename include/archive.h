#pragma once

#include "Definitions.h"

#include <bitset>
#include <iostream>

void compare_testing() {

	using namespace std;
	bool lesst, great, equal;

	const unsigned __int64 n_bits = 12;

	int x = 30;
	int y = 46;

	auto x_bin = std::bitset<n_bits>(x);
	auto y_bin = std::bitset<n_bits>(y);

	// Version of only NOT and AND
	lesst = !x_bin[n_bits - 1] && y_bin[n_bits - 1];
	great = x_bin[n_bits - 1] && !y_bin[n_bits - 1];
	for (__int64 i = n_bits - 2; i >= 0; i--) {

		lesst = !(!(!great && (!x_bin[i] && y_bin[i])) && !lesst);
		great = !(!(!lesst && (x_bin[i] && !y_bin[i])) && !great);
	}
	equal = !lesst && !great;

	cout << x;

	if (lesst) { cout << " < "; }
	else if (great) { cout << " > "; }
	else if (equal) { cout << " = "; }

	cout << y << endl;

	// Mixed Version
	lesst = !x_bin[n_bits - 1] && y_bin[n_bits - 1];
	great = x_bin[n_bits - 1] && !y_bin[n_bits - 1];
	for (__int64 i = n_bits - 2; i >= 0; i--) {

		lesst |= !great && (!x_bin[i] && y_bin[i]);
		great |= !lesst && (x_bin[i] && !y_bin[i]);
	}
	equal = !lesst && !great;

	cout << x;

	if (lesst) { cout << " < "; }
	else if (great) { cout << " > "; }
	else if (equal) { cout << " = "; }

	cout << y << endl;

	// Version of only NOT and OR
	lesst = !(x_bin[n_bits - 1] || !y_bin[n_bits - 1]);
	great = !(!x_bin[n_bits - 1] || y_bin[n_bits - 1]);
	for (__int64 i = n_bits - 2; i >= 0; i--) {

		lesst = !(great || (x_bin[i] || !y_bin[i])) || lesst;
		great = !(lesst || (!x_bin[i] || y_bin[i])) || great;
	}
	equal = !(lesst || great);

	cout << x;

	if (lesst) { cout << " < "; }
	else if (great) { cout << " > "; }
	else if (equal) { cout << " = "; }

	cout << y << endl;
}