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


#ifndef __FILESYSTEM_HPP__
#define __FILESYSTEM_HPP__

#include <fstream>

#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/BinaryWriter.h"

#include "boost/thread/recursive_mutex.hpp"
#include "utils.hpp"

class CFileSystem
{
public:

  		CFileSystem(const Poco::Path &_base);
      bool saveSecKeys(const std::vector<boost::multiprecision::uint256_t> &sec_keys_in);
      bool savePubkey(const boost::multiprecision::uint256_t &pubkey_in);
      bool loadSecKeys(std::vector<boost::multiprecision::uint256_t> &sec_keys_out);
  		
private:

      Poco::Path base;
      boost::recursive_mutex r_mtx;

};

#endif /* __FILESYSTEM_HPP__ */
