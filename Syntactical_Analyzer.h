/*******************************************************************************
* File name: Syntactical_Analyzer.h                                            *
* Project: CS 460 Project 3 Fall 2021                                          *
* Author: Dr. Watts                                                            *
* Modified by: Max Lankford                                                    *
* Date: 12/14/2021                                                             *
* Description: This file contains the header file for Syntactical Analyzer     *
*******************************************************************************/

#ifndef SYNTACTICALANALYZER_H
#define SYNTACTICALANALYZER_H

#include <iostream>
#include <fstream>
#include "Lexical_Analyzer.h"
#include "Code_Generator.h"

using namespace std;

class Syntactical_Analyzer 
{
    public:
	Syntactical_Analyzer (char * filename);
	~Syntactical_Analyzer ();
    private:
	Lexical_Analyzer * lex;
	Code_Generator * cg;
	token_type token;
	ofstream p2file;
	int program ();
	int more_defines ();
	int define ();
	int stmt_list ();
	int stmt ();
	int literal ();
	int quoted_lit ();
	int more_tokens ();
	int param_list ();
	int else_part ();
	int stmt_pair ();
	int stmt_pair_body ();
	int assign_pair ();
	int more_assigns ();
	int action ();
	int any_other_token ();
	bool space = false;
	bool isReturn = false;
	bool isCond = false;
	bool isRetVal = false;
	bool isDisplay = false;
};
	
#endif
