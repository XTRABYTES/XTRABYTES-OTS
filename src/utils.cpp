/*
Copyright 2022 XTRABYTES
Copyright 2022 Zoltan Szabo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 3 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include <openssl/evp.h>
#include <string.h>
#include "boost/thread/recursive_mutex.hpp"

#include "utils.hpp"

boost::recursive_mutex r_mtx_uint256Insert;
void uint256Insert(const boost::multiprecision::uint256_t &src, std::vector<std::byte> &dst) {
   boost::recursive_mutex::scoped_lock lLock(r_mtx_uint256Insert);
   std::vector<unsigned char> byte_data;
   boost::multiprecision::export_bits(src, std::back_inserter(byte_data), 8,true);
	while( byte_data.size() < 32 ) {
        byte_data.insert(byte_data.begin(),0);
   }
   for (auto it = byte_data.begin(); it != byte_data.end(); ++it) {
    dst.push_back(static_cast<std::byte>(*it));
   }   
}

boost::recursive_mutex r_mtx_uint256Hash;
boost::multiprecision::uint256_t uint256Hash(const boost::multiprecision::uint256_t &data_in) 
{
	boost::recursive_mutex::scoped_lock lLock(r_mtx_uint256Hash);
   std::vector<std::byte> data_bytes;	
   std::vector<std::byte> hash_bytes;
	uint256Insert(data_in,data_bytes);	
	if (!computeHash(data_bytes, hash_bytes)) return 0;
	boost::multiprecision::uint256_t hash;
	boost::multiprecision::import_bits(hash, hash_bytes.begin(), hash_bytes.end(), 8, true);
   return hash;
}

boost::recursive_mutex r_mtx_computeHash;
bool computeHash(const std::vector<std::byte> &data_in, std::vector<std::byte> &hash_out)
{
	 boost::recursive_mutex::scoped_lock lLock(r_mtx_computeHash);
    bool success = false;
    
    hash_out.clear();
    EVP_MD_CTX* context = EVP_MD_CTX_create();

    if(context != NULL)
    {
    	
        if(EVP_DigestInit_ex(context, EVP_sha3_256(), NULL))
        {
            if(EVP_DigestUpdate(context, data_in.data(), data_in.size()))
            {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if(EVP_DigestFinal_ex(context, hash, &lengthOfHash))
                {
                    if (lengthOfHash==32) {
                       //memcpy(&hash_out, hash, lengthOfHash);
                       for (int i = 0; i < lengthOfHash; i++) {
                          hash_out.push_back(std::byte(hash[i]));                       
                       }
                       success = true;
                    }   
                }
                
            }
        }

        EVP_MD_CTX_destroy(context);
    }

    return success;
}
