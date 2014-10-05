#ifndef _FILE_H__
#define _FILE_H__

#include <string>
#include <vector>

namespace bible {

/**
* Checks if a folder exists
* @param foldername path to the folder to check.
* @return true if the folder exists, false otherwise.
*/
bool folder_exists(std::string foldername);

/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int _mkdir(const char *path);

/**
 * Recursive, portable wrapper for mkdir.
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int mkdir(const char *path);

/**
 * @brief Obtain the length of file.
 * @return The length of file.
 */
size_t getFileLength(const char *filename);

///< attention: add \0 to the end of the file.
void loadTextFile(const char *filename, char*&memblock, size_t &filesize);

void loadFile(const char *filename, char*&memblock, size_t &filesize);

void createEmptyFile(const char *filename);

void ensureFileExists(const char *filename);

bool checkFileExists(const char *filename);

/**
 * @brief Return all files and directories given a directory name.
 * @return All files and directories
 */
std::vector<std::string> *getFilesInDirectory(const char *directory);
} // end namespace bible.
#endif
