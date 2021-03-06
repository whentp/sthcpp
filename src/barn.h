/**
 * (C) Copyright 2014.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 3.0 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     whentp <tpsmsproject@gmail.com>
 */

#ifndef _BARN_H__
#define _BARN_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "structure.h"

using namespace std;

namespace bible{

	enum BarnOpenMode{
		BOM_LAZY,
		BOM_GREEDY,
		BOM_READONLY
	};

/**
 * @brief A record for a given string. flag1 and flag2 have not been used. 
 */
 struct BarnNode{
 	BibleIntType start;
 	BibleIntType length;
		unsigned int flag1; ///< not used
		unsigned int flag2; ///< not used
	};

/**
 * @brief A heap style container. store an integer index -> value. 
 */
 class Barn{
 private:
 	fstream _file_keys;
 	fstream _file_values;
 	size_t _file_keys_length;
 	size_t _file_values_length;
 	BarnOpenMode _mode;
 	const char* _mem = nullptr;
 public:
 	Barn(){};
 	Barn(
 		const char *filename_keys,
 		const char *filename_values,
 		BarnOpenMode mode);
 	Barn(
 		const char *filename_keys,
 		const char *filename_values): Barn(filename_keys, filename_values, BOM_GREEDY){}
 	~Barn();
		/**
		 * @brief Open files
		 * @details [long description]
		 * 
		 * @param filename_keys key filename
		 * @param filename_values value filename
		 */
		 void Open(
		 	const char *filename_keys,
		 	const char *filename_values);
		 
		 void Open(
		 	const char *filename_keys,
		 	const char *filename_values,
			BarnOpenMode mode ///< the mode is not used.
			);
		 void Close();

		 void Get(const size_t index, char * &to, size_t &length);

		 size_t Set(const char* content, const size_t length);

	/**
	 * @brief return _file_keys_length before merge
	 * 
	 * @param[in] filename_keys
	 * @param[in] filename_values
	 * 
	 * @return index numbers before merge
	 */
	 size_t Merge(
	 	const char *filename_keys,
	 	const char *filename_values);
	};


} // end namespace bible.
#endif
