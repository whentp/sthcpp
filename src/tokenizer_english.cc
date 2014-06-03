#include <iostream>
#include <vector>
#include <string>
#include <ctype.h>
#include "common.h"
#include "hash.h"
#include "tokenizer_english.h"

using namespace std;

/**
 * return a list of {offset of word, hash of word}.
 * */
vector<TokenItem> * TokenizerEnglish::Tokenize(const string& original){
	auto result = new vector<TokenItem>();
	auto len = original.size();
	auto c_str = original.c_str();
	size_t pos = 0;
	size_t prev_pos = 0;
	size_t offset = 0; // the indexes of "words".

	while (pos < len){
		while (pos < len && isalnum(c_str[pos])) pos++; // isalnum in ctype.h checks if a char is [\d\w]. 

		if(prev_pos < pos){
			// generate hashcode.
			debug_print("hashcode of %s: %d\n", original.substr(prev_pos, pos-prev_pos).c_str(), hashMem(c_str + prev_pos, pos - prev_pos, 1));
			TokenItem new_token;
			new_token.offset = offset; 
			new_token.hash = hashMem(c_str + prev_pos, pos - prev_pos, 1);
			result->push_back(new_token);
			++offset;
		}
		prev_pos = ++pos;
	}
	return result;
}
