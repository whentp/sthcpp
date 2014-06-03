#!/bin/sh

g++ -c -Wall -std=c++11 hash.c
g++ -c -Wall -std=c++11 tokenizer.cc
g++ -c -Wall -std=c++11 tokenizer_english.cc
#g++ -c -Wall -std=c++11 test_tokenizer_english.cc
g++ -c -Wall -std=c++11 file_op.cc
g++ -c -Wall -std=c++11 structure.c
g++ -c -Wall -std=c++11 indexer.cc

g++ -c -Wall -std=c++11 test_indexer.cc

g++ -o a.out test_indexer.o indexer.o structure.o file_op.o tokenizer_english.o hash.o