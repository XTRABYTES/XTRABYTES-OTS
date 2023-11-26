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


#include <gtest/gtest.h>

#include "Poco/TemporaryFile.h"

#include "../src/xkeys.hpp"
#include "test-data/xkeys.hpp"

 

TEST(CXKeys, MakeNewKeys) {
	 CXKeys keys;	 
	 EXPECT_FALSE(keys.IsValid());
    keys.MakeNewKeys();
    EXPECT_TRUE(keys.IsValid());    
}


TEST(CXKeys, Reset) {
	 CXKeys keys;	 
	 EXPECT_FALSE(keys.IsValid());
    keys.MakeNewKeys();
    EXPECT_TRUE(keys.IsValid());
    keys.Reset();
    EXPECT_FALSE(keys.IsValid());
}


TEST(CXKeys, SetKeys) {
    CXKeys keys;
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey)); 
    EXPECT_TRUE(keys.IsValid());
    EXPECT_FALSE(keys.SetKeys(0, t1_pubkey)); 
}


TEST(CXKeys, SaveKeys) {
	 CXKeys keys;	 
	 EXPECT_FALSE(keys.IsValid());
	 
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));

    EXPECT_TRUE(keys.IsValid());
    
    Poco::TemporaryFile temp;
    temp.keepUntilExit();            
    CFileSystem fsystem(temp.path());
    
    EXPECT_TRUE(keys.SaveKeys(fsystem));
    EXPECT_FALSE(keys.SaveKeys(fsystem));
}


TEST(CXKeys, SavePubkey) {
	 CXKeys keys;	 
	 EXPECT_FALSE(keys.IsValid());
    
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));
    EXPECT_TRUE(keys.IsValid());
    
    Poco::TemporaryFile temp;
    temp.keepUntilExit();            
    CFileSystem fsystem(temp.path());
    
    EXPECT_TRUE(keys.SavePubkey(fsystem));
    EXPECT_FALSE(keys.SavePubkey(fsystem));
}



TEST(CXKeys, GetPubkey) {
    CXKeys keys;
    
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));
    EXPECT_TRUE(keys.IsValid());
    boost::multiprecision::uint256_t pubkey; 
    pubkey = keys.GetPubkey();
    EXPECT_TRUE( pubkey == t1_pubkey ); 
}



TEST(CXKeys, LoadKeys) {    
    Poco::TemporaryFile temp;
    temp.keepUntilExit();            
    CFileSystem fsystem(temp.path());
 
    CXKeys keys;   
    EXPECT_FALSE(keys.IsValid());    
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));
    EXPECT_TRUE(keys.IsValid());   
    EXPECT_FALSE(keys.LoadKeys(fsystem));
    keys.Reset();
    EXPECT_FALSE(keys.IsValid());
    EXPECT_FALSE(keys.LoadKeys(fsystem));
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));
    EXPECT_TRUE(keys.IsValid());   
    EXPECT_TRUE(keys.SaveKeys(fsystem));
    keys.Reset();
    EXPECT_FALSE(keys.IsValid());
    EXPECT_TRUE(keys.LoadKeys(fsystem));
    boost::multiprecision::uint256_t pubkey; 
    pubkey = keys.GetPubkey();
    EXPECT_TRUE( pubkey == t1_pubkey ); 
}


TEST(CXKeys, SignMsgHash) {    
    CXKeys keys;   
    EXPECT_FALSE(keys.IsValid());
    boost::multiprecision::uint256_t msgHash;
    std::vector<boost::multiprecision::uint256_t> signature_out;
    EXPECT_FALSE(keys.SignMsgHash(msgHash, signature_out));
      
    EXPECT_TRUE(keys.SetKeys(t1_seckey, t1_pubkey));
    EXPECT_TRUE(keys.IsValid());
    msgHash=0;          
    
    EXPECT_TRUE(keys.SignMsgHash(t1_message_hash, signature_out));
    std::vector<std::byte> signature_bytes;
    for (size_t i = 0; i < signature_out.size(); i++) {
    	uint256Insert(signature_out[i],signature_bytes);
    }
       
    EXPECT_TRUE( 0 == std::memcmp( signature_bytes.data(), t1_signature_uc, sizeof( t1_signature_uc ) ) );
    EXPECT_TRUE(keys.SignMsgHash(t2_message_hash, signature_out));
    signature_bytes.clear();
    for (size_t i = 0; i < signature_out.size(); i++) {
    	uint256Insert(signature_out[i],signature_bytes);
    }
    EXPECT_FALSE( 0 == std::memcmp( signature_bytes.data(), t1_signature_uc, sizeof( t1_signature_uc ) ) );         
}


TEST(CXPubKeys, SetPubkey) {
	 CXPubKeys pubkeys;	 
	 EXPECT_FALSE(pubkeys.IsValid());
    EXPECT_TRUE(pubkeys.SetPubkey(t1_pubkey));
    EXPECT_TRUE(pubkeys.IsValid());
    EXPECT_FALSE(pubkeys.SetPubkey(t1_pubkey));
}

TEST(CXPubKeys, Reset) {
	 CXPubKeys pubkeys;	 
	 EXPECT_FALSE(pubkeys.IsValid());
    EXPECT_TRUE(pubkeys.SetPubkey(t1_pubkey));
    EXPECT_TRUE(pubkeys.IsValid());
    pubkeys.Reset();
    EXPECT_FALSE(pubkeys.IsValid());
}

TEST(CXPubKeys, SetMessageHash) {
	 CXPubKeys pubkeys;	 
	 EXPECT_FALSE(pubkeys.IsValid());
	 EXPECT_FALSE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_TRUE(pubkeys.SetPubkey(t1_pubkey));
    EXPECT_TRUE(pubkeys.IsValid());
    EXPECT_TRUE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_FALSE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_TRUE(pubkeys.IsValid());
}


TEST(CXPubKeys, SignatureIsValid) {
	 CXPubKeys pubkeys;	 
	 std::vector<boost::multiprecision::uint256_t> t1_signature;    
    for (int i = 0; i < 36; i++) {
       boost::multiprecision::uint256_t signature_item;
       std::vector<std::byte> signature_bytes;       
       signature_bytes.resize(32);
       memcpy(&signature_bytes[0], &t1_signature_uc[32*i], 32);         	       	    	    
	    boost::multiprecision::import_bits(signature_item, signature_bytes.begin(), signature_bytes.end(), 8, true);       	    	    	    
	    t1_signature.push_back(signature_item);    
    }
	 
	 EXPECT_FALSE(pubkeys.IsValid());
	 EXPECT_FALSE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_TRUE(pubkeys.SetPubkey(t1_pubkey));
    EXPECT_TRUE(pubkeys.IsValid());
    EXPECT_FALSE(pubkeys.SignatureIsValid(t1_signature));
    EXPECT_TRUE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_FALSE(pubkeys.SetMessageHash(t1_message_hash));
    EXPECT_TRUE(pubkeys.SignatureIsValid(t1_signature));
    EXPECT_TRUE(pubkeys.IsValid());
}
