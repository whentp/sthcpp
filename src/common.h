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

#ifndef COMMON_H
#define COMMON_H

#include "debug.h"

namespace bible{
	using namespace std;
	class StopWatch {
	private:
		clock_t start;
	public:
		StopWatch();
		double Stop();
	};

// file extensions.
	extern const char *file_ext_container_key;
	extern const char *file_ext_container_value;
	extern const char *file_ext_keyindex;
	extern const char *file_ext_compressedindex;

} // end namespace bible.

#endif
