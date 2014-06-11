#ifndef _search__structure__
#define _search__structure__

#define getfilenumber(aaa) ((aaa)>>16)
#define getfileoffset(aaa) (((aaa)<<16)>>16)

#define makeFileNode(fileindex, offset) (((fileindex)<<16)+(offset))

struct IndexNode {
	unsigned int file;
	unsigned int key;
};

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

typedef char FileNode[64];

struct SearchResult {
	int resultcount;
	double elapsetime;
	unsigned int* result_index;
	FileNode* filenames;
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

extern const char* fkeyindex;
extern const char* fcontainer;
extern const char* fraw;
extern const char* fcompressindex;

extern int keynodelength;

int compareIndex(const CompareNode a, const CompareNode b);

#endif