/*******************************************************************************
* File name: Syntactical_Analyzer.cpp                                          *
* Project: CS 460 Project 3 Fall 2021                                          *
* Author: Dr. Watts                                                            *
* Modified by: Max Lankford                                                    *
* Date: 12/14/2021                                                             *
* Description: This file contains the functions for my Synbatactical Analyzer  *
* and the code for writing to files and generating C++ code                    *
*******************************************************************************/

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include "Syntactical_Analyzer.h"

using namespace std;

Syntactical_Analyzer::Syntactical_Analyzer (char * filename)
{
	lex = new Lexical_Analyzer (filename);
	cg = new Code_Generator (filename, lex);
        string name = filename;
        string p2name = name.substr (0, name.length()-3) + ".p2";
        p2file.open (p2name.c_str());
	token = lex->Get_Token();
	int totalErrors = program ();
}

Syntactical_Analyzer::~Syntactical_Analyzer ()
{
	delete cg;
	delete lex;
        p2file.close ();
}

static string token_lexemes[] = {
	"{}", "identifier", "numeric literal", "string literal", "if", "cond", "display",
	"newline", "list operation", "cons or append" "and", "or", "not", "define",
	"let", "number?", "list?", "zero?", "null?", "string?", "modulo", "round",
	"else", "+", "-", "/", "*", "=", ">", "<", ">=", "<=", "(", ")", "'", "error",
	"end of file", "end of file", "end of file", "end of file", "end of file"
}; 

int Syntactical_Analyzer::program ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T)
	{ // Rule 1
		 p2file << "Using Rule 1\n";
		 token = lex->Get_Token ();
		 errors += define (); //Call define if token is LPAREN_T
		 if (token == LPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[LPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += more_defines (); //Call more_defines if next token isnt also an LPAREN_T
		 if (token == EOF_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[EOF_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::more_defines ()
{
	int errors = 0;

	string message;
	if (token == IDENT_T)
	{ // Rule 3
		 p2file << "Using Rule 3\n";
		 token = lex->Get_Token ();
		 errors += stmt_list (); //Call stmt_list if token is an IDENT_T
		 if (token == RPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else if (token == DEFINE_T)
	{ // Rule 2
		 p2file << "Using Rule 2\n";
		 errors += define (); //Call define if token is DEFINE_T
		 if (token == LPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[LPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += more_defines (); //Call more_defines if next token isnt LPAREN_T
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::define ()
{
	int errors = 0;
	bool isMain = false;

	string message;
	if (token == DEFINE_T)
	{ // Rule 4
		 p2file << "Using Rule 4\n";
		 token = lex->Get_Token ();
		 if (token == LPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[LPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 if (token == IDENT_T)
		 {
		     if (lex->Get_Lexeme() == "main"){ //Check if current lexeme is "main"
		         isMain = true; //If it is "main" set bool to true
		     }
		     if (isMain){ //If bool is true...
		         cg->WriteCode(0, "int main ("); //Write corresponding code in C++
		     }
		     else {
		         cg->WriteCode(0, "Object " + lex->Get_Lexeme() + "("); //If not true set function declaration as type Object
		     }
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[IDENT_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += param_list (); //Call param_list if token isnt IDENT_T or LPAREN_T
		 if (token == RPAREN_T)
		 {
		     cg->WriteCode(0, "){\n"); //Write corresponding ){ for function declaration
             cg->WriteCode(1, "Object __RetVal;\n"); //Create new object variable
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ";\n"); //Write corresponding semi colon
		 errors += stmt_list (); //Call stmt_list
		 if (token == RPAREN_T)
		 {
		     if (isMain){ //If you are in main...
		         cg->WriteCode(1, "return 0;\n}\n"); //Write return value
		     }
		     else {
		         isReturn = true;
		         cg->WriteCode(1, "return __RetVal;\n}\n"); //Write return value
		     }
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::stmt_list ()
{
	int errors = 0;

	string message;
	if (token == IDENT_T || token == LPAREN_T)
	{ // Rule 5
		 p2file << "Using Rule 5\n";
		 errors += stmt (); //Call stmt
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == RPAREN_T)
	{ // Rule 6
		 p2file << "Using Rule 6\n";
		 ;
	}
	else if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T)
	{ // Rule 5
		 p2file << "Using Rule 5\n";
		 stmt (); //Call stmt
		 if (token == NUMLIT_T || token == SQUOTE_T || token == STRLIT_T){
		     cg->WriteCode(0, ", "); //Write commas between parameters
		 }
		 stmt_list (); //Call stmt_list
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::stmt ()
{
	int errors = 0;

	string message;
	if (token == IDENT_T)
	{ // Rule 8
		 p2file << "Using Rule 8\n";
		 cg->WriteCode(0, lex->Get_Lexeme()); //Write out identifier
		 token = lex->Get_Token ();

	}
	else if (token == LPAREN_T)
	{ // Rule 9
		 p2file << "Using Rule 9\n";
		 token = lex->Get_Token ();
		 errors += action (); //Call action
		 if (token == RPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else if (token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T)
	{ // Rule 7
		 p2file << "Using Rule 7\n";
		 errors += literal (); //Call literal
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::literal ()
{
	int errors = 0;

	string message;
	if (token == NUMLIT_T)
	{ // Rule 10
         cg->WriteCode(0, "Object(" + lex->Get_Lexeme() + ")"); //Write out NUMLIT_T object variable
		 p2file << "Using Rule 10\n";
		 token = lex->Get_Token ();
		 if (token == IDENT_T){
		     cg->WriteCode(0, ", ");  //Separate by commas
		 }
	}
	else if (token == STRLIT_T)
	{ // Rule 11
	    cg->WriteCode(0, "Object(" + lex->Get_Lexeme() + ")"); //Write out STRLIT_T object variable
		 p2file << "Using Rule 11\n";
		 token = lex->Get_Token ();
        if (token == STRLIT_T){
            cg->WriteCode(0, ", "); //Separate by commas
        }
	}
	else if (token == SQUOTE_T)
	{ // Rule 12
		 p2file << "Using Rule 12\n";
		 cg->WriteCode(0, "Object(\"");
		 token = lex->Get_Token ();
		 errors += quoted_lit ();
        cg->WriteCode(0, "\")");
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::quoted_lit ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T || token == IDENT_T || token == NUMLIT_T || token == STRLIT_T
		 || token == IF_T || token == DISPLAY_T || token == NEWLINE_T || token == LISTOP1_T
		 || token == LISTOP2_T || token == AND_T || token == OR_T || token == NOT_T
		 || token == DEFINE_T || token == LET_T || token == NUMBERP_T || token == LISTP_T
		 || token == ZEROP_T || token == NULLP_T || token == STRINGP_T || token == PLUS_T
		 || token == MINUS_T || token == DIV_T || token == MULT_T || token == MODULO_T
		 || token == ROUND_T || token == EQUALTO_T || token == GT_T || token == LT_T
		 || token == GTE_T || token == LTE_T || token == SQUOTE_T || token == COND_T
		 || token == ELSE_T)
	{ // Rule 13
	    space = true;
		 p2file << "Using Rule 13\n";
		 errors += any_other_token (); //Call any_other_token
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::more_tokens ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T || token == IDENT_T || token == NUMLIT_T || token == STRLIT_T
		 || token == IF_T || token == DISPLAY_T || token == NEWLINE_T || token == LISTOP1_T
		 || token == LISTOP2_T || token == AND_T || token == OR_T || token == NOT_T
		 || token == DEFINE_T || token == LET_T || token == NUMBERP_T || token == LISTP_T
		 || token == ZEROP_T || token == NULLP_T || token == STRINGP_T || token == PLUS_T
		 || token == MINUS_T || token == DIV_T || token == MULT_T || token == MODULO_T
		 || token == ROUND_T || token == EQUALTO_T || token == GT_T || token == LT_T
		 || token == GTE_T || token == LTE_T || token == SQUOTE_T || token == COND_T
		 || token == ELSE_T)
	{ // Rule 14
		 p2file << "Using Rule 14\n";
		 errors += any_other_token (); //Call any_other_token
		 errors += more_tokens (); //Call more_tokens
	}
	else if (token == RPAREN_T)
	{ // Rule 15
		 p2file << "Using Rule 15\n";
		 ;
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::param_list ()
{
	int errors = 0;

	string message;
	if (token == IDENT_T)
	{ // Rule 16
		 p2file << "Using Rule 16\n";
		 cg->WriteCode(0, "Object " + lex->Get_Lexeme()); //Write out identifiers
		 token = lex->Get_Token ();
		 if (token == IDENT_T){
		     cg->WriteCode(0, ", "); //Separate by commas
		 }
		 errors += param_list (); //Call param_list
	}
	else if (token == RPAREN_T)
	{ // Rule 17
		 p2file << "Using Rule 17\n";
		 ;
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::else_part ()
{
	int errors = 0;

	string message;
	if (token == IDENT_T || token == LPAREN_T || token == NUMLIT_T || token == STRLIT_T
		 || token == SQUOTE_T)
	{ // Rule 18
		 p2file << "Using Rule 18\n";
		 cg->WriteCode(1, "\nelse {\n"); //Write out else in C++ with {
		 errors += stmt(); //Call stmt
		 cg->WriteCode(1, "}\n"); //Close the else with }
	}
	else if (token == RPAREN_T)
	{ // Rule 19
		 p2file << "Using Rule 19\n";
		 ;
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::stmt_pair ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T)
	{ // Rule 20
		 p2file << "Using Rule 20\n";
		 token = lex->Get_Token ();
		 errors += stmt_pair_body (); //Call stmt_pair_body
	}
	else if (token == RPAREN_T)
	{ // Rule 21
		 p2file << "Using Rule 21\n";
		 ;
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::stmt_pair_body ()
{
	int errors = 0;

	string message;
	if (token == ELSE_T)
	{ // Rule 23
		 p2file << "Using Rule 23\n";
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 if (token == RPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else if (token == IDENT_T || token == LPAREN_T || token == NUMLIT_T || token == STRLIT_T
		 || token == SQUOTE_T)
	{ // Rule 22
		 p2file << "Using Rule 22\n";
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, "){\n"); //Close corresponding line with ){
		 errors += stmt (); //Call stmt
		 cg->WriteCode(1, "}\n"); //Close the stmt with }
		 if (token == RPAREN_T)
		 {
			token = lex->Get_Token ();

		 }
		 else
		 {
             cg->WriteCode(0, ")"); //Close with )
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 if (token == LPAREN_T){
             cg->WriteCode(1, "else if ("); //Else If logic
		 }
		 errors += stmt_pair (); //Call stmt_pair
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::assign_pair ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T)
	{ // Rule 24
		 p2file << "Using Rule 24\n";
		 token = lex->Get_Token ();
        cg->WriteCode(1, "Object " + lex->Get_Lexeme() + " = "); //Let logic here
		 if (token == IDENT_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[IDENT_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ";\n"); //Ending semi colon
		 if (token == RPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::more_assigns ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T)
	{ // Rule 25
		 p2file << "Using Rule 25\n";
		 errors += assign_pair (); //Call assign_pair
		 errors += more_assigns (); //Call more_assigns
	}
	else if (token == RPAREN_T)
	{ // Rule 26
		 p2file << "Using Rule 26\n";
		 ;
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::action ()
{
	int errors = 0;

	string message;
	if (token == IF_T) //If logic here
	{ // Rule 27
		 p2file << "Using Rule 27\n";
		 if (isDisplay){
             cg->WriteCode(0, lex->Get_Lexeme());
		 }
		 cg->WriteCode(1, "if (("); //Opening if
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")){\n"); //Closing the if statement
		 errors += stmt (); //Call stmt
        cg->WriteCode(1, "\n}\n"); //Closing } here
		 errors += else_part (); //Call else_part
	}
	else if (token == COND_T) //More if logic here
	{ // Rule 28
	    isCond = true;
		 p2file << "Using Rule 28\n";
		 cg->WriteCode(1, "if ("); //Opening if for cond
		 token = lex->Get_Token ();
		 if (token == LPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
		     cg->WriteCode(0, ")\n"); //Closing the if statement
			errors++;
			message = token_lexemes[LPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += stmt_pair_body (); //Call stmt_pair_body
	}
	else if (token == LET_T) //New variable logic here
	{ // Rule 29
		 p2file << "Using Rule 29\n";
		 token = lex->Get_Token ();

		 if (token == LPAREN_T)
		 {
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[LPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += more_assigns (); //Call more_assigns
		 if (token == RPAREN_T)
		 {
		     cg->WriteCode(0, "\n"); //Write a newline
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
		 errors += stmt (); //Call stmt
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == LISTOP1_T) //Car, cdr logic here
	{ // Rule 30
		 p2file << "Using Rule 30\n";
		 cg->WriteCode(0, "listop (\"" + lex->Get_Lexeme() + "\", "); //Output listop1 line
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close listop1 call
	}
	else if (token == LISTOP2_T) //Cons, append logic here
	{ // Rule 31
		 p2file << "Using Rule 31\n";
        cg->WriteCode(0, "listop (\"" + lex->Get_Lexeme() + "\", "); //Output listop2 line
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ", "); //Separate listop function calls by commas
		 errors += stmt (); //Call stmt
        cg->WriteCode(0, ")"); //Close listop2 call
	}
	else if (token == AND_T) //And logic here
	{ // Rule 32
		 p2file << "Using Rule 32\n";
		 token = lex->Get_Token ();
		 errors += stmt(); //Call stmt
		 cg->WriteCode(0, " && "); //Output &&
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == OR_T) //Or logic here
	{ // Rule 33
		 p2file << "Using Rule 33\n";
		 token = lex->Get_Token ();
        errors += stmt(); //Call stmt
        cg->WriteCode(0, " || "); //Output ||
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == NOT_T)
	{ // Rule 34
		 p2file << "Using Rule 34\n";
		 token = lex->Get_Token ();
        cg->WriteCode(0, "!("); //Output ! and beginning (
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the ! statement
	}
	else if (token == NUMBERP_T) //Numberp logic here
	{ // Rule 35
		 p2file << "Using Rule 35\n";
		 cg->WriteCode(0, "numberp ("); //Output numberp(
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the numberp call
	}
	else if (token == LISTP_T) //Listp logic here
	{ // Rule 36
		 p2file << "Using Rule 36\n";
		 cg->WriteCode(0, "listp ("); //Output listp(
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the listp call
	}
	else if (token == ZEROP_T) //Zerop logic here
	{ // Rule 37
		 p2file << "Using Rule 37\n";
		 cg->WriteCode(0, "zerop ("); //Output zerop(
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the zerop call
	}
	else if (token == NULLP_T) //Nullp logic here
	{ // Rule 38
		 p2file << "Using Rule 38\n";
		 cg->WriteCode(0, "nullp ("); //Output nullp(
		 token = lex->Get_Token ();
		 errors += stmt (); //call stmt
		 cg->WriteCode(0, ")"); //Close the nullp call
	}
	else if (token == STRINGP_T) //Stringp logic here
	{ // Rule 39
		 p2file << "Using Rule 39\n";
		 cg->WriteCode(0, "stringp ("); //Output stringp(
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the stringp call
	}
	else if (token == PLUS_T) //Plus logic here
	{ // Rule 40
        p2file << "Using Rule 40\n";
    token = lex->Get_Token ();
    cg->WriteCode(0, "("); //Output opening (
    errors += stmt (); //Call stmt
    while (token != RPAREN_T){
        cg->WriteCode(0, " + "); //Output the +
        errors += stmt(); //Call stmt
    }
    cg->WriteCode(0, ")"); //Close the statement
	}
	else if (token == MINUS_T) //Minus logic here
	{ // Rule 41
		 p2file << "Using Rule 41\n";
		 token = lex->Get_Token ();
        cg->WriteCode(0, "("); //Output opening (
        errors += stmt (); //Call stmt
		 while (token != RPAREN_T){
             cg->WriteCode(0, " - "); //Output the -
             errors += stmt(); //Call stmt
		 }
        cg->WriteCode(0, ")"); //Close the statement
	}
	else if (token == DIV_T) //Division logic here
	{ // Rule 42
		 p2file << "Using Rule 42\n";
       token = lex->Get_Token ();
        cg->WriteCode(0, "(");  //Output opening (
        errors += stmt (); //Call stmt
		 while (token != RPAREN_T){
             cg->WriteCode(0, " / "); //Output the /
             errors += stmt(); //Call stmt
		 }
        cg->WriteCode(0, ")"); //Close the statement
	}
	else if (token == MULT_T) //Multiplication logic here
	{ // Rule 43
		 p2file << "Using Rule 43\n";
        token = lex->Get_Token ();
        cg->WriteCode(0, "("); //Output the opening (
        errors += stmt (); //Call stmt
        while (token != RPAREN_T){
            cg->WriteCode(0, " * "); //Output the *
            errors += stmt(); //Call stmt
        }
        cg->WriteCode(0, ")"); //Close the statement

	}
	else if (token == MODULO_T) //Mod logic here
	{ // Rule 44
		 p2file << "Using Rule 44\n";
        token = lex->Get_Token ();
        cg->WriteCode(0, "("); //Output the opening (
        errors += stmt (); //Call stmt
        while (token != RPAREN_T){
            cg->WriteCode(0, " % "); //Output the %
            errors += stmt(); //Call stmt
        }
        cg->WriteCode(0, ")"); //Close the statement
	}
	else if (token == ROUND_T) //Round logic here
	{ // Rule 45
		 p2file << "Using Rule 45\n";
        cg->WriteCode(0, "round ("); //Output the round function call
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ")"); //Close the function call
	}
	else if (token == EQUALTO_T) //Equal to logic here
	{ // Rule 46
		 p2file << "Using Rule 46\n";
		 token = lex->Get_Token ();
        errors += stmt(); //Call stmt
        cg->WriteCode(0, " == "); //Output the ==
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == GT_T) //Greater than logic here
	{ // Rule 47
		 p2file << "Using Rule 47\n";
		 token = lex->Get_Token ();
        errors += stmt(); //Call stmt
        cg->WriteCode(0, " > "); //Output the >
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == LT_T) //Less than logic here
	{ // Rule 48
		 p2file << "Using Rule 48\n";
		 token = lex->Get_Token ();
		 errors += stmt(); //Call stmt
        cg->WriteCode(0, " < "); //Output the <
		 errors += stmt_list (); //Call stmt_list
	}
	else if (token == GTE_T) //Greater than or equal to logic here
	{ // Rule 49
		 p2file << "Using Rule 49\n";
		 token = lex->Get_Token ();
		 cg->WriteCode(0, "("); //Open the statement
		 errors += stmt(); //Call stmt
		 cg->WriteCode(0, " >= "); //Output the >=
		 errors += stmt_list (); //Call stmt_list
		 cg->WriteCode(0, ")"); //Close the statement
	}
	else if (token == LTE_T) //Less than or equal to logic here
	{ // Rule 50
		 p2file << "Using Rule 50\n";
		 token = lex->Get_Token ();
		 cg->WriteCode(0, "("); //Open the statement
		 errors += stmt(); //Call stmt
		 cg->WriteCode(0, " <= "); //Output the <=
		 errors += stmt_list (); //Call stmt_list
		 cg->WriteCode(0, ")"); //Close the statement

	}
	else if (token == IDENT_T) //Identifier logic here
	{ // Rule 51
		 p2file << "Using Rule 51\n";
		 cg->WriteCode(1, "(__RetVal = " + lex->Get_Lexeme() + "("); //RetVal = line here
		 token = lex->Get_Token ();
		 errors += stmt_list (); //Call stmt_list
		 cg->WriteCode(0, "));\n"); //Close the statement
	}
	else if (token == DISPLAY_T) //Cout logic here
	{ // Rule 52
		 p2file << "Using Rule 52\n";
		 if (lex->Get_Lexeme() == "display"){ //If the lexeme is display...
             isDisplay = true; //Set bool to true
		 }
		 if (isDisplay){ //If bool is true...
             cg->WriteCode(1, "cout << "); //Output cout <<
		 }
		 token = lex->Get_Token ();
		 errors += stmt (); //Call stmt
		 cg->WriteCode(0, ";\n"); //End line with semi colon
	}
	else if (token == NEWLINE_T) //Endl logic here
	{ // Rule 53
	    cg->WriteCode(1, "cout << endl;\n"); //Output endl here
		 p2file << "Using Rule 53\n";
		 token = lex->Get_Token ();
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

int Syntactical_Analyzer::any_other_token ()
{
	int errors = 0;

	string message;
	if (token == LPAREN_T)
	{ // Rule 54
		 p2file << "Using Rule 54\n";
        cg->WriteCode(0, lex->Get_Lexeme()); //Write out current lexeme
		 token = lex->Get_Token ();
		 errors += more_tokens (); //Call more_tokens
		 if (token == RPAREN_T)
		 {
             cg->WriteCode(0, lex->Get_Lexeme()); //Write out current lexeme
			token = lex->Get_Token ();
		 }
		 else
		 {
			errors++;
			message = token_lexemes[RPAREN_T] + " expected";
			lex->Report_Error (message);
		 }
	}
	else if (token == IDENT_T)
	{ // Rule 55
		 p2file << "Using Rule 55\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == NUMLIT_T)
	{ // Rule 56
		 p2file << "Using Rule 56\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == STRLIT_T)
	{ // Rule 57
		 p2file << "Using Rule 57\n";
		 char firstFound;
		 char lastFound;
		 firstFound = lex->Get_Lexeme().find_first_of('\"'); //Find first "
        lastFound = lex->Get_Lexeme().find_last_of('\"'); //Find last "
        cg->WriteCode(0, lex->Get_Lexeme().erase(lastFound).erase(firstFound, 1) + " "); //Write out current lexeme without quotations
		 token = lex->Get_Token ();
	}
	else if (token == IF_T)
	{ // Rule 58
		 p2file << "Using Rule 58\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == DISPLAY_T)
	{ // Rule 59
		 p2file << "Using Rule 59\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == NEWLINE_T)
	{ // Rule 60
		 p2file << "Using Rule 60\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LISTOP1_T)
	{ // Rule 61
		 p2file << "Using Rule 61\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LISTOP2_T)
	{ // Rule 62
		 p2file << "Using Rule 62\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == AND_T)
	{ // Rule 63
		 p2file << "Using Rule 63\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == OR_T)
	{ // Rule 64
		 p2file << "Using Rule 64\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == NOT_T)
	{ // Rule 65
		 p2file << "Using Rule 65\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == DEFINE_T)
	{ // Rule 66
		 p2file << "Using Rule 66\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LET_T)
	{ // Rule 67
		 p2file << "Using Rule 67\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == NUMBERP_T)
	{ // Rule 68
		 p2file << "Using Rule 68\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LISTP_T)
	{ // Rule 69
		 p2file << "Using Rule 69\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == ZEROP_T)
	{ // Rule 70
		 p2file << "Using Rule 70\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == NULLP_T)
	{ // Rule 71
		 p2file << "Using Rule 71\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == STRINGP_T)
	{ // Rule 72
		 p2file << "Using Rule 72\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == PLUS_T)
	{ // Rule 73
		 p2file << "Using Rule 73\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == MINUS_T)
	{ // Rule 74
		 p2file << "Using Rule 74\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == DIV_T)
	{ // Rule 75
		 p2file << "Using Rule 75\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == MULT_T)
	{ // Rule 76
		 p2file << "Using Rule 76\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == MODULO_T)
	{ // Rule 77
		 p2file << "Using Rule 77\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == ROUND_T)
	{ // Rule 78
		 p2file << "Using Rule 78\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == EQUALTO_T)
	{ // Rule 79
		 p2file << "Using Rule 79\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == GT_T)
	{ // Rule 80
		 p2file << "Using Rule 80\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LT_T)
	{ // Rule 81
		 p2file << "Using Rule 81\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == GTE_T)
	{ // Rule 82
		 p2file << "Using Rule 82\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == LTE_T)
	{ // Rule 83
		 p2file << "Using Rule 83\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == SQUOTE_T)
	{ // Rule 84
		 p2file << "Using Rule 84\n";
		 token = lex->Get_Token ();
		 errors += any_other_token ();
	}
	else if (token == COND_T)
	{ // Rule 85
		 p2file << "Using Rule 85\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else if (token == ELSE_T)
	{ // Rule 86
		 p2file << "Using Rule 86\n";
        cg->WriteCode(0, lex->Get_Lexeme() + " "); //Write out current lexeme
		 token = lex->Get_Token ();
	}
	else 
	{
		message = token_lexemes[token] + " unexpected";
		lex->Report_Error (message);
	}
	return errors;
}

