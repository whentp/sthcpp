#!/bin/bash

g++ -c -Wall -std=c++0x hash.c
g++ -c -Wall -std=c++0x tokenizer.cc
g++ -c -Wall -std=c++0x tokenizer_english.cc
g++ -c -Wall -std=c++0x test_tokenizer_english.cc
g++ -o a.out test_tokenizer_english.o tokenizer_english.o hash.o
