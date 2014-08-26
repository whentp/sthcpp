#ifndef TOKENIZER_SINGLETON_H
#define TOKENIZER_SINGLETON_H

#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.h"
#include "structure.h"

namespace bible {

using namespace std;

/**
 * @brief hash the whole string using the hash function in hash.h.
 */
class TokenizerSingleton: Tokenizer {
private:
    size_t _hashcode_number = 0;
    bool _ignorecase = false;
public:
    TokenizerSingleton(
    	size_t hashcode_number = 3, ///< specify the number of hash codes.
    	bool ignorecase = false);
    ~TokenizerSingleton(){};
    vector<TokenItem> *Tokenize(const string &original);
    string GetConfiguration();
};

} // end namespace bible.

#endif
