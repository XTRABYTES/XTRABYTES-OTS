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


#include <openssl/rand.h>
#include <openssl/bn.h>

#include "xkeys.hpp"
#include "filesystem.hpp"
#include "utils.hpp"


CXKeys::CXKeys() : fValid(false) {
}

void CXKeys::Reset() {
       boost::recursive_mutex::scoped_lock lLock(r_mtx); 
       fValid = false; 
       cs_seck = 0; cs_pubk = 0; cs_proof = 0; pubkey=0;
       sec_keys.clear();
       pub_keys.clear();
       proof_keys.clear();
}

bool CXKeys::IsValid() {
      	 boost::recursive_mutex::scoped_lock lLock(r_mtx); 
          return fValid; 
      }


void CXKeys::MakeNewKeys() {
	
   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   Reset();
   isSetSeckeys = false;
   for (int i = 0; i < 8192; i++) {
       std::vector<std::byte> skey_bytes;
       skey_bytes.resize(32);	 	          
	    int ret = RAND_priv_bytes(reinterpret_cast<unsigned char*>(skey_bytes.data()), 32);
	    if (ret != 1) return;
	   	    
	    boost::multiprecision::uint256_t skey;	    	    
	    boost::multiprecision::import_bits(skey, skey_bytes.begin(), skey_bytes.end(), 8, true);       	    	    	    
	    sec_keys.push_back(skey);   
   }   
   
   if (!ComputeSubKeys()) return;
   fValid = true;
   
}

bool CXKeys::ComputeSubKeys() {

	 boost::recursive_mutex::scoped_lock lLock(r_mtx);
	 
    cs_seck = 0; cs_pubk = 0; cs_proof = 0; pubkey=0;
    pub_keys.clear();
    proof_keys.clear();
	 
	 for (int i = 0; i < 8192; i++) {	    	    
	    cs_seck+=sec_keys[i];
	    boost::multiprecision::uint256_t pkey;
	    pkey = uint256Hash(sec_keys[i]); 
       cs_pubk+=pkey;
       pub_keys.push_back(pkey);
	 }
       

    for (int i = 0; i < 8192; i++) {

          std::vector<std::byte> proof;
          std::vector<std::byte> proof_hash_bytes;
          proof.push_back(std::byte(i & 0xff));
          proof.push_back(std::byte((i>>8) & 0xff));

          boost::multiprecision::uint256_t sp,pp;
          sp = cs_seck - sec_keys[i];          
          pp = cs_pubk - pub_keys[i];
          
                                        
          uint256Insert(sp,proof);
          uint256Insert(pp,proof);
          
          if (!computeHash(proof, proof_hash_bytes)) return false;
          
	       boost::multiprecision::uint256_t proof_hash;
	       boost::multiprecision::import_bits(proof_hash, proof_hash_bytes.begin(), proof_hash_bytes.end(), 8, true);
          cs_proof+=proof_hash;
          proof_keys.push_back(proof_hash);
    }
    
    boost::multiprecision::uint256_t hcs_seck, hcs_pubk, hcs_proof;
    
    hcs_seck = uint256Hash(cs_seck);
    hcs_pubk = uint256Hash(cs_pubk);
    hcs_proof = uint256Hash(cs_proof);        
    
    if (hcs_seck==0 || hcs_pubk==0 || hcs_proof==0) return false; 
    
    std::vector<std::byte> pubkey_bytes;
    
    uint256Insert(hcs_seck, pubkey_bytes);    
    uint256Insert(hcs_pubk, pubkey_bytes);
    uint256Insert(hcs_proof, pubkey_bytes);
        
    std::vector<std::byte> pubkey_hash_bytes;    
    if (!computeHash(pubkey_bytes, pubkey_hash_bytes)) return false;    
    
    boost::multiprecision::import_bits(pubkey, pubkey_hash_bytes.begin(), pubkey_hash_bytes.end(), 8, true);            
    
    return true;
    
}

bool CXKeys::SaveKeys(CFileSystem &fs) {
	
		boost::recursive_mutex::scoped_lock lLock(r_mtx);
		if (!fValid) return false;
		return fs.saveSecKeys(sec_keys);
}

bool CXKeys::SavePubkey(CFileSystem &fs) {
	
		boost::recursive_mutex::scoped_lock lLock(r_mtx);
		if (!fValid) return false;
		return fs.savePubkey(pubkey);
}


bool CXKeys::SetKeys(const std::vector<boost::multiprecision::uint256_t> &keys_in, const boost::multiprecision::uint256_t &pubkey_in)
{
	fValid = false;
	sec_keys.clear();
	for (int i = 0; i < 8192; i++) {
	   sec_keys.push_back(keys_in[i]);
	}
	if (!ComputeSubKeys()) return false;
		
	if (pubkey != pubkey_in) return false;
	
   fValid = true;
   return true;
}


bool CXKeys::LoadKeys(CFileSystem &fs) {
	
		boost::recursive_mutex::scoped_lock lLock(r_mtx);
		if (fValid) return false;
		if (fs.loadSecKeys(sec_keys)) {  
	      if (ComputeSubKeys()) {
           fValid = true;
           return true;	      
	      }
		}
      return false;
}

boost::multiprecision::uint256_t CXKeys::GetPubkey() {
   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   if (!fValid) return 0;
   return pubkey;   
}

bool CXKeys::SignMsgHash(boost::multiprecision::uint256_t &msgHash, std::vector<boost::multiprecision::uint256_t> &signature_out) {
   boost::recursive_mutex::scoped_lock lLock(r_mtx);
   if (!fValid) return false;
   signature_out.clear();
   std::vector<std::byte> proof_bytes;
   std::vector<std::byte> msgWorkBuffer;
   std::vector<std::byte> msgHash_bytes;
   uint256Insert(msgHash, msgHash_bytes);
	uint256Insert(pubkey, msgHash_bytes);
	computeHash(msgHash_bytes,msgWorkBuffer);
	boost::multiprecision::uint256_t sum_seck, sum_pubk, sum_proof;
	sum_seck = 0; sum_pubk = 0; sum_proof = 0;
   for (int i = 0; i < 32; i++) {
	   boost::multiprecision::uint256_t sec_key;
	   int key_index = (i*256)+(int)msgWorkBuffer[i];
	   signature_out.push_back(sec_keys[key_index]);
	   sum_seck+=sec_keys[key_index];
	   sum_pubk+=pub_keys[key_index];
	   sum_proof+=proof_keys[key_index];
	   
	   proof_bytes.push_back((std::byte)i);
	   uint256Insert(proof_keys[key_index], proof_bytes);
	   std::vector<std::byte> msgNexHash_bytes;
	   msgNexHash_bytes = msgWorkBuffer;
      uint256Insert(sec_keys[key_index], msgNexHash_bytes);
      computeHash(msgNexHash_bytes,msgWorkBuffer);              
   }
   signature_out.push_back(cs_seck-sum_seck);   
   signature_out.push_back(cs_pubk-sum_pubk);
   signature_out.push_back(cs_proof-sum_proof);
   
   std::vector<std::byte> proof_bytes_hash;
   computeHash(proof_bytes,proof_bytes_hash);
   boost::multiprecision::uint256_t proof_hash;
   boost::multiprecision::import_bits(proof_hash, proof_bytes_hash.begin(), proof_bytes_hash.end(), 8, true);

   signature_out.push_back(proof_hash);
   return true;   
}


CXPubKeys::CXPubKeys()  {
    boost::recursive_mutex::scoped_lock lLock(r_mtx);
    Reset();
}

void CXPubKeys::Reset() {
       boost::recursive_mutex::scoped_lock lLock(r_mtx); 
       fValid = false; 
       isset_Hash = false;
       cs_seck = 0; cs_pubk = 0; cs_proof = 0; pubkey=0;
       sec_keys.clear();
       pub_keys.clear();
       proof_keys.clear();
       signature.clear();
}

bool CXPubKeys::IsValid() {
      	 boost::recursive_mutex::scoped_lock lLock(r_mtx); 
          return fValid; 
      }

bool CXPubKeys::SetPubkey(boost::multiprecision::uint256_t &pubkey_in) {	   
	   boost::recursive_mutex::scoped_lock lLock(r_mtx); 
	   if (fValid) return false;
      pubkey = pubkey_in;
      fValid = true;
      return true;
}

bool CXPubKeys::SetMessageHash(boost::multiprecision::uint256_t &msghash_in) {
	     boost::recursive_mutex::scoped_lock lLock(r_mtx);
        if (!fValid) return false;
        if (isset_Hash) return false;
        message_hash = msghash_in;
        isset_Hash = true;
        return true;
}


bool CXPubKeys::SignatureIsValid(std::vector<boost::multiprecision::uint256_t> &signature_in) {
	      boost::recursive_mutex::scoped_lock lLock(r_mtx);
         if (!fValid) return false;
         if (!isset_Hash) return false;

         signature = signature_in;
		   std::vector<std::byte> msgWorkBuffer;
		   std::vector<std::byte> msgHash_bytes;
		   uint256Insert(message_hash, msgHash_bytes);
			uint256Insert(pubkey, msgHash_bytes);
			computeHash(msgHash_bytes,msgWorkBuffer);
			boost::multiprecision::uint256_t sum_seck, sum_pubk, sum_proof;
			sum_seck = 0; sum_pubk = 0; sum_proof = 0;
			index_keys.resize(32);
			sec_keys.resize(32);
			pub_keys.resize(32);
			
         for (int i = 0; i < 32; i++) {
	          sec_keys[i] = signature_in[i];
	          sum_seck+=sec_keys[i];
	          index_keys[i] = (i*256)+(int)msgWorkBuffer[i];
	          std::vector<std::byte> msgNexHash_bytes;
	          msgNexHash_bytes = msgWorkBuffer;
             uint256Insert(sec_keys[i], msgNexHash_bytes);
             computeHash(msgNexHash_bytes,msgWorkBuffer);              
			    pub_keys[i] = uint256Hash(sec_keys[i]);
			    sum_pubk+=pub_keys[i];
			     
	      }  
	      
	      sum_seck+=signature_in[32];
	      sum_pubk+=signature_in[33];
	      std::vector<std::byte> proof_bytes;
	      for (int i = 0; i < 32; i++) {
	          std::vector<std::byte> proof;
	          std::vector<std::byte> proof_hash_bytes;
	          proof.push_back(std::byte(index_keys[i] & 0xff));
	          proof.push_back(std::byte((index_keys[i] >> 8) & 0xff));
	          boost::multiprecision::uint256_t sp,pp;
	          sp = sum_seck - sec_keys[i];	          
	          pp = sum_pubk - pub_keys[i];
	          uint256Insert(sp,proof);
	          uint256Insert(pp,proof);
	          if (!computeHash(proof, proof_hash_bytes)) return false;
		       boost::multiprecision::uint256_t proof_hash;
		       boost::multiprecision::import_bits(proof_hash, proof_hash_bytes.begin(), proof_hash_bytes.end(), 8, true);
	          sum_proof+=proof_hash;
	          proof_keys.push_back(proof_hash);
	 
	          proof_bytes.push_back((std::byte)i);
	          uint256Insert(proof_keys[i], proof_bytes);
	      }
	      sum_proof+=signature_in[34];

		   boost::multiprecision::uint256_t hcs_seck, hcs_pubk, hcs_proof;	    
		   hcs_seck = uint256Hash(sum_seck);
		   hcs_pubk = uint256Hash(sum_pubk);
		   hcs_proof = uint256Hash(sum_proof);

         

         if (hcs_seck==0 || hcs_pubk==0 || hcs_proof==0) return false;

         std::vector<std::byte> pubkey_bytes;    
         uint256Insert(hcs_seck, pubkey_bytes);    
         uint256Insert(hcs_pubk, pubkey_bytes);
         uint256Insert(hcs_proof, pubkey_bytes);

         std::vector<std::byte> pubkey_hash_bytes;    
         if (!computeHash(pubkey_bytes, pubkey_hash_bytes)) return false;    
         
         boost::multiprecision::uint256_t pubkey_from_sign;
         boost::multiprecision::import_bits(pubkey_from_sign, pubkey_hash_bytes.begin(), pubkey_hash_bytes.end(), 8, true);    
         
         
         if (pubkey_from_sign != pubkey) return false;
         
         
         std::vector<std::byte> proof_bytes_hash;
         computeHash(proof_bytes,proof_bytes_hash);
         boost::multiprecision::uint256_t proof_hash;
         boost::multiprecision::import_bits(proof_hash, proof_bytes_hash.begin(), proof_bytes_hash.end(), 8, true);
         
           
         if (proof_hash == signature_in[35]) return true;        
         
         return false;
}

