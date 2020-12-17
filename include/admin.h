#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <assert.h>
#include <fstream>
#include <streambuf>
#include "Definitions.h"

using namespace std;



void admistrator(int tot_clients);


void client_signMessage(string msg, int client_nr);
void server_signMessage(string msg, int client_nr);

void server_verifySignature(string original_msg, int client_nr);
void client_verifySignature(string original_msg);


void client_encrypt(string plaintext, int client_nr);
string server_decrypt();


int new_client(int n_client);