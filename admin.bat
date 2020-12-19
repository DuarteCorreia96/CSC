@ECHO OFF

::check number of arguments
SET argC=0
FOR %%x in (%*) DO Set /A argC+=1

IF NOT %argC% == 2 GOTO Syntax

::make a directory
SET admin_path=%~1\
SET CA_path=%admin_path%CA\
SET server_path=%~2\
ECHO %server_path%
IF NOT EXIST %admin_path%openssl.cnf GOTO Exception
IF NOT EXIST %CA_path% md %CA_path%
IF NOT EXIST %server_path% md %server_path%

:: generate key pair for root CA
openssl genrsa -des3 -out %CA_path%CA-key.pem 2048

:: generate root CA certificate
openssl req -new -key %CA_path%CA-key.pem -x509 -days 1000 -out %CA_path%CA-cert.pem

:: send CA root certificate to the server
copy %CA_path%CA-cert.pem %server_path%

::generate key pair of the server
openssl genrsa -des3 -out %CA_path%server-key.pem 2048

::generate server certificate request and send it to the CA
openssl req -new -key %CA_path%server-key.pem -out %CA_path%signingReq.csr

::the certificate from our CA is used to sign the server's certificate
openssl x509 -req -days 365 -in %CA_path%signingReq.csr -CA %CA_path%CA-cert.pem -CAkey %CA_path%CA-key.pem -CAcreateserial -out %server_path%server-cert.pem



GOTO:EOF
:Syntax
ECHO first argument is path to admin folder
ECHO second argument is path to database certs folder

:Exception
ECHO admin folder needs openssl.cnf file


