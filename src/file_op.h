#ifndef _FILE_H__
#define _FILE_H__
namespace bible{
	int getFileLength(const char* filename);
	void loadFile(const char * filename, char * &memblock, size_t &filesize);
	void createEmptyFile(const char* filename);
	void ensureFileExists(const char* filename);
} // end namespace bible.
#endif