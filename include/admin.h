#pragma once

#include <string>

/// <summary>
/// Creates folder organization as specified in admin_main.cpp.
/// </summary>
void create_folders();

/// <summary>
/// Generates SEAL keys.
/// </summary>
void generate_SEAL();

/// <summary>
/// Generates OpenSSL keys and certificates .
/// </summary>
void generate_OpenSSL();

/// <summary>
/// Generates/Copies all need keys and certificates to clients folders, while also creating them if needed.
/// </summary>
/// <param name="certs_folder">Client's certificate folder.</param>
/// <param name="cert_name">Desired filename for the certificate.</param>
/// <param name="client_name">Client name.</param>
/// <param name="database">Set to true if used to create database.</param>
void generate_Clients(std::string certs_folder, std::string cert_name, std::string client_name = "client", bool database = false);

/// <summary>
/// Used to verify if certificate is correctly signed by the CA
/// </summary>
/// <param name="certs_folder">Client's certificate folder.</param>
/// <param name="cert_name">Filename for the certificate.</param>
/// <returns>0 if certificate is valid, -1 if not.</returns>
int verify_cert(std::string certs_folder, std::string cert_name);
