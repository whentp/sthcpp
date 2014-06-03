#ifndef _search__structure__
#define _search__structure__

#define getfilenumber(aaa) (aaa)>>16
#define getfileoffset(aaa) ((aaa)<<16)>>16
#define makeFileNode(a,b) ((a)<<16)+(b)

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
};

typedef char FileNode[64];

struct SearchResult {
	int resultcount;
	double elapsetime;
	unsigned int* result_index;
	FileNode* result;
};

struct MemBlock {
	int length;
	int flag;
	char* block;
	MemBlock(size_t size);
	MemBlock();
	~MemBlock();
	void Free();
};

extern const char* fkeyindex;
extern const char* fcontainer;
extern const char* fraw;
extern const char* fcompressindex;

extern int keynodelength;

int compareIndex(const CompareNode a, const CompareNode b);

#endif