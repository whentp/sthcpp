#ifndef TOKENIZER_FOUR_BYTES_H
#define TOKENIZER_FOUR_BYTES_H

#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.h"
#include "structure.h"

namespace bible {

using namespace std;

class TokenizerFourBytes: Tokenizer {
public:
    vector<TokenItem> *Tokenize(const string &original);
    string GetConfiguration();
};

} // end namespace bible.

#endif
