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

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "structure.h"
#include "schedule.h"
#include "hash.h"
#include "indexer.h"
#include "search.h"
#include "file_op.h"
#include "keyword_tree.h"
#include "exceptions.h"
#include "barn.h"

namespace bible{

	using namespace std;

	Schedule::Schedule(const string directoryname): _searchers(NULL){
		_directory = directoryname;
		if (_directory.size()){
			if(_directory.at(_directory.size() - 1) != '/' &&
					_directory.at(_directory.size() - 1) != '\\'){
				_directory += "/";
			}
		} else {
			_directory = "./";
		}
		Start();
	}

	Schedule::~Schedule(){
		if(_searchers){
			delete _searchers;
			_searchers = NULL;
		}
	}

	void Schedule::PrepareSearchers(){
		if(_searchers){
			delete _searchers;
			_searchers = NULL;
		}

		_searchers = new vector<Searcher*>;

		auto files = FindIndexFiles();
		for(size_t i = 0; i < files->size(); ++i){
			string tmpstr = _directory + files->at(i).filename;
			string container_str = tmpstr; // + ".container"; 
			string keyindex_str = tmpstr + file_ext_keyindex;
			string compressed_str = tmpstr + file_ext_compressedindex;

			Searcher *tmpsearcher = new Searcher(
					container_str.c_str(),
					keyindex_str.c_str(),
					compressed_str.c_str());
			_searchers->push_back(tmpsearcher);
		}
		delete files;
	}

	SearchResult *Schedule::Search(const char *keyword_str){
		size_t count = 0;
		SearchResult *merged_res = new SearchResult();
		vector<SearchResult*> *results = new vector<SearchResult*>();

		try{
			string keywordstring(keyword_str);
			KeywordTree *kt = parseKeywordTree(keywordstring);

			for(size_t i = 0; i < _searchers->size(); ++i){
				SearchResult *result_piece = _searchers->at(i)->SearchByKeywordTree(kt);
				_searchers->at(i)->MatchFilenames(result_piece);
				results->push_back(result_piece);
				count += result_piece->count;
			}
			delete kt;
			merged_res->indexes = new BibleIntType[count];
			merged_res->filenames = new FileNode[count];
			auto tmp_index = merged_res->indexes;
			auto tmp_filenames = merged_res->filenames;

			for(auto &result_piece : *results){
				memcpy((void*)tmp_index,
						(void*)(result_piece->indexes),
						result_piece->count * sizeof(BibleIntType));
				tmp_index += result_piece->count;

				memcpy((void*)tmp_filenames,
						(void*)(result_piece->filenames),
						result_piece->count * sizeof(FileNode));
				tmp_filenames += result_piece->count;
			}
			merged_res->state = 1; // 1 stands for sucessful.
			merged_res->count = count;
		} catch (ExceptionBible& e) {
			merged_res->count = 0;
			merged_res->state = 0; // means failure.
			merged_res->SetMsg(e.what());
		}
		delete results;
		return merged_res;
	}

	void Schedule::AddFile(const char *filename){
		addFileToIndex(
				filename,
				(_directory + "tmp.raw").c_str(),
				(_directory + "0").c_str());
	}

	void Schedule::AddText(const char *key, const char *value){
		addTextToIndex(
				key, value,
				(_directory + "tmp.raw").c_str(),
				(_directory + "0").c_str());
	}

	vector<ScheduleFileNode> *Schedule::FindIndexFiles(){
		vector<string> *filelist = getFilesInDirectory(_directory.c_str());
		vector<ScheduleFileNode> *container_files = filterFilenameAll(filelist);
		delete filelist;
		return container_files;
	}

	/*vector<ScheduleFileNode> *Schedule::FindIndexFilesForMerge(){
		vector<string> *filelist = getFilesInDirectory(_directory.c_str());
		vector<ScheduleFileNode> *container_files = filterFilenameAll(filelist);
		for(size_t i = container_files->size() - 1; i >= 0; --i){
			if(container_files->at(i).filesize >= sizeof(BarnNode) * getfilenumber(MAX_BIBLE_INT_VALUE)){
				container_files->erase(&(container_files->at(i)));
			}
		}
		delete filelist;
		return container_files;
	}*/

	void Schedule::Start(){
		auto container_files = FindIndexFiles();;

		if(!container_files->size()) return;

		for(auto &tmpfile : *container_files){
			string tmp = tmpfile.filename;
			if(tmp.find("0") != string::npos){
				string repeated_tmpstr = repeatChar(tmp.size());
				// container
				rename((_directory + tmp + file_ext_container_key).c_str(),
						(_directory + repeated_tmpstr + file_ext_container_key).c_str());
				rename((_directory + tmp + file_ext_container_value).c_str(),
						(_directory + repeated_tmpstr + file_ext_container_value).c_str());				
				// keyindex
				rename((_directory + tmp + file_ext_keyindex).c_str(),
						(_directory + repeated_tmpstr + file_ext_keyindex).c_str());
				// compressedindex
				rename((_directory + tmp + file_ext_compressedindex).c_str(),
						(_directory + repeated_tmpstr + file_ext_compressedindex).c_str());
			}
		}
		delete container_files;
	}

	void Schedule::Commit(){
		sortIndex((_directory + "tmp.raw").c_str());
		compressIndex(
				(_directory + "tmp.raw").c_str(),
				(_directory + "0" + file_ext_compressedindex).c_str(),
				(_directory + "0" + file_ext_keyindex).c_str());
		Merge();
	}

	void Schedule::Merge(){
		auto container_files = FindIndexFiles();

		size_t cannot_merge_count = 0;
		for(size_t i = container_files->size(); i-- > 0;){ // attention to the loop. fuck size_t.
			bool flag = false;
			//actually only using the first char can get the result. 
			for(auto x = container_files->at(i).filename.begin(); x != container_files->at(i).filename.end(); ++x){
				if(*x == '0' || *x =='1'){
					flag = true;
					break;
				}
			}
			if(flag){
				++cannot_merge_count;
				//container_files->erase(container_files->begin() + i);
			}
		}

		if(container_files->size() < 2) return;

		string a = container_files->at(0).filename;
		for(size_t i = 1; i < container_files->size(); ++i){
			string b = container_files->at(i).filename;
			//cout << "a=" << a << ", \tb=" << b << endl;
			if(a.size() != b.size()){
				break;
			}

			// check the sizes of containers.		
			size_t filesize_a = getFileLength((_directory + a + file_ext_container_key).c_str()) / sizeof(BarnNode);
			size_t filesize_b = getFileLength((_directory + b + file_ext_container_key).c_str()) / sizeof(BarnNode);

			if(filesize_a + filesize_b > getfilenumber(MAX_BIBLE_INT_VALUE)){
				string file1 = a;
				string file2 = b;
				if(filesize_a > filesize_b){
					file1 = b;
					file2 = a;
				}

				string cannot_merge_name = file_fixed_container_prefix + size_t_to_string(cannot_merge_count);

				// rename file2 to cannot_merge_name_xxx 
				rename((_directory + file2 + file_ext_container_key).c_str(),
						(_directory + cannot_merge_name + file_ext_container_key).c_str());
				rename((_directory + file2 + file_ext_container_value).c_str(),
						(_directory + cannot_merge_name + file_ext_container_value).c_str());
				rename((_directory + file2 + file_ext_keyindex).c_str(),
						(_directory + cannot_merge_name + file_ext_keyindex).c_str());
				rename((_directory + file2 + file_ext_compressedindex).c_str(),
						(_directory + cannot_merge_name + file_ext_compressedindex).c_str());

				// rename file1 to file2.
				rename((_directory + file1 + file_ext_container_key).c_str(),
						(_directory + file2 + file_ext_container_key).c_str());
				rename((_directory + file1 + file_ext_container_value).c_str(),
						(_directory + file2 + file_ext_container_value).c_str());
				rename((_directory + file1 + file_ext_keyindex).c_str(),
						(_directory + file2 + file_ext_keyindex).c_str());
				rename((_directory + file1 + file_ext_compressedindex).c_str(),
						(_directory + file2 + file_ext_compressedindex).c_str());
				break;
			}

			MergePair(_directory + a, _directory + b);

			rename((_directory + "tmp_container" + file_ext_container_key).c_str(),
					(_directory + b + "0" + file_ext_container_key).c_str());
			rename((_directory + "tmp_container" + file_ext_container_value).c_str(),
					(_directory + b + "0" + file_ext_container_value).c_str());
			rename((_directory + "tmp_key").c_str(),
					(_directory + b + "0" + file_ext_keyindex).c_str());
			rename((_directory + "tmp_compressed").c_str(),
					(_directory + b + "0" + file_ext_compressedindex).c_str());

			//remove((_directory + a + ".container").c_str());
			remove((_directory + a + file_ext_keyindex).c_str());
			remove((_directory + a + file_ext_compressedindex).c_str());

			remove((_directory + b + file_ext_container_key).c_str());
			remove((_directory + b + file_ext_container_value).c_str());
			remove((_directory + b + file_ext_keyindex).c_str());
			remove((_directory + b + file_ext_compressedindex).c_str());

			a = b+"0";
		}

		delete container_files;

		cout<<"merge ok."<<endl;
	}

	void Schedule::MergePair(const string &a, const string &b){
		mergeIndex(
				//(a+".container").c_str(),
				//(b+".container").c_str(),
				//(_directory + "tmp_container").c_str(),
				a.c_str(), b.c_str(), (_directory + "tmp_container").c_str(),
				(a + file_ext_keyindex).c_str(),
				(b + file_ext_keyindex).c_str(),
				(_directory + "tmp_key").c_str(),
				(a + file_ext_compressedindex).c_str(),
				(b + file_ext_compressedindex).c_str(),
				(_directory + "tmp_compressed").c_str());
		//cout<<"merge ok."<<endl;
	}

	string repeatChar(size_t size){
		string res(size, '1');
		return res;
	}

	vector<ScheduleFileNode> *filterFilenameAll(vector<string> *filenames){
		auto res = new vector<ScheduleFileNode>();
		size_t pos;
		for(auto &i : *filenames){
			//cout<<"\t"<<i<<endl;
			if((pos = i.find(file_ext_container_key)) != string::npos){
				ScheduleFileNode snode;
				snode.filename = i.substr(0, pos);
				snode.filesize = getFileLength(i.c_str());
				res->push_back(snode);
			}
		}
		if(res->size()){
			sort(res->begin(), res->end(), 
					[](const ScheduleFileNode &a, const ScheduleFileNode &b) -> bool { 
					return a.filename < b.filename; 
					});
		}
		return res;
	}
} // end namespace bible.
