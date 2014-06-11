#include <iostream>
#include <string>
#include <map>
#include "indexer.h"
#include "search.h"
#include "hash.h"
#include "common.h"

using namespace std;
using namespace bible;
typedef void(*pointer_of_function)(int, char const *[]);

void showUsage(){
	char usagestr[] = "Usage:\n"
		"main_executable_program create|merge|search\n\n"
		"create: add new files to a new index. each index contains a container, keyindex and compressed index.\n"
		"merge: merge two indexes to one temporary index.\n"
		"search: search an index.\n";

	cout<<usagestr<<endl;
}

void actionCreateIndex(int argc, char const *argv[]){
	string filename;
	//printf("Add files to index...\n");
	prepareCryptTable();

	while(cin>>filename) {
		addFileToIndex(filename.c_str(), "raw.index", "container.index");
	}

	sortIndex("raw.index");
	compressIndex("raw.index", "compress.index", "keyindex.index");
}

void actionMergeIndexes(int argc, char const *argv[]){
	if (argc < 7){
		cout << "usage: merge container1, container2, keyindex1, keyindex2, compressed1, compressed2" <<endl;
		return;
	}
	mergeIndex(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

	cout<<"merge ok."<<endl;
}

void actionSearch(int argc, char const *argv[]){
	char str[256];

	cin.get(str, 256);
	cout<<"start searching..."<<endl;
	StopWatch watch;

	auto searcher = new Searcher("container.index", "keyindex.index", "compress.index");
	auto res = searcher->Search(str);
	searcher->MatchFilenames(res);
	res->elapsetime = watch.Stop();

	// display results.
	for(int i = 0; i < res->resultcount; ++i){
		cout << res->filenames[i] 
			<< "\t->\t" << getfileoffset(res->result_index[i]) << " match(es)." << endl;
	}
	cout << "Total " << res->resultcount << " result(s). Cost " 
		<< res->elapsetime << " second(s)." << endl;

	delete res;
	delete searcher;
	cout<<"end."<<endl;
}

void actionSearchSingle(int argc, char const *argv[]){
	char str[256];

	cin.get(str, 256);
	cout<<"start searching..."<<endl;

	StopWatch watch;
	SearchResult *res = new SearchResult();
	searchSingleKeyword(res, str, "keyindex.index", "compress.index");
	shrinkSearchSingleKeyword(res);
	matchFilenamesForResults(res, "container.index");
	res->elapsetime = watch.Stop();
	for(int i = 0; i < res->resultcount; ++i){
		cout << res->filenames[i] 
			<< "\t->\t" << getfileoffset(res->result_index[i]) << " match(es)." << endl;
	}
	cout << "Total " << res->resultcount << " result(s). Cost " 
		<< res->elapsetime << " second(s)." << endl;

	delete res;
	cout<<"end."<<endl;
}



void actionTest(int argc, char const *argv[]){
	cout << "start testing..." << endl;
	///////////////// to do
	cout << "end testing." << endl;
}

map<string, pointer_of_function> actions = {
	{"create",	actionCreateIndex},
	{"merge",	actionMergeIndexes},
	{"search",	actionSearch},
	{"searchsingle",	actionSearchSingle},
	{"test",	actionTest}
};

int main(int argc, char const *argv[]) {
	if (argc < 2){
		showUsage();
	} else {
		string action_str = argv[1];
		if ( actions.find(action_str) == actions.end() ) {
			cout << "action \"" << action_str << "\" does not exist." << endl;
			showUsage();
		} else {
			// just skip the first argv.
			actions.at(action_str)(argc - 1, argv + 1);
		}
	}
	return 0; 
}
