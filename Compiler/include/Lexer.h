#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <fstream>
#include <set>
#include <iostream>
#include "Token.h"
#include <queue>
using namespace std;
#define see(x) cerr << #x << '=' << (x) << endl
class  Lexer
{
public:
	 Lexer(string filename);
	 /* return next token
	  * if match error string, return Token(ERROR)
	  * if match EOF, return Token(EOF)
	  */
	 //string CurrentLocation() const;
	 Token PeekToken();
	 void AdvanceToken();
	 bool NoMoreToken();
	~ Lexer();

private:
	ifstream _fin;
	int _row, _column;
	set<string> _keyword_dict;
	set<string> _operator_dict;
	set<char> _delimiter_dict;
	queue<Token> _token_queue;

	char peek();
	char advance();
	void reverse();
	Token NextToken();
	Token ScanIdentifierOrKeyword();
	Token ScanOperator();
	Token ScanNumber();
	Token ScanDelimiter();
	bool ScanComments();
};

#endif // !LEXER_H
