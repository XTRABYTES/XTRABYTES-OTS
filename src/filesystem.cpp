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


#include "Poco/FileStream.h"
#include "filesystem.hpp"

CFileSystem::CFileSystem(const Poco::Path &_base) {
	
	   boost::recursive_mutex::scoped_lock lLock(r_mtx);
		base = _base;           	
	   base.pushDirectory("xots");
      Poco::File(base).createDirectories();
} 

/*
bool CFileSystem::saveSecKeys(const std::vector<boost::multiprecision::uint256_t> &sec_keys_in) {

   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   Poco::File secKeyFile(Poco::Path(base, std::string("secret.keys")));
   if (secKeyFile.createFile()) {
   	
      Poco::FileOutputStream fos(secKeyFile.path(), std::ios::binary);
      if (fos.good()) {
			Poco::BinaryWriter writer(fos);
	
			for(size_t i=0; i < sec_keys_in.size(); i++) {
				std::vector<std::byte> raw_bytes;    
            uint256Insert(sec_keys_in[i], raw_bytes);    
            writer.writeRaw((const char*)raw_bytes.data(), raw_bytes.size());
			}
	      
         fos.close();
         return fos.good();
      }      

   }
   return false;
}*/

bool CFileSystem::savePubkey(const boost::multiprecision::uint256_t &pubkey_in) {

   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   Poco::File fPrintFile(Poco::Path(base, std::string("public.key")));
   if (fPrintFile.createFile()) {

      Poco::FileOutputStream fos(fPrintFile.path(), std::ios::binary);
      if (fos.good()) {
			Poco::BinaryWriter writer(fos);

			std::vector<std::byte> raw_bytes;    
         uint256Insert(pubkey_in, raw_bytes);    
         writer.writeRaw((const char*)raw_bytes.data(), raw_bytes.size());
         fos.close();
         return fos.good();
      }      

   }
   return false;
}


bool CFileSystem::saveSecKey(const boost::multiprecision::uint256_t &seckey_in) {

   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   Poco::File fPrintFile(Poco::Path(base, std::string("secret.key")));
   if (fPrintFile.createFile()) {

      Poco::FileOutputStream fos(fPrintFile.path(), std::ios::binary);
      if (fos.good()) {
			Poco::BinaryWriter writer(fos);

			std::vector<std::byte> raw_bytes;    
         uint256Insert(seckey_in, raw_bytes);    
         writer.writeRaw((const char*)raw_bytes.data(), raw_bytes.size());
         fos.close();
         return fos.good();
      }      

   }
   return false;
}


bool CFileSystem::loadSecKey( boost::multiprecision::uint256_t &sec_keys_out ) {
 
   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   Poco::File secKeyFile(Poco::Path(base, std::string("secret.key")));
   if (secKeyFile.exists()) {
			Poco::FileInputStream *fis = new Poco::FileInputStream(secKeyFile.path(), std::ios::binary); 

			while ( (!fis->eof())) {
				 std::vector<std::byte> raw_bytes;
				 raw_bytes.resize(32);
             fis->read( reinterpret_cast<char*>(raw_bytes.data()) , 32);
             
             std::streamsize len = fis->gcount();
				 if (len == 32) {    	    
	            boost::multiprecision::import_bits(sec_keys_out, raw_bytes.begin(), raw_bytes.end(), 8, true);       	    	    	       
				 }	else break;             
			}
         fis->close(); 

         return true;
   }   
   return false;
}

