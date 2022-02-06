/*******************************************************************************
* Assignment: Project 3 - Code Generator for Scheme to C++ Translator      *
* Author: Dr. Watts                                                            *
* Date: Fall 2017                                                              *
* File: Project2.cpp                                                           *
*                                                                              *
* Description: This file contains the                                          *
*******************************************************************************/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "Set_Limits.h"
#include "Syntactical_Analyzer.h"

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf ("format: %s <filename>\n", argv[0]);
		exit (1);
	}
	Set_Limits ();
	Syntactical_Analyzer parse (argv[1]);

	return 0;
}
