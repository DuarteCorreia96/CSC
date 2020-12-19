@ECHO OFF

::check number of arguments
SET argC=0
FOR %%x in (%*) DO Set /A argC+=1

IF NOT %argC% == 2 GOTO Syntax

SET client_number=%~1
SET admin_path=%~2\Admin\
SET client_path=%~2\Clients\client%client_number%\
SET server_cert_path=%~2\Database\certs\
SET client_path_in_db_folder=%server_cert_path%clients\client%client_number%\


:: Check if a client already exists
IF EXIST %client_path% GOTO ClientExist


:: create directories
IF NOT EXIST %client_path% md %client_path%
IF NOT EXIST %client_path_in_db_folder% md %client_path_in_db_folder%

:: send CA root certificate to a client
copy %admin_path%CA\CA-cert.pem %client_path%

:: generate key pair of a client
openssl genrsa -des3 -out %client_path%private-key.pem 2048

:: get public key of a client
openssl rsa -in %client_path%private-key.pem -pubout > %client_path_in_db_folder%public-key.pem

:: send server's certificate to a client
copy %server_cert_path%server-cert.pem %client_path%

:: obtain the public key of the server
ECHO hi
openssl x509 -pubkey -noout -in %client_path%server-cert.pem  > %client_path%server-public-key.pem
	
	

GOTO:EOF
:Syntax
ECHO first argument is the client number
ECHO second argument is the path to data folder
GOTO:EOF

:ClientExist
ECHO The client already exists