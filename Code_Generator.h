// Project: CS 460 Fall 2020 Project 3
// File: Code_Generator.h
// Author: Dr. Watts
// Desciption: Code Generator class for Scheme to C++ translation

#ifndef CG_H
#define CG_H

#include <iostream>
#include <fstream>
#include "Lexical_Analyzer.h"

using namespace std;

class Code_Generator 
{
    public:
	Code_Generator (string filename, Lexical_Analyzer * L);
	~Code_Generator ();
	void WriteCode (int tabs, string code);
    private:
	Lexical_Analyzer * lex;
	ofstream cpp;
};
	
#endif
