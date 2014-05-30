#ifndef _search__structure__
#define _search__structure__

#define getfilenumber(aaa) (aaa)>>16
#define getfileoffset(aaa) ((aaa)<<16)>>16
#define makeFileNode(a,b) ((a)<<16)+(b)

struct memblock {
	int length;
	int flag;
	unsigned int* block;
};

struct IndexNode {
	unsigned int file;
	unsigned int key;
};

struct CompareNode {
	unsigned int n2;
	unsigned int n1;
};

struct keynode {
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

const char* fkeyindex="keyindex.index";
const char* fcontainer="container.index";
const char* fraw="raw.index";
const char* fcompressindex="compress.index";

int keynodelength;

int compareIndex(const void* a, const void* b);

void FreeMemBlock(struct memblock* aaa);

#endif