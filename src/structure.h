#ifndef _search__structure__
#define _search__structure__

#include <stdint.h>

// basic type. change it to uint64 to support large indexes.
typedef uint32_t BibleIntType;

#define MAX_BIBLE_INT_VALUE 0xffffffff

#define FILE_TOKEN_COUNT_BIT_MAX 16 // NEVER touch here unless u know what u are doing.

#define getfilenumber(an_integer) ((an_integer) >> FILE_TOKEN_COUNT_BIT_MAX) ///< Very important.

#define getfileoffset(an_integer) ((an_integer) << (sizeof(BibleIntType)*8 - FILE_TOKEN_COUNT_BIT_MAX) >> (sizeof(BibleIntType)*8 - FILE_TOKEN_COUNT_BIT_MAX)) ///< Very important.

#define makeFileNode(fileindex, offset) (((fileindex) << FILE_TOKEN_COUNT_BIT_MAX)+(offset))

namespace bible{

	/**
	 * @brief Attention to the order of file and key.
	 */
	/*struct IndexNode {
	  BibleIntType file;
	  BibleIntType key;
	  };*/

	/**
	 * @brief Attention to the order of n1 and n2.
	 */
	struct CompareNode {
		BibleIntType n2;
		BibleIntType n1;
	};

	struct KeyNode {
		BibleIntType key;
		size_t start;
		size_t length;
		//int type; ///< the type of a keynode. 0: normal, 1: using zlib, maybe.
	};

	typedef char* FileNode;

	struct SearchResult {
		size_t count;
		double elapsetime;
		BibleIntType* indexes;
		FileNode* filenames; ///< the filenames. Should be NULL before the matchfilenames is called.
		size_t state;
		char *msg;
		SearchResult(): count(0), elapsetime(0), indexes(NULL), filenames(NULL), state(0), msg(NULL){}
		~SearchResult();
		void SetMsg(const char* msg);
	};

	struct MemBlock {
		size_t length;
		int flag;
		char* block;
		MemBlock(size_t size);
		MemBlock(): MemBlock(0) {}
		~MemBlock();
		void Lock(); ///< lock block and do not release block when destructor is called.
		void Free();
	};

	bool compareIndex(const CompareNode a, const CompareNode b);
} // end namespace bible.
#endif
