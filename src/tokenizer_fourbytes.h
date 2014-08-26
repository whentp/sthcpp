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
private:
    size_t _blocksize = 0;
    bool _ignorecase = false;
public:
    TokenizerFourBytes(
    	size_t blocksize = 4,
    	bool ignorecase = false);
    ~TokenizerFourBytes(){};
    vector<TokenItem> *Tokenize(const string &original);
    string GetConfiguration();
    void RestoreFromConfigurationString(const string& configstr){};
};

} // end namespace bible.

#endif
