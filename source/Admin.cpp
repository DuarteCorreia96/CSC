#include "admin.h"


void admistrator(int tot_clients) {

	std::string CA_path = ADMIN_FOLDERS + "CA\\";

	// create server's directory
	std::filesystem::create_directory("..\\data\\server\\");
	system("mkdir ..\\data\\server\\");

	// generate key pair for root CA
	system(("openssl genrsa -des3 -out " + CA_path + "CA-key.pem 2048").c_str());

	// generate root CA certificate
	system("openssl req -new -key ..\\data\\CA\\CA-key.pem -x509 -days 1000 -out ..\\data\\CA\\CA-cert.pem");

	// send CA root certificate to the server
	system("copy ..\\data\\CA\\CA-cert.pem ..\\data\\server");

	// generate key pair of the server
	system("openssl genrsa -des3 -out ..\\data\\server\\server-key.pem 2048");

	// generate server certificate request and send it to the CA
	system("openssl req -new -config ..\\data\\CA\\openssl.cnf -key ..\\data\\server\\server-key.pem -out ..\\data\\CA\\signingReq.csr");

	// the certificate from our CA is used to sign the server's certificate
	system("openssl x509 -req -days 365 -in ..\\data\\CA\\signingReq.csr -CA ..\\data\\CA\\CA-cert.pem -CAkey ..\\data\\CA\\CA-key.pem -CAcreateserial -out ..\\data\\server\\server-cert.pem");

};


void client_signMessage(string msg, int client_nr) {

	// create file fo store signed message
	ofstream MyFile("..\\data\\client1\\msg.txt");

	// Write to the file
	MyFile << msg;

	// Close the file
	MyFile.close();

	// sign the message
	string command = "openssl dgst -sha256 -sign ..\\data\\client" + std::to_string(client_nr) + "\\client" + std::to_string(client_nr) + "-private-key.pem -out ..\\data\\client" + std::to_string(client_nr) + "\\sign.sha256 ..\\data\\client" + std::to_string(client_nr) + "\\msg.txt";
	system(command.c_str());

	// convert to base64
	string command1 = "openssl base64 -in ..\\data\\client" + std::to_string(client_nr) + "\\sign.sha256 -out ..\\data\\client" + std::to_string(client_nr) + "\\msg_signed";
	system(command1.c_str());

}


void server_signMessage(string msg, int client_nr) {

	// create file fo store signed message
	ofstream MyFile("..\\data\\server\\client" + std::to_string(client_nr) + "\\msg.txt");

	// Write to the file
	MyFile << msg;

	// Close the file
	MyFile.close();

	// sign the message
	string command = "openssl dgst -sha256 -sign ..\\data\\server\\server-key.pem -out ..\\data\\server\\client" + std::to_string(client_nr) + "\\sign.sha256 ..\\data\\server\\client" + std::to_string(client_nr) + "\\msg.txt";
	system(command.c_str());

	// convert to base64
	string command1 = "openssl base64 -in ..\\data\\server\\client" + std::to_string(client_nr) + "\\sign.sha256 -out ..\\data\\server\\client" + std::to_string(client_nr) + "\\msg_signed";
	system(command1.c_str());

}


void server_verifySignature(string original_msg, int client_nr) {

	// open file
	ofstream MyFile("..\\data\\client1\\original_msg.txt");

	// Write to the file
	MyFile << original_msg;

	// Close the file
	MyFile.close();

	system("openssl base64 -d -in ..\\data\\client1\\msg_signed -out ..\\data\\client1\\sign.sha256");
	string command = "openssl dgst -sha256 -verify ..\\data\\server\\clients\\client1\\client" + std::to_string(client_nr) + "-public-key.pem -signature ..\\data\\client1\\sign.sha256 ..\\data\\client1\\original_msg.txt";
	system(command.c_str());

}

void client_verifySignature(string original_msg) {

	// open file
	ofstream MyFile("..\\data\\client1\\msg_to_sign2.txt");

	// Write to the file
	MyFile << original_msg;

	// Close the file
	MyFile.close();

	system("openssl base64 -d -in ..\\data\\client1\\msg_signed -out ..\\data\\client1\\sign.sha256");
	system("openssl dgst -sha256 -verify ..\\data\\client1\\public1key.pem -signature ..\\data\\client1\\sign.sha256 ..\\data\\client1\\msg_to_sign2.txt");

}


void client_encrypt(string plaintext, int client_nr) {

	// put the string in a file
	ofstream MyFile("..\\data\\client" + std::to_string(client_nr) + "\\msg.txt");
	// Write to the file
	MyFile << plaintext;
	// Close the file
	MyFile.close();

	// ecrypt

	string command = "openssl rsautl -encrypt -pubin -inkey ..\\data\\client" + std::to_string(client_nr) + "\\server-public-key.pem -in ..\\data\\client" + std::to_string(client_nr) + "\\msg.txt -out ..\\data\\server\\clients\\client" + std::to_string(client_nr) + "\\encrypted-msg.txt";
	system(command.c_str());
}

string server_decrypt() {

	// decrypt
	system("openssl rsautl -decrypt -inkey ..\\data\\server\\server-key.pem -in ..\\data\\server\\clients\\client1\\encrypted-msg.txt -out ..\\data\\server\\clients\\client1\\plaintext.txt");

	// put the content of the file in a string
	ifstream t("..\\data\\server\\encrypted.txt"); //taking file as inputstream
	string str;
	if (t) {
		ostringstream ss;
		ss << t.rdbuf(); // reading data
		str = ss.str();
	}

	return str;
}


int new_client(int n_client) {

	n_client++;

	// create directories
	string command = "mkdir ..\\data\\server\\clients\\client" + std::to_string(n_client);
	system(command.c_str());
	string command1 = "mkdir ..\\data\\client" + std::to_string(n_client);
	system(command1.c_str());

	// send CA root certificate to a client
	string command2 = "copy ..\\data\\CA\\CA-cert.pem ..\\data\\client" + std::to_string(n_client);
	cout << "---->" << (command2) << endl;
	system(command2.c_str());

	// generate key pair of a client
	string command3 = "openssl genrsa -des3 -out ..\\data\\client" + std::to_string(n_client) + "\\client" + std::to_string(n_client) + "-private-key.pem 2048";
	cout << "---->" << (command3) << endl;
	system(command3.c_str());

	// get public key of a client
	string command4 = "openssl rsa -in ..\\data\\client" + std::to_string(n_client) + "\\client" + std::to_string(n_client) + "-private-key.pem -pubout > ..\\data\\server\\clients\\client" + std::to_string(n_client) + "\\client" + std::to_string(n_client) + "-public-key.pem";
	cout << "---->" << command4 << endl;
	system(command4.c_str());

	// send server's certificate to a client
	string command5 = "copy ..\\data\\server\\server-cert.pem ..\\data\\client" + std::to_string(n_client);
	cout << "---->" << (command5) << endl;
	system(command5.c_str());

	// verify server's certificate
	string command6 = "openssl verify -verbose -CAfile ..\\data\\client" + std::to_string(n_client) + "\\CA-cert.pem  ..\\data\\client" + std::to_string(n_client) + "\\server-cert.pem";
	cout << "---->" << (command6) << endl;
	system(command6.c_str());

	// obtain the public key of the server
	string command7 = "openssl x509 -pubkey -noout -in ..\\data\\client" + std::to_string(n_client) + "\\server-cert.pem  > ..\\data\\client" + std::to_string(n_client) + "\\server-public-key.pem";
	cout << "---->" << (command7) << endl;
	system(command7.c_str());

	return n_client;
}