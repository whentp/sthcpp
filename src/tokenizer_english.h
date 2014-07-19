#ifndef TOKENIZER_ENGLISH_H
#define TOKENIZER_ENGLISH_H

#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.h"
#include "structure.h"

namespace bible {

using namespace std;

class TokenizerEnglish: Tokenizer {
private:
    bool _ignorecase = false;
public:
    TokenizerEnglish(bool ignorecase)
        : _ignorecase(ignorecase) {};
    ~TokenizerEnglish() {};
    vector<TokenItem> *Tokenize(const string &original);
    string GetConfiguration();
};

} // end namespace bible.

#endif
