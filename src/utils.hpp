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

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "boost/multiprecision/cpp_int.hpp"

void uint256Insert(const boost::multiprecision::uint256_t &src, std::vector<std::byte> &dst);
boost::multiprecision::uint256_t uint256Hash(const boost::multiprecision::uint256_t &data_in);
bool computeHash(const std::vector<std::byte> &data_in, std::vector<std::byte> &hash_out);

#endif /* __UTILS_HPP__ */
