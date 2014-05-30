#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct TokenItem;

class Tokenizer {		
	public:
		virtual vector<TokenItem> * Tokenize(const string& original);
};

#endif
