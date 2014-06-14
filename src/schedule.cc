#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "structure.h"
#include "schedule.h"
#include "hash.h"
#include "indexer.h"
#include "search.h"
#include "file_op.h"

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
		prepareCryptTable();
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
			string keyindex_str = tmpstr + ".keyindex";
			string compressed_str = tmpstr + ".compressedindex";

			Searcher *tmpsearcher = new Searcher(
					container_str.c_str(),
					keyindex_str.c_str(),
					compressed_str.c_str());
			_searchers->push_back(tmpsearcher);
		}
		delete files;
	}

	SearchResult *Schedule::Search(const char *keyword_str){
		vector<SearchResult*> *results = new vector<SearchResult*>();
		size_t count = 0;
		for(size_t i = 0; i < _searchers->size(); ++i){
			auto result_piece = _searchers->at(i)->Search(keyword_str);
			_searchers->at(i)->MatchFilenames(result_piece);
			results->push_back(result_piece);
			count += result_piece->resultcount;
			//cout << count << endl;
		}
		auto merged_res = new SearchResult();
		merged_res->result_index = new unsigned int[count];
		merged_res->filenames = new FileNode[count];
		auto tmp_index = merged_res->result_index;
		auto tmp_filenames = merged_res->filenames;

		for(auto &result_piece : *results){
			memcpy((void*)tmp_index,
					(void*)(result_piece->result_index),
					result_piece->resultcount * sizeof(unsigned int));
			tmp_index += result_piece->resultcount;

			memcpy((void*)tmp_filenames,
					(void*)(result_piece->filenames),
					result_piece->resultcount * sizeof(FileNode));
			tmp_filenames += result_piece->resultcount;
		}
		merged_res->resultcount = count;
		delete results;
		return merged_res;
	}

	void Schedule::AddFile(const char *filename){
		addFileToIndex(
				filename,
				(_directory + "tmp.raw").c_str(),
				(_directory + "0").c_str());
	}

	vector<ScheduleFileNode> *Schedule::FindIndexFiles(){
		vector<string> *filelist = getFilesInDirectory(_directory.c_str());
		vector<ScheduleFileNode> *container_files = filterFilenameAll(filelist);
		delete filelist;
		return container_files;
	}

	void Schedule::Start(){
		auto container_files = FindIndexFiles();;

		if(!container_files->size()) return;

		for(auto &tmpfile : *container_files){
			string tmp = tmpfile.filename;
			if( tmp.find("0") != string::npos){
				rename((_directory + tmp + ".container").c_str(), (_directory + repeatChar(tmp.size()) + ".container").c_str());
				rename((_directory + tmp + ".barnvalue").c_str(), (_directory + repeatChar(tmp.size()) + ".barnvalue").c_str());				
				
				rename((_directory + tmp + ".keyindex").c_str(), (_directory + repeatChar(tmp.size()) + ".keyindex").c_str());
				rename((_directory + tmp + ".compressedindex").c_str(), (_directory + repeatChar(tmp.size()) + ".compressedindex").c_str());
			}
		}
		delete container_files;
	}

	void Schedule::Commit(){
		sortIndex((_directory + "tmp.raw").c_str());
		compressIndex(
				(_directory + "tmp.raw").c_str(),
				(_directory + "0.compressedindex").c_str(),
				(_directory + "0.keyindex").c_str());

		Merge();
	}

	void Schedule::Merge(){
		auto container_files = FindIndexFiles();
		if(container_files->size() < 2) return;

		string a = container_files->at(0).filename;
		for(size_t i = 1; i < container_files->size(); ++i){
			string b = container_files->at(i).filename;
			//cout << "a=" << a << ", \tb=" << b << endl;
			if(a.size() != b.size()){
				break;
			}
			MergePair(_directory + a, _directory + b);

			rename((_directory + "tmp_container.container").c_str(), (_directory + b + "0.container").c_str());
			rename((_directory + "tmp_container.barnvalue").c_str(), (_directory + b + "0.barnvalue").c_str());
			rename((_directory + "tmp_key").c_str(), (_directory + b + "0.keyindex").c_str());
			rename((_directory + "tmp_compressed").c_str(), (_directory + b + "0.compressedindex").c_str());

			//remove((_directory + a + ".container").c_str());
			remove((_directory + a + ".keyindex").c_str());
			remove((_directory + a + ".compressedindex").c_str());

			remove((_directory + b + ".container").c_str());
			remove((_directory + b + ".barnvalue").c_str());
			remove((_directory + b + ".keyindex").c_str());
			remove((_directory + b + ".compressedindex").c_str());

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
				(a+".keyindex").c_str(),
				(b+".keyindex").c_str(),
				(_directory + "tmp_key").c_str(),
				(a+".compressedindex").c_str(),
				(b+".compressedindex").c_str(),
				(_directory + "tmp_compressed").c_str());
		//cout<<"merge ok."<<endl;
	}

	string repeatChar(size_t size){
		string res = "";
		for(size_t i = 0; i < size; ++i){
			res += "1";
		}
		return res;
	}

	vector<ScheduleFileNode> *filterFilenameAll(vector<string> *filenames){
		auto res = new vector<ScheduleFileNode>();
		size_t pos;
		for(auto &i : *filenames){
			//cout<<"\t"<<i<<endl;
			if((pos = i.find(".container")) != string::npos){
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
