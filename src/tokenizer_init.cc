#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "tokenizer.h"
#include "structure.h"
#include "tokenizer_init.h"
#include "tokenizer_fourbytes.h"
#include "tokenizer_english.h"
#include "hash.h"
#include "common.h"

namespace bible {

using namespace std;

map<string, TokenizerFunc> globalTokenizers;

//call english tokenizer
vector<TokenItem> *tokenizer_english_func(const string &str) {
    bool ignorecase = false;
    ignorecase = globalConfigs.Read("tokenizerenglish.ignorecase", ignorecase);

    TokenizerEnglish *tokenizer = new TokenizerEnglish(ignorecase);
    vector<TokenItem> *result = tokenizer->Tokenize(string(str));
    delete tokenizer;
    return result;
}

//call fourbytes tokenizer
vector<TokenItem> *tokenizer_fourbytes_func(const string &str) {
    size_t blocksize = 4;
    blocksize = globalConfigs.Read("tokenizerfourbytes.blocksize", blocksize);
    
    bool ignorecase = false;
    ignorecase = globalConfigs.Read("tokenizerfourbytes.ignorecase", ignorecase);
    
    TokenizerFourBytes *tokenizer = new TokenizerFourBytes(blocksize, ignorecase);
    vector<TokenItem> *result = tokenizer->Tokenize(string(str));
    delete tokenizer;
    return result;
}

void initTokenizer() {
    // add english tokenizer.
    prepareCryptTable();//required by english tokenizer
    globalTokenizers["english"] = tokenizer_english_func;

    // add fourbytes tokenizer.
    globalTokenizers["fourbytes"] = tokenizer_fourbytes_func;
}

// try to call initTokenizer when loading.
InitTokenizer ___inittokenizeruseless;

} // end namespace bible.
