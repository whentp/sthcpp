#include <iostream>
#include <vector>
#include <string>
#include <ctype.h>
#include "common.h"
#include "hash.h"
#include "tokenizer_english.h"

using namespace std;
/**
 * A TokenItem contains only the hashcode instead of a real string itself.
 * */
struct TokenItem{
	size_t offset;
	int hash; ///< hashcode here.
};


vector<TokenItem> * TokenizerEnglish::Tokenize(const string& original){
	auto result = new vector<TokenItem>();
	auto len = original.size();
	auto c_str = original.c_str();
	size_t pos = 0;
	size_t prev_pos = 0;

	while (pos < len){
		while (pos < len && isalnum(c_str[pos])) pos++; // isalnum in ctype.h checks if a char is [\d\w]. 

		if(prev_pos < pos){
			// generate hashcode.
			printf("%d\n", hashMem(c_str + prev_pos, pos - prev_pos, 0));

		}
		prev_pos = ++pos;
	}

	return result;
}
