#ifndef _HASH_H__
#define _HASH_H__
#include <stdlib.h>

namespace bible{
	void prepareCryptTable(); ///< must run once before generating hashcode.

	/**
	 * @brief hashString: convert string to hashcode
	 * @param[in] raw_string c-style string
	 * @param[in] dwHashType should be any in <1,2,3>. different value generates different hashcode.
	 * @return unsigned int hashcode.
	 * */
	unsigned int hashString(const char * raw_string, unsigned int dwHashType );
	unsigned int hashMem(const char * raw_string, size_t len, unsigned int dwHashType );///< convert mem section to hashcode. the same to hashString. length of the mem is required.

	extern unsigned int cryptTable[0x500];///< required by hashString and hashString
} // end namespace bible.
#endif
