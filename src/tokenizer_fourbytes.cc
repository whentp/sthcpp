#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "tokenizer_fourbytes.h"
#include "structure.h"
#include "config.h"

namespace bible {
using namespace std;

/**
 * four bytes as an integer. simple. if len is less than 4, then fill blanks with \0.
 * return a list of {offset of word, hash of word}.
 * */
vector<TokenItem> *TokenizerFourBytes::Tokenize(const string &original) {
    vector<TokenItem> *result = new vector<TokenItem>();
    size_t pos = 0;

    size_t blocksize = 4;
    blocksize = globalConfigs.Read("tokenizerfourbytes.blocksize", blocksize);
    if (blocksize > 4) {
        blocksize = 4;
    }

    size_t len = original.size() - blocksize + 1;
    const char *c_str = original.c_str();
    while (pos < len) {
        TokenItem new_token;
        new_token.offset = pos;
        new_token.hash = 0;
        memcpy((char *)(&(new_token.hash)), c_str + pos, blocksize); // no need to deal with blocks less than blocksize bytes.
        result->push_back(new_token);
        ++pos;
    }
    return result;
}

string TokenizerFourBytes::GetConfiguration() {
    return "FourBytes Tokenizer.";
}

} // end namespace bible.
