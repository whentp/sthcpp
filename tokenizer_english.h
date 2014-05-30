#ifndef TOKENIZER_ENGLISH_H
#define TOKENIZER_ENGLISH_H

#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.h"

using namespace std;

class TokenizerEnglish: Tokenizer {
	public:
		vector<TokenItem> * Tokenize(const string& original);
};

#endif
