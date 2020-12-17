#pragma once

#include <string>

// Powers of 2: 4096 8192 16384 32768

const unsigned __int64 N_BITS = 12;
const uint64_t PLAIN_MOD  = 4096;
const size_t POLY_MOD_DEG = 32768;

const std::string ADMIN_FOLDERS = "data\\Admin\\";
const std::string ADMIN_SEAL    = ADMIN_FOLDERS + "SEAL\\";
const std::string ADMIN_CA      = ADMIN_FOLDERS + "CA\\";

const std::string CLIENT_FOLDERS   = "data\\Clients\\";
const std::string DATABASE_FOLDERS = "data\\Database\\";
const std::string DATABASE_TABLES  = DATABASE_FOLDERS + "tables\\";
const std::string DATABASE_CERTS   = DATABASE_FOLDERS + "certs\\";
