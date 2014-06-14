#ifndef _search__structure__
#define _search__structure__

#define getfilenumber(aaa) ((aaa)>>16) ///< Very important.
#define getfileoffset(aaa) (((aaa)<<16)>>16) ///< Very important.

#define makeFileNode(fileindex, offset) (((fileindex)<<16)+(offset))
namespace bible{

	/**
	 * @brief Attention to the order of file and key.
	 */
	struct IndexNode {
		unsigned int file;
		unsigned int key;
	};

	/**
	 * @brief Attention to the order of n1 and n2.
	 */
	struct CompareNode {
		unsigned int n2;
		unsigned int n1;
	};

	struct KeyNode {
		unsigned int key;
		int start;
		int length;
		//int type; ///< the type of a keynode. 0: normal, 1: using zlib, maybe.
	};

	typedef char* FileNode;

	struct SearchResult {
		size_t resultcount;
		double elapsetime;
		unsigned int* result_index;
		FileNode* filenames; ///< the filenames. Should be NULL before the matchfilenames is called.
		SearchResult(): resultcount(0), result_index(NULL), filenames(NULL){}
		~SearchResult();
	};

	struct MemBlock {
		int length;
		int flag;
		char* block;
		MemBlock(size_t size);
		MemBlock(): MemBlock(0) {}
		~MemBlock();
		void Lock(); ///< lock block and do not release block when destructor is called.
		void Free();
	};

	int compareIndex(const CompareNode a, const CompareNode b);
} // end namespace bible.
#endif
