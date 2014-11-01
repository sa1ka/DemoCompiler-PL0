#include <iostream>
#include <fstream>
#include <iomanip>
#include "Lexer.h"
#include "Token.h"
#include "Parser.h"
#include <map>
#include <vector>
using namespace std;

void main()
{
	freopen("result.txt", "w", stdout);
	string input_filename = "test.txt";
	Lexer lexer(input_filename);
	Parser parser(&lexer);
}