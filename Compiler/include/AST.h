#ifndef AST_H
#define AST_H
#include <string>
#include <vector>
#include "Token.h"
class AST
{
public:
	AST();
	std::string type;
	Token *token;
	std::vector<AST*> children;
	void Show();
};

#endif