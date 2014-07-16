#ifndef TOKENIZER_INIT_H
#define TOKENIZER_INIT_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "tokenizer.h"
#include "structure.h"

namespace bible {

using namespace std;

/**
 * @brief the type of tokenizer functions.
 */
typedef vector<TokenItem> *(*TokenizerFunc)(const string &);

extern map<string, TokenizerFunc> globalTokenizers;

vector<TokenItem> *tokenizer_english_func(const string &str);
vector<TokenItem> *tokenizer_fourbytes_func(const string &str);

void initTokenizer();

} // end namespace bible.

#endif
