#include <iostream>
#include <string>
#include "indexer.h"
#include "search.h"
#include "hash.h"


using namespace std;

/*int main(int argc, char const *argv[]) {
	string filename;
	//printf("Add files to index...\n");
	prepareCryptTable();
	
	while(cin>>filename) {
		addFileToIndex(filename.c_str(), "raw.index");
	}

	sortIndex("raw.index");
	compressIndex("raw.index", "compress.index", "keyindex.index");
	return 0; 
}*/


int main(int argc, char const *argv[]) {
	char str[256];
	SearchResult ddd;
	prepareCryptTable();
	initSearch();
	cin.get(str, 256);
	cout<<"start searching."<<endl;
	search(&ddd, str);
	showCompleteResult(&ddd);
	cout<<"end."<<endl;
	return 0;
}


