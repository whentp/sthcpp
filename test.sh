#!/usr/bin/sh

g++ -c -Wall -std=c++11 hash.c
g++ -c -Wall -std=c++11 tokenizer.cc
g++ -c -Wall -std=c++11 tokenizer_english.cc
g++ -Wall -std=c++11 test_tokenizer_english.cc
