/**
 * (C) Copyright 2014.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 3.0 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 *     whentp <tpsmsproject@gmail.com>
 */

#ifndef _SCHEDULE_H__
#define _SCHEDULE_H__
#include <string>
#include <vector>
#include "search.h"

namespace bible{

	struct ScheduleFileNode{
		string filename;
		size_t filesize;
	};

	using namespace std;
	class Schedule{
		private:
			string _directory;
			string _tokenizer_name;
			vector<Searcher*> *_searchers;
		public:
			Schedule(const string directoryname, const string tokenizer_name);
			~Schedule();

			/**
			 * @brief Add a file to index
			 * @details the length of the given filename should be less than 60 bytes.
			 * 
			 * @param[in] filename filename
			 */
			void AddFile(const char *filename);

			/**
			 * @brief Add a text to index
			 * @details the length of the given filename should be less than 60 bytes.
			 * 
			 * @param[in] key a key string
			 * @param[in] value a value string
			 */
			void AddText(const char *key, const char *value);

			/**
			 * @brief Commit all changes. Then the added files can be searched.
			 */
			void Commit();

			/**
			 * @brief Should be called before adding files to index.
			 * @details This method ensures no "0" exists in filenames.
			 */
			void Start();

			/**
			 * @brief Merge indexes using tp's idea.
			 */
			void Merge();

			/**
			 * @brief Find all container files. Then strip their extensions.
			 * @return Schedule
			 */
			vector<ScheduleFileNode> *FindIndexFiles();
			
			/**
			 * @brief Find all container files for merge. That is, when a container contains more 
			 * than MAX files, it will be deleted from the results.Then strip their extensions.
			 * @return Schedule
			 */
			//vector<ScheduleFileNode> *FindIndexFilesForMerge();

			/**
			 * @brief Merge two group of indexes.
			 * 
			 * @param[in] a filename a
			 * @param[in] b filename b
			 */
			void MergePair(
					const string &a,
					const string &b);

			/**
			 * @brief List all index groups. Then create searchers. Must be called before searching.
			 * @details [long description]
			 */
			void PrepareSearchers();

			/**
			 * @brief Search index groups then merge the results.
			 * 
			 * @param keyword_str The keywords.
			 * @return The search result. Filenames are already attached.
			 */
			SearchResult *Search(const char *keyword_str);
	};

	/**
	 * @brief Generate a string containing n "1". Only "1".
	 * 
	 * @param[in] size The repeating times.
	 * @return The generated string
	 */
	string repeatChar(size_t size);

	vector<ScheduleFileNode> *filterFilenameAll(
			vector<string> *filenames);
} // end namespace bible.

#endif
