#include "AST.h"
#include <queue>
#include <iostream>
#include <iomanip>
using namespace std;
AST::AST()
{
	token = nullptr;
}

int Width(AST* root)
{
	if (root->type == "TM" || root->type == "EP")
		return 1;
	int width = 0;
	for (auto child : root->children)
		width += Width(child);
	return width;
}

void AST::Show()
{
	queue<pair<AST*, int>> ASTQueue;
	int cfloor = 1, nfloor = 0;
	int format = 7;
	ASTQueue.push(pair<AST*, int>(this, 0));
	while (!ASTQueue.empty())
	{
		int position = 0;
		while (cfloor)
		{
			pair<AST*, int> ast = ASTQueue.front();
			ASTQueue.pop();
			cfloor--;
			for (; position < ast.second - 1;position++)
				cout << setw(format) << left << "  ";
			cout << setw(format) << left << ast.first->type;
			position++;
			for (auto child : ast.first->children)
			{
				ASTQueue.push(pair<AST*, int>(child, position));
				nfloor++;
			}
			for (int i = 0; i < Width(ast.first) - 1; ++i)
			{
				cout << setw(format) << left << "  ";
				position++;
			}
		}
		cout << endl;
		cfloor = nfloor;
		nfloor = 0;
	}
}

