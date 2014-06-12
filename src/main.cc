#include <iostream>
#include <string>
#include <map>
#include "indexer.h"
#include "search.h"
#include "hash.h"
#include "common.h"
#include "schedule.h"
#include "file_op.h"

using namespace std;
using namespace bible;
typedef void(*pointer_of_function)(int, char const *[]);

void showUsage(){
	char usagestr[] = "Usage:\n"
		"main_executable_program add|merge|search\n\n"
		"create: index new files to a new index. The length of each filename should be less than 60 bytes (including path). The size of each file should be less than 60k.\n"
		//"merge: merge two indexes to one temporary index.\n"
		"search: search an index.\n";

	cout<<usagestr<<endl;
}

void actionCreateIndex(int argc, char const *argv[]){
	string filename;
	Schedule *indexer = new Schedule("index_files/");
	indexer->Start();
	while(cin>>filename) {
		indexer->AddFile(filename.c_str());
	}
	indexer->Commit();
	delete indexer;
}

void actionMergeIndexes(int argc, char const *argv[]){
	if (argc < 10){
		cout << "usage: merge container1 container2 to_container keyindex1 keyindex2 to_keyindex compressed1 compressed2 to_compressed" <<endl;
		return;
	}
	mergeIndex(
			argv[1], argv[2], argv[3], 
			argv[4], argv[5], argv[6], 
			argv[7], argv[8], argv[9]);

	cout<<"merge ok."<<endl;
}

void actionSearch(int argc, char const *argv[]){
	char str[256];

	cin.get(str, 256);
	cout<<"start searching..."<<endl;
	StopWatch watch;

	string directory = "index_files/";
	auto searcher = new Schedule(directory);
	searcher->PrepareSearchers();
	auto res = searcher->Search(str);

	res->elapsetime = watch.Stop();

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
	string filename;
	Schedule *aaa = new Schedule("index_files/");
	//int x= 2;
	aaa->Start();
	while(cin>>filename) {
		aaa->AddFile(filename.c_str());//, "raw.index", "container.index");
		//if ((x++%5) == 0)
	}
	aaa->Commit();
	delete aaa;
	cout << "end testing." << endl;

}

map<string, pointer_of_function> actions = {
	{"add",	actionCreateIndex},
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
