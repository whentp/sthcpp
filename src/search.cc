#include <stdio.h>
#include <iostream>
#include <fstream>
#include "file_op.h"
#include "hash.h"
#include "structure.h"
#include "tokenizer.h"
#include "tokenizer_english.h"
#include "search.h"
#include "keyword_tree.h"
#include "common.h"
namespace bible{

	KeyIndex::KeyIndex(const char *fkeyindex){
		keyindexf.open(fkeyindex, ios::in|ios::binary);
		if (!keyindexf.is_open()) {
			cout << "Unable to open file";
			exit(0);
		}
		keynodelength = getFileLength(fkeyindex)/sizeof(KeyNode);
	}

	KeyIndex::~KeyIndex(){
		keyindexf.close();
	}

	CompareNode KeyIndex::Find(unsigned int key){
		int l, m, r; // left, middle, right.
		KeyNode tmpk;
		CompareNode returnvalue;

		returnvalue.n2 = 0;
		returnvalue.n1 = 0;

		l = 1;
		r = keynodelength - 1;
		m = (l + r) / 2;

		while(l <= m && m <= r) {
			keyindexf.seekg(m * sizeof(KeyNode), ios::beg);
			keyindexf.read((char*)(&tmpk), sizeof(KeyNode));
			if(tmpk.key == key) {
				returnvalue.n1 = tmpk.start;
				returnvalue.n2 = tmpk.length;
				break;
			}
			else if(tmpk.key > key) {
				r = m - 1; m = (l + r)/2;
			}
			else if(tmpk.key < key) {
				l = m + 1; m=(l + r)/2;
			}
		}
		return returnvalue;
	}

	Searcher::Searcher(const char* fcontainerstr,
			const char* fkeyindexstr,
			const char* fcompressedstr){
		_fcontainer = fcontainerstr;
		_fkeyindex = fkeyindexstr;
		_fcompressed = fcompressedstr;
		_keyindex = new KeyIndex(_fkeyindex);
	}

	Searcher::~Searcher(){
		delete _keyindex;
	}

	SearchResult *Searcher::Search(const char *keyword_str){
		return searchMultipleKeywords(keyword_str, _fkeyindex, _fcompressed);
	}

	void Searcher::MatchFilenames(SearchResult* res){
		matchFilenamesForResults(res, _fcontainer);
	}

	MemBlock* getMemBlock(CompareNode node, const char* fcompressindex) {
		if(node.n2 <= 0) {
			return new MemBlock();
		}

		ifstream indexfile(fcompressindex, ios::in|ios::binary);

		auto res = new MemBlock(node.n2);
		if (indexfile.is_open()) {
			indexfile.seekg(node.n1, ios::beg);
			indexfile.read(res->block, node.n2);
			indexfile.close();
		}
		else{
			cout << "Unable to open file";
			exit(0);
		}
		return res;
	}

	void makeNext(MemBlock* &m1) {
		unsigned int* tmp = (unsigned int*)(m1->block);
		int length = m1->length/sizeof(int);
		for(int i=0; i<length; ++i){
			//printf("%x -> %x\n", tmp[i], tmp[i]+1);
			++(*(tmp++));
		}
	}

	int compareBlock(MemBlock* m1, MemBlock* m2) {
		unsigned int* a = (unsigned int*)(m1->block);
		unsigned int* b = (unsigned int*)(m2->block);
		int i = m1->length/sizeof(int), j = m2->length/sizeof(int);
		int count=0;
		unsigned int* tmp;
		tmp = (unsigned int*)m1->block;
		while(i>0 && j>0) {
			//debug_print("Comparing: %x, %x\n", *a, *b);
			if (*a < *b) {
				--i;
				++a;
			} else if(*a > *b) {
				--j;
				++b;
			} else { // deal with equal.
				++count;
				--i; --j;
				*tmp = *a;
				++a;
				++b;
				++tmp;
			}
		}
		m1->length = count * sizeof(int);
		return count;
	}

	void matchFilenamesForResults(SearchResult* res, const char* fcontainer) {
		int length = res->resultcount;
		fstream filenameindex(fcontainer, ios::in|ios::binary);
		unsigned int *tmp;
		unsigned int filenumber;
		tmp = res->result_index;
		res->filenames = new FileNode[length];
		for(int i = 0; i < length; ++i) {
			filenumber = getfilenumber(tmp[i]);
			filenameindex.seekg(filenumber * sizeof(FileNode));
			filenameindex.read((char*)&(res->filenames[i]), sizeof(FileNode));
		}
		filenameindex.close();
	}

	vector<TokenItem> * parseKeywords(const char* str) {
		auto tokenizer = new TokenizerEnglish();
		auto result = tokenizer->Tokenize(string(str));
		return result;
	}

	SearchResult *searchMultipleKeywords( 
			const char* keyword,
			const char* fkeyindex,
			const char* fcompressed)
	{
		string keywordstring(keyword);
		cout<<"parse tree begin."<<endl;
		KeywordTree *kt = parseKeywordTree(keywordstring);
		cout<<"parse tree end."<<endl;
		prepareCryptTable();
		//initSearch();
		auto res = searchByKeywordTree(kt, fkeyindex, fcompressed);
		delete kt;
		return res;
	}

	SearchResult *searchByKeywordTree(
			KeywordTree *kt,
			const char* fkeyindex,
			const char* fcompressed)
	{	
		//cout<<"Keytree type:"<<kt->type<<endl;
		if(KT_STRING == kt->type){
			SearchResult *res = new SearchResult();
			searchSingleKeyword(res, kt->keyword.c_str(), fkeyindex, fcompressed);
			shrinkSearchSingleKeyword(res);
			return res;
		} else if(KT_AND == kt->type){
			SearchResult *res1 = searchByKeywordTree(kt->left, fkeyindex, fcompressed);
			if(res1->resultcount){
				SearchResult *res2 = searchByKeywordTree(kt->right, fkeyindex, fcompressed);
				if (res2->resultcount){
					mergeSearchSingleAnd(res1, res2);
				}
				delete res2;
			}
			return res1;
		} else if(KT_OR == kt->type){
			SearchResult *res1 = searchByKeywordTree(kt->left, fkeyindex, fcompressed);
			SearchResult *res2 = searchByKeywordTree(kt->right, fkeyindex, fcompressed);
			if(res1->resultcount > 0 && res2->resultcount == 0){
				delete res2;
				return res1;
			} else if(res1->resultcount == 0 && res2->resultcount > 0){
				delete res1;
				return res2;
			} else {
				mergeSearchSingleOr(res1, res2);
				delete res2;
				return res1;
			}
		} else if(KT_SUB == kt->type){
			SearchResult *res1 = searchByKeywordTree(kt->left, fkeyindex, fcompressed);
			SearchResult *res2 = searchByKeywordTree(kt->right, fkeyindex, fcompressed);
			if(res1->resultcount == 0 || res2->resultcount == 0){
				delete res2;
				return res1;
			} else {
				mergeSearchSingleSub(res1, res2);
				delete res2;
				return res1;
			}
		} else {
			return new SearchResult();
		}
	}

	void searchSingleKeyword(
			SearchResult* abc, 
			const char* keyword,
			const char* fkeyindex,
			const char* fcompressed)
	{
		CompareNode start_and_length;
		MemBlock *m1, *m2;
		StopWatch watch; // create a stopwatch.
		vector<TokenItem> *keywords = parseKeywords(keyword);
		int lsize = keywords->size();

		if (lsize > 0) {
			KeyIndex key_finder(fkeyindex);
			start_and_length = key_finder.Find(keywords->at(0).hash);
			if(start_and_length.n2) {
				m1 = getMemBlock(start_and_length, fcompressed);
				for(int i=1; i<lsize; ++i) {
					//debug_print("dealing with %d, size:%d...\n", keywords->at(i).hash, start_and_length.n2);
					start_and_length = key_finder.Find(keywords->at(i).hash);
					if (start_and_length.n2) {
						m2 = getMemBlock(start_and_length, fcompressed);
						makeNext(m1);
						size_t count = compareBlock(m1, m2);
						if (!count) {				
							m1->Free();
							m2->Free();
							break;
						}
						delete m2;
					}
				}
				abc->result_index = (unsigned int *)m1->block;
				m1->Lock();
				abc->resultcount = m1->length/sizeof(int);
				delete m1;
			}
		} else {
			abc->resultcount = 0;
			abc->result_index = NULL;
			//abc->result = NULL;
		}
		abc->elapsetime = watch.Stop();
		delete keywords;
		// before leaving, m1 & m1->block should be released properly. or leak here.
	}

	void shrinkSearchSingleKeyword(SearchResult* res) {
		int len = res->resultcount;
		if (len == 0) return;
		unsigned int *cmp = res->result_index, *cur = res->result_index;
		*cur = makeFileNode(getfilenumber(*cur), 0);
		while (len-- > 0){
			if (getfilenumber(*cmp) == getfilenumber(*cur)) {
				++(*cur); // yes this equals to makeFileNode(getfilenumber(*cur), getfileoffset(*cur) + 1);
			} else {
				++cur;
				*cur = makeFileNode(getfilenumber(*cmp), 1);
			}
			++cmp;
		}
		res->resultcount = cur - res->result_index + 1;
	}

	size_t mergeSearchSingleAnd(SearchResult* a, SearchResult* b){
		int i = a->resultcount;
		int j = b->resultcount;
		if(i == 0){
			return 0;
		}
		if(j == 0){
			delete[] a->result_index;
			a->result_index = NULL;
			a->resultcount = 0;
			return 0;
		}
		unsigned int *ap = a->result_index;
		unsigned int *tmp = a->result_index;
		unsigned int *bp = b->result_index;
		unsigned int tmpa, tmpb;
		size_t count = 0;
		while(i > 0 && j > 0){
			tmpa = getfilenumber(*ap);
			tmpb = getfilenumber(*bp);
			if (tmpa < tmpb){
				--i;
				++ap;
			} else if (tmpa > tmpb){
				--j;
				++bp;
			} else { // deal with equal. 
				++count;
				*tmp = makeFileNode(tmpa,1);
				++tmp;
				++ap;
				++bp;
				--i; --j;
			}
		}
		a->resultcount = count;
		return count;
	}

	size_t mergeSearchSingleOr(SearchResult* a, SearchResult* b){
		int len1 = a->resultcount;
		int len2 = b->resultcount;
		int i = 0, j = 0;
		unsigned int *ap = a->result_index;
		unsigned int *bp = b->result_index;
		unsigned int *tmp = new unsigned int[len1 + len2];
		unsigned int tmpa, tmpb;
		size_t count = 0;
		unsigned int *newresult = tmp;

		i = len1;
		j = len2;

		while(i > 0 && j > 0){
			tmpa = getfilenumber(*ap);
			tmpb = getfilenumber(*bp);
			if (tmpa < tmpb){
				*tmp = makeFileNode(tmpa, 1);
				++tmp;
				--i;
				++ap;
			} else if (tmpa > tmpb){
				//cout<<"xxx"<<endl;
				*tmp = makeFileNode(tmpb, 1);
				++tmp;
				--j;
				++bp;
			} else { // deal with equal.
				*tmp = makeFileNode(tmpa, 1);
				++tmp;
				++ap;
				++bp;
				--i; --j;
			}
			++count;
		}

		// deal with remaining items.
		while(i>0){
			*tmp = *ap;
			++tmp;
			--i;
			++ap;
			++count;
		}
		while(j>0){
			*tmp = *bp;
			++tmp;
			--j;
			++bp;
			++count;
		}

		a->resultcount = count;

		delete[] a->result_index;
		a->result_index = newresult;
		cout<<"end or merge."<<count<<endl;
		return count;
	}

	size_t mergeSearchSingleSub(SearchResult* a, SearchResult* b){
		int i = a->resultcount;
		int j = b->resultcount;
		if (j == 0){
			return i;
		}
		unsigned int *ap = a->result_index;
		unsigned int *bp = b->result_index;
		unsigned int *tmp = ap;
		unsigned int tmpa, tmpb;
		size_t count = 0;

		while(i > 0 && j > 0){
			tmpa = getfilenumber(*ap);
			tmpb = getfilenumber(*bp);
			if (tmpa < tmpb){
				*tmp = makeFileNode(tmpa, 1);
				++tmp;
				++count;
				--i;
				++ap;
			} else if (tmpa > tmpb){
				--j;
				++bp;
			} else { // deal with equal.
				++ap;
				++bp;
				--i; --j;
			}
		}

		// deal with remaining items.
		while(i>0){
			*tmp = *ap;
			++tmp;
			--i;
			++ap;
			++count;
		}

		a->resultcount = count;
		cout<<"end sub merge."<<count<<endl;
		return count;
	}
} // end namespace bible.
