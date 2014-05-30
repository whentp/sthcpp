#include <iostream>
#include <string>
#include "tokenizer.h"
#include "tokenizer_english.h"

using namespace std;

int main(){
	string a = "hello, world!!!";
	auto t = new TokenizerEnglish();
	t->Tokenize(a);
	return 0;
}
