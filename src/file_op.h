#ifndef _FILE_H__
#define _FILE_H__

#include <string>
#include <vector>

namespace bible{
	/**
	 * @brief Obtain the length of file.
	 * @return The length of file.
	 */
	size_t getFileLength(const char* filename);

	///< attention: add \0 to the end of the file.
	void loadFile(const char * filename, char * &memblock, size_t &filesize);

	void createEmptyFile(const char* filename);

	void ensureFileExists(const char* filename);

	/**
	 * @brief Return all files and directories given a directory name.
	 * @return All files and directories
	 */
	std::vector<std::string> *getFilesInDirectory(const char *directory);
} // end namespace bible.
#endif
