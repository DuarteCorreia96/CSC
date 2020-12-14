#pragma once
#include <filesystem>
#include <string>
using namespace std;

void admistrator(int tot_clients) {
	system("openssl genrsa -des3 -out ..\\CA\\CA-key.pem 2048");
	//root CA certificate
	system("openssl req -new -key ..\\CA\\CA-key.pem -x509 -days 1000 -out ..\\CA\\CA-cert.pem");

	//put in a function
	//distribute the certificates
	for (int n_client = 1; n_client <= tot_clients; n_client++) {
		string command = "copy ..\\CA\\CA-cert.pem ..\\client";
		command = command + std::to_string(n_client);
		cout << command << endl;
		system(command.c_str());
	}

	system("copy ..\\CA\\CA-cert.pem ..\\server");

	system("openssl genrsa -des3 -out ..\\server\\server-key.pem 2048");

	//DOES NOT WORK. ONLY IN TERMINAL
	system("openssl req –new –config ..\\server\\openssl.cnf –key ..\\server\\server-key.pem –out ..\\CA\\signingReq.csr");

	system("openssl x509 -req -days 365 -in ..\\CA\\signingReq.csr -CA ..\\CA\\CA-cert.pem -CAkey ..\\CA\\CA-key.pem -CAcreateserial -out ..\\server\\server-cert.pem");


};