#include <iostream>
#include <vector>
#include <string>
#include <ctype.h>
#include "common.h"
#include "tokenizer_singleton.h"
#include "structure.h"
#include "hash.h"

namespace bible {
using namespace std;


TokenizerSingleton::TokenizerSingleton(
    size_t hashcode_number,
    bool ignorecase) {
    _hashcode_number = (hashcode_number % 3) + 1; // should be 1, 2, 3.
    _ignorecase = ignorecase;
}

vector<TokenItem> *TokenizerSingleton::Tokenize(const string &original) {
    vector<TokenItem> *result = new vector<TokenItem>();
    size_t len = original.size();
    const char *c_str = original.c_str();

    string data;
    if (_ignorecase) {
        data = toLowercase(original);
        c_str = data.c_str();
    }

    for (size_t i = 0; i < _hashcode_number; ++i) {
        TokenItem new_token;
        new_token.offset = i;
        new_token.hash = hashMem(c_str, len, i + 1);
        result->push_back(new_token);
    }
    return result;
}

string TokenizerSingleton::GetConfiguration() {
    return "Singleton Tokenizer.";
}

} // end namespace bible.
