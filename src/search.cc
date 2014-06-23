#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "file_op.h"
#include "hash.h"
#include "structure.h"
#include "tokenizer.h"
#include "tokenizer_english.h"
#include "search.h"
#include "keyword_tree.h"
#include "common.h"
#include "container.h"

namespace bible{

	KeyIndex::KeyIndex(const char *fkeyindex){
		//cout << "open keyindex: " << fkeyindex << endl;
		keyindexf.open(fkeyindex, ios::in|ios::binary);
		if (!keyindexf.is_open()) {
			cout << "Unable to open keyindex file" << endl;
			exit(0);
		}
		keynodelength = getFileLength(fkeyindex)/sizeof(KeyNode);
	}

	KeyIndex::~KeyIndex(){
		keyindexf.close();
	}

	CompareNode KeyIndex::Find(BibleIntType key){
		size_t l, m, r; // left, middle, right.
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
		string tmp = "";
		_fcontainer = fcontainerstr;
		_fkeyindex = fkeyindexstr;
		_fcompressed = fcompressedstr;
		//cout << _fcontainer << endl;
		//cout << _fkeyindex << endl;
		//cout << _fcompressed << endl;
		_keyindex_finder = new KeyIndex(_fkeyindex.c_str());

		_indexfile.open(_fcompressed.c_str(), ios::in|ios::binary);

		if (!_indexfile.is_open()){
			cout << "Unable to open file: " << _fcompressed;
			exit(0);
		}
		prepareCryptTable();
	}

	Searcher::~Searcher(){
		//cout << "searcher destroyed." << endl;
		_indexfile.close();
		delete _keyindex_finder;
	}

	SearchResult *Searcher::Search(const char *keyword_str){
		//cout << "mmmmmmmmmmm" << endl;
		//cout << "containerindex in .search.: " << this->_fcontainer << endl;
		//cout << "keyindex in .search.: " << this->_fkeyindex << endl;
		//cout << "compressed in .search.: " << this->_fcompressed << endl;
		return SearchMultipleKeywords(keyword_str);
	}

	void Searcher::MatchFilenames(SearchResult* res){
		matchFilenamesForResults(res, _fcontainer.c_str());
	}

	MemBlock *Searcher::GetMemBlock(CompareNode &node) {
		if(node.n2 <= 0) {
			return new MemBlock();
		}

		MemBlock *res = new MemBlock(node.n2);
		_indexfile.seekg(node.n1, ios::beg);
		_indexfile.read(res->block, node.n2);
		return res;
	}

	void makeNext(MemBlock* &m1) {
		BibleIntType* tmp = (BibleIntType*)(m1->block);
		size_t length = m1->length/sizeof(BibleIntType);
		while(length > 0){
			++(*(tmp++));
			--length;
		}
	}

	size_t compareBlock(MemBlock* m1, MemBlock* m2) {
		BibleIntType *a = (BibleIntType*)(m1->block);
		BibleIntType *b = (BibleIntType*)(m2->block);
		size_t i = m1->length / sizeof(BibleIntType), j = m2->length / sizeof(BibleIntType);
		size_t count=0;
		BibleIntType *tmp;
		tmp = (BibleIntType*)m1->block;
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
		m1->length = count * sizeof(BibleIntType);
		return count;
	}

	void matchFilenamesForResults(SearchResult* res, const char* fcontainer) {
		size_t length = res->resultcount;
		//fstream filenameindex(fcontainer, ios::in|ios::binary);
		BibleIntType *tmp;
		BibleIntType filenumber;
		tmp = res->result_index;

		Container filenamefounder;
		filenamefounder.Open(fcontainer);

		res->filenames = new FileNode[length];
		for(size_t i = 0; i < length; ++i) {
			filenumber = getfilenumber(tmp[i]);
			//cout << filenumber << endl;
			res->filenames[i] = (FileNode)filenamefounder.GetFilename(filenumber);
		}
		filenamefounder.Close();
	}

	vector<TokenItem> * parseKeywords(const char* str) {
		auto tokenizer = new TokenizerEnglish();
		auto result = tokenizer->Tokenize(string(str));
		return result;
	}

	SearchResult *Searcher::SearchMultipleKeywords(const char* keyword){
		//cout << "keyindex in .searchMultipleKeywords.: " << fkeyindex << endl;
		string keywordstring(keyword);
		//cout<<"parse tree begin."<<endl;
		KeywordTree *kt = parseKeywordTree(keywordstring);
		//cout<<"parse tree end."<<endl;
		//initSearch();
		auto res = SearchByKeywordTree(kt);
		delete kt;
		return res;
	}

	SearchResult *Searcher::SearchByKeywordTree(const KeywordTree *kt){
		//cout << ">keyindex in ..: " << fkeyindex << endl;
		//cout<<"Keytree type:"<<kt->type<<endl;
		if(KT_STRING == kt->type){
			SearchResult *res = new SearchResult();
			SearchSingleKeyword(res, kt->keyword.c_str());
			shrinkSearchSingleKeyword(res);
			return res;
		} else if(KT_AND == kt->type){
			SearchResult *res1 = SearchByKeywordTree(kt->left);
			if(res1->resultcount){
				SearchResult *res2 = SearchByKeywordTree(kt->right);
				if (res2->resultcount){
					mergeSearchSingleAnd(res1, res2);
				}
				delete res2;
			}
			return res1;
		} else if(KT_OR == kt->type){
			SearchResult *res1 = SearchByKeywordTree(kt->left);
			SearchResult *res2 = SearchByKeywordTree(kt->right);
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
			SearchResult *res1 = SearchByKeywordTree(kt->left);
			SearchResult *res2 = SearchByKeywordTree(kt->right);
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

	void Searcher::SearchSingleKeyword(
		SearchResult* res, 
		const char* keyword)
	{
		CompareNode start_and_length;
		MemBlock *m1, *m2;
		//StopWatch watch; // create a stopwatch.
		vector<TokenItem> *keywords = parseKeywords(keyword);
		size_t lsize = keywords->size();

		if (lsize > 0) {
			//cout << "here...." << fkeyindex << fcompressed << endl;
			//KeyIndex key_finder(fkeyindex);
			start_and_length = _keyindex_finder->Find(keywords->at(0).hash);
			if(start_and_length.n2) {
				m1 = GetMemBlock(start_and_length);
				for(size_t i = 1; i < lsize; ++i) {
					//debug_print("dealing with %d, size:%d...\n", keywords->at(i).hash, start_and_length.n2);
					start_and_length = _keyindex_finder->Find(keywords->at(i).hash);
					if (start_and_length.n2) {
						m2 = GetMemBlock(start_and_length);
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
				res->result_index = (BibleIntType *)m1->block;
				m1->Lock();
				res->resultcount = m1->length/sizeof(BibleIntType);
				delete m1;
			}
		} else {
			res->resultcount = 0;
			res->result_index = NULL;
			//abc->result = NULL;
		}
		//res->elapsetime = watch.Stop();
		delete keywords;
		// before leaving, m1 & m1->block should be released properly. or leak here.
	}

	void shrinkSearchSingleKeyword(SearchResult* res) {
		size_t len = res->resultcount;
		if (len == 0) return;
		BibleIntType *cmp = res->result_index, *cur = res->result_index;
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
		size_t i = a->resultcount;
		size_t j = b->resultcount;
		if(i == 0){
			return 0;
		}
		if(j == 0){
			delete[] a->result_index;
			a->result_index = NULL;
			a->resultcount = 0;
			return 0;
		}
		BibleIntType *ap = a->result_index;
		BibleIntType *tmp = a->result_index;
		BibleIntType *bp = b->result_index;
		BibleIntType tmpa, tmpb;
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
		size_t len1 = a->resultcount;
		size_t len2 = b->resultcount;
		size_t i = 0, j = 0;
		BibleIntType *ap = a->result_index;
		BibleIntType *bp = b->result_index;
		BibleIntType *tmp = new BibleIntType[len1 + len2];
		BibleIntType tmpa, tmpb;
		size_t count = 0;
		BibleIntType *newresult = tmp;

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
		//cout<<"end or merge."<<count<<endl;
		return count;
	}

	size_t mergeSearchSingleSub(SearchResult* a, SearchResult* b){
		size_t i = a->resultcount;
		size_t j = b->resultcount;
		if (j == 0){
			return i;
		}
		BibleIntType *ap = a->result_index;
		BibleIntType *bp = b->result_index;
		BibleIntType *tmp = ap;
		BibleIntType tmpa, tmpb;
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
		//cout<<"end sub merge."<<count<<endl;
		return count;
	}
} // end namespace bible.
