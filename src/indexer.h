#ifndef _INDEXER_H__
#define _INDEXER_H__
namespace bible{

	/**
	 * @brief tokenize the content of a given file, then add all tokens to a temporary file for further compressing.
	 * 
	 * @param filename the original filename
	 * @param to the temporary file.
	 * @param fcontainer the filename container.
	 */
	void addFileToIndex(
			const char* filename,
			const char* to,
			const char* fcontainer);

	/**
	 * @brief add all tokens of a char* to a temporary file for further compressing.
	 * 
	 * @param keystr just a name.
	 * @param valuestr the string that want to be indexed.
	 * @param to the temporary file.
	 * @param fcontainer the filename container.
	 */
	void addTextToIndex(
		const char *keystr,
		const char *valuestr,
		const char *to,
		const char *fcontainer);

	size_t sortIndex(const char * filename);

	/**
	 * @brief merge keys in a sorted file_raw to filename_keyindex, only the 2nd int of each node remains.
	 * 
	 * @param filename_raw filename
	 * @param filename_compress filename
	 * @param filename_keyindex filename
	 * @return the number of nodes
	 */
	size_t compressIndex(
			const char * filename_raw,
			const char * filename_compress,
			const char * filename_keyindex);

	/**
	 * @brief merge two indexes.
	 * @details merge two index files. each index contains a container, a keyindex, a compressed index.
	 * 
	 * @param container1 container1
	 * @param container2 container2
	 * @param keyindex1 keyindex1
	 * @param keyindex2 keyindex2
	 * @param compressed1 compressed1
	 * @param compressed2 compressed2
	 */
	void mergeIndex(
			const char * container1,
			const char * container2,
			const char * tmpcontainer,
			const char * keyindex1,
			const char * keyindex2,
			const char * tmpkeyindex,
			const char * compressed1,
			const char * compressed2,
			const char * tmpcompressed);
} // end namespace bible.
#endif
