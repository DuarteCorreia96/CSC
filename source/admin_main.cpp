#include "Definitions.h"
#include "admin.h"

/// Sets up Database and Clients with requires certificates and keys
/// / data : 
///	|	/ Admin :
///	|	!	CA   : CA certificates and keys
///	|	!	SEAL : SEAL public, secret and relinearization keys
///	|	
///	|	/ Clients : Folder that will client information, they are private to the clients
///	|	
///	|	/ Database :
///	|	|	/ certs  : Has database and CA certificates and public/private keys
///	|	|	|	Clients : Save clients public_keys
///	|	|	|	SEAL	: Saves relinearization key
///	|	|	
///	|	|	/ tables : Saves tables information, 1 folder per table
///	|	
///	|	/ swap	: "Internet" folder, used to communicate between Database and clients
///	|
///	|	/ tmp	: tmp folder used by everyone, files written here should be deleted right away, 
///	|			: it's considered a private folder by the people writing
int main(){

	create_folders();
	generate_SEAL();
	generate_OpenSSL();

	std::string client_name = "client";
	generate_Clients(CLIENT_FOLDERS + client_name + "\\certs\\", "client-cert.crt", client_name);

	client_name = "admin";
	generate_Clients(CLIENT_FOLDERS + client_name + "\\certs\\", "client-cert.crt", client_name);

}
