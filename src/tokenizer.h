#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include "structure.h"

namespace bible{
	using namespace std;

	/**
	 * A TokenItem contains only the hashcode instead of a real string itself.
	 * */
	struct TokenItem{
		BibleIntType offset;
		BibleIntType hash;
	};

	class Tokenizer {		
		public:
			virtual ~Tokenizer(){};
			virtual vector<TokenItem> * Tokenize(const string& original)=0;
			virtual string GetConfiguration()=0;
	};
} // end namespace bible.
#endif
