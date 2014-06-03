#ifndef _INDEXER_H__
#define _INDEXER_H__

/**
 * @brief obtain an unique number of a given filename.
 * 
 * @param filename filename
 * @return the number
 */
unsigned int getFileNumber(const char* filename);

/**
 * @brief add all tokens to a temporary file for further compressing.
 * 
 * @param filename the original filename
 * @param to the temporary file.
 */
void addFileToIndex(const char* filename, const char* to);

int sortIndex(const char * filename);

/**
 * @brief merge keys in a sorted file_raw to filename_keyindex, only the 2nd int of each node remains.
 * 
 * @param filename_raw filename
 * @param filename_compress filename
 * @param filename_keyindex filename
 * @return the number of nodes
 */
int compressIndex(const char * filename_raw, const char * filename_compress, const char * filename_keyindex);

#endif