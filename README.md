# Homomorphic Database using SEAL

## Libraries needed

1. [Microsoft SEAL](https://github.com/Microsoft/SEAL)
2. [OpenSSL](https://github.com/openssl/openssl)
3. [JsonCpp](https://github.com/open-source-parsers/jsoncpp)

How to install them
===================

Recommended way when using Visual Studio is with [Microsoft's vcpkg](https://github.com/Microsoft/vcpkg)

SEAL
----
`.\vcpkg.exe install seal:x64-windows`

SEAL is include folder should be installed on `vcpkg\installed\x64-windows\include\seal` 
but sometimes it creates a parent directory `vcpkg\installed\x64-windows\include\SEAL-3-0\seal`
simply pulling the seal directory to the correct place solves any include issues.

OpenSSL
-------
`.\vcpkg.exe install openssl:x64-windows`

OpenSSL sometimes is not configured on the path and it should be added (on windows through environment variables)

Other bug may arise where it cannot find the openssl.cfg
simply copy it from `\vcpkg\packages\openssl_x64-windows\tools\openssl` to `\vcpkg\packages\openssl_x64-windows`

JsonCpp
-------
`.\vcpkg.exe install jsoncpp:x64-windows`

finally do `.\vcpkg.exe integrate install` to do integration with Visual Studio

Run Software
============

1. Launch solution (Homomorphic Database.sln) on Visual Studio 2019
2. Open Solution Explorer: View -> Solution Explorer (or Ctrl + Alt + L) (should look like this https://imgur.com/d205DPn)
3. Unload Project 1 (https://imgur.com/tOUn872)
4. Run Admin by pressing on "Local Windows Debugger" (ensure that it is on Release and x64 mode https://imgur.com/dbS3lF5)
5. Unload Admin and Load Project1
6. Run client by  pressing on "Local Windows Debugger" (ensure that it is on Release and x64 mode https://imgur.com/dbS3lF5)
7. Insert client name (see main_admin is possible to see that only the clients "admin" and "client" are created if more needed admin should be ran again with extra clients)
8. Inserts requests to the database.
9. After compilation Admin and Database can be run from `.\x64\Release\Admin.exe` and databse `.\x64\Release\Project1.exe`

It can also be externally compiled by using g++ with c++17 standard flag.

admin_main.cpp
--------------
`Admin.h(.cpp)`, `utilities\seal_utilities_extra.h(.cpp)` and `Definitions.h`


main.cpp
--------
`utilities\seal_utilities_extra.h(.cpp)`, `utilities\SQL_Command.h(.cpp)`, `Definitions.h`, `SQL_Client.h(.cpp)`and `SQL_Database.h(.cpp)`

