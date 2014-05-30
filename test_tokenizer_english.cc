#include <iostream>
#include <string>
#include "tokenizer.h"
#include "tokenizer_english.h"

using namespace std;

int main(){
	string a = "hello, world!!!";
	auto t = new TokenizerEnglish();
	t->Tokenize(a);
	cout<<"======="<<endl;
	t->Tokenize("a a a a a a a    a   a  a   ");
	cout<<"======="<<endl;
	t->Tokenize("a a a a a a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa    a   a  a");
	cout<<"======="<<endl;
	t->Tokenize("   a a a a a a a    a   a  a   ");
	cout<<"======="<<endl;
	t->Tokenize("a b c d e f g");

	cout<<"end."<<endl;
	return 0;
}
