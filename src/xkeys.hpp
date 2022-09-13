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

#ifndef __XKEYS_HPP__
#define __XKEYS_HPP__

#include <vector>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include "filesystem.hpp"

class CXPubKeys
{
public:
  		CXPubKeys();
  		
  		void Reset();
  		bool SetPubkey(boost::multiprecision::uint256_t &pubkey_in);
  		bool SetMessageHash(boost::multiprecision::uint256_t &msghash_in);
  		bool SignatureIsValid(std::vector<boost::multiprecision::uint256_t> &signature_in);
      bool IsValid();
      
      private:

      boost::recursive_mutex r_mtx;

      bool fValid;
      bool isset_Hash;

      std::vector<int> index_keys;
      std::vector<boost::multiprecision::uint256_t> sec_keys;    
      std::vector<boost::multiprecision::uint256_t> pub_keys;
      std::vector<boost::multiprecision::uint256_t> proof_keys;
      
      boost::multiprecision::uint256_t cs_seck;
      boost::multiprecision::uint256_t cs_pubk;
      boost::multiprecision::uint256_t cs_proof;
      
      boost::multiprecision::uint256_t pubkey;  
      boost::multiprecision::uint256_t message_hash;
      std::vector<boost::multiprecision::uint256_t> signature;
};


class CXKeys
{
public:

  		CXKeys();
  		
  		void Reset();
      bool IsValid();

      void MakeNewKeys();
      bool ComputeSubKeys();
      bool SetKeys(const std::vector<boost::multiprecision::uint256_t> &keys_in, const boost::multiprecision::uint256_t &pubkey_in);      
      bool SaveKeys(CFileSystem &fs);
      bool SavePubkey(CFileSystem &fs);
      bool LoadKeys(CFileSystem &fs);
      boost::multiprecision::uint256_t GetPubkey();
      bool SignMsgHash(boost::multiprecision::uint256_t &msgHash, std::vector<boost::multiprecision::uint256_t> &signature_out);

private:

      boost::recursive_mutex r_mtx;

      bool fValid;
      bool isSetSeckeys;

      std::vector<boost::multiprecision::uint256_t> sec_keys;    
      std::vector<boost::multiprecision::uint256_t> pub_keys;
      std::vector<boost::multiprecision::uint256_t> proof_keys;
      
      boost::multiprecision::uint256_t cs_seck;
      boost::multiprecision::uint256_t cs_pubk;
      boost::multiprecision::uint256_t cs_proof;
      
      boost::multiprecision::uint256_t pubkey;      

};

#endif /* __XKEYS_HPP__ */
