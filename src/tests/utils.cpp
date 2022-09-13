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

#include "test-data/utils.hpp"

TEST(Utils, computeHash) {
			 
	 std::vector<std::byte> t1_hash_out;
	 std::vector<std::byte> t1_data_in;

    for (long unsigned int i = 0; i < sizeof(t1_data_in_uc); i++) {
       t1_data_in.push_back(std::byte(t1_data_in_uc[i]));                       
    }
 
    EXPECT_TRUE(computeHash(t1_data_in, t1_hash_out));
    EXPECT_TRUE( 0 == std::memcmp( t1_hash_out.data(), t1_hash_out_uc, sizeof( t1_hash_out_uc ) ) );

    t1_data_in.push_back(std::byte(1));     
    EXPECT_TRUE(computeHash(t1_data_in, t1_hash_out));
    EXPECT_FALSE( 0 == std::memcmp( t1_hash_out.data(), t1_hash_out_uc, sizeof( t1_hash_out_uc ) ) );    
        
}

TEST(Utils, uint256Hash) {
     boost::multiprecision::uint256_t t2_hash_tmp;   
     t2_hash_tmp = uint256Hash(t2_data_in);
     EXPECT_TRUE( t2_hash_tmp == t2_hash ); 
}

TEST(Utils, uint256Insert) {
     std::vector<std::byte> buffer;   
     uint256Insert(t3_value1,buffer);
     uint256Insert(t3_value2,buffer);
     EXPECT_TRUE( 0 == std::memcmp( buffer.data(), t3_result_uc, sizeof( t3_result_uc ) ) );      
}

