#include "Parser.h"
#define PD(name) AST* Parser::Parse##name()
#define CPD(name) root->children.push_back(Parse##name())
#define ADDT(name) root->children.push_back(name)
#define INIT AST *root = new AST()

Parser::Parser(Lexer *lex)
{
	_root = new AST();
	_root->type = "root";
	_lex = lex;
	try
	{
		_root->children.push_back(ParsePROGRAM());
	}
	catch (exception e)
	{
		Token error_token = _lex->PeekToken();
		cout << e.what();
		if (error_token.type != Token::INTEGER_TOKEN.type && error_token.type != Token::DOUBLE_TOKEN.type)
			cout << " : " << error_token.value.s << endl;
		else
			cout << endl;
		cout << "location at " << error_token.location.first << " row, " << error_token.location.second << " column." << endl;
		exit(-1);
	}
	_root->Show();
}

Parser::~Parser()
{
	destroy(_root);
}

void Parser::destroy(AST* root)
{
	if (root->type == terminal)
	{
		delete root->token;
		delete root;
		return;
	}
	for (auto child : root->children)
	{
		destroy(child);
	}
	delete root;
}

AST* Parser::expect(const string &type, const string &value)
{
	AST* root = new AST();
	Token token = _lex->PeekToken();
	if (token.type != type)
		//throw SyntaxError(string("unexpected " + token.type + " token").c_str());
		throw exception(string("unexpected " + token.type + " token").c_str());
	if (token.type == Token::DOUBLE_TOKEN.type ||
		token.type == Token::INTEGER_TOKEN.type ||
		token.type == Token::IDENTIFIER_TOKEN.type)
		goto MATCH;
	else if (strcmp(token.value.s, value.c_str()) == 0)
		goto MATCH;
	//throw SyntaxError(string("unexpected ") + token.value.s);
	throw exception(string("unexpected " + string(token.value.s)).c_str());

MATCH:
	root->token = new Token(token);
	root->type = terminal;
	_lex->AdvanceToken();
	return root;
}

bool Parser::test(const string &type, const string &value)
{
	Token token = _lex->PeekToken();
	if (token.type != type)
		return false;
	if (token.type == Token::DOUBLE_TOKEN.type ||
		token.type == Token::INTEGER_TOKEN.type ||
		token.type == Token::IDENTIFIER_TOKEN.type)
		return true;
	else if (strcmp(token.value.s, value.c_str()) == 0)
		return true;
	return false;
}

PD(PROGRAM)
{
	INIT;
	root->type = program;
	CPD(PROGRAM_HEAD);
	CPD(PROGRAM_BODY);
	return root;
}

PD(PROGRAM_HEAD)
{
	INIT;
	root->type = program_head;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "PROGRAM"));
	ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	return root;
}

PD(PROGRAM_BODY)
{
	INIT;
	root->type = program_body;
	if (test(Token::KEYWORD_TOKEN.type, "CONST"))
		CPD(CONST_DECLARATION);
	if (test(Token::KEYWORD_TOKEN.type, "VAR"))
		CPD(VAR_DECLARATION);
	CPD(STATEMENT_SECTION);
	return root;
}


PD(CONST_DECLARATION)
{
	INIT;
	root->type = const_declaration;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "CONST"));
	CPD(CONST_DEFINITION);
	while (true)
	{
		if (!test(Token::DELIMITER_TOKEN.type, ","))
			break;
		ADDT(expect(Token::DELIMITER_TOKEN.type, ","));
		CPD(CONST_DEFINITION);
	}
	ADDT(expect(Token::DELIMITER_TOKEN.type, ";"));
	return root;
}

PD(VAR_DECLARATION)
{
	INIT;
	root->type = var_declaration;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "VAR"));
	ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	while (true)
	{
		if (!test(Token::DELIMITER_TOKEN.type, ","))
			break;
		ADDT(expect(Token::DELIMITER_TOKEN.type, ","));
		ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	}
	ADDT(expect(Token::DELIMITER_TOKEN.type, ";"));
	return root;
}
PD(STATEMENT_SECTION)
{
	INIT;
	root->type = statement_section;
	if (test(Token::KEYWORD_TOKEN.type, "BEGIN"))
		CPD(COMPOUND_STATEMENT);
	else
		CPD(STATEMENT);
	return root;
}
PD(CONST_DEFINITION)
{
	INIT;
	root->type = const_definition;
	ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	ADDT(expect(Token::OPERATOR_TOKEN.type, "="));
	ADDT(expect(Token::INTEGER_TOKEN.type));
	return root;
}
PD(STATEMENT)
{
	INIT;
	root->type = statement;
	if (test(Token::IDENTIFIER_TOKEN.type))
		CPD(ASSIGN_STATEMENT);
	else if (test(Token::KEYWORD_TOKEN.type, "IF"))
		CPD(CONDITION_STATEMENT);
	else if (test(Token::KEYWORD_TOKEN.type, "WHILE"))
		CPD(LOOP_STATEMENT);
	else if (test(Token::KEYWORD_TOKEN.type, "BEGIN"))
		CPD(COMPOUND_STATEMENT);
	else
		CPD(EMPTY_STATEMENT);
	return root;
}
PD(COMPOUND_STATEMENT)
{
	INIT;
	root->type = compound_statement;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "BEGIN"));
	CPD(STATEMENT);
	while (true)
	{
		if (!test(Token::DELIMITER_TOKEN.type, ";"))
			break;
		ADDT(expect(Token::DELIMITER_TOKEN.type, ";"));
		CPD(STATEMENT);
	}
	ADDT(expect(Token::KEYWORD_TOKEN.type, "END"));
	return root;
}
PD(ASSIGN_STATEMENT)
{
	INIT;
	root->type = assign_statement;
	ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	ADDT(expect(Token::OPERATOR_TOKEN.type, ":="));
	CPD(EXPRESSION);
	return root;
}
PD(CONDITION_STATEMENT)
{
	INIT;
	root->type = condition_statement;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "IF"));
	CPD(CONDITION);
	ADDT(expect(Token::KEYWORD_TOKEN.type, "THEN"));
	CPD(STATEMENT);
	return root;
}
PD(LOOP_STATEMENT)
{
	INIT;
	root->type = loop_statement;
	ADDT(expect(Token::KEYWORD_TOKEN.type, "WHILE"));
	CPD(CONDITION);
	ADDT(expect(Token::KEYWORD_TOKEN.type, "DO"));
	CPD(STATEMENT);
	return root;
}
PD(EMPTY_STATEMENT)
{
	INIT;
	root->type = empty_statement;
	if (test(Token::DELIMITER_TOKEN.type, ";"))
		ADDT(expect(Token::DELIMITER_TOKEN.type, ";"));
	return root;
}
PD(EXPRESSION)
{
	INIT;
	root->type = expression;
	if (test(Token::OPERATOR_TOKEN.type, "+"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "+"));
	else if (test(Token::OPERATOR_TOKEN.type, "-"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "-"));
	CPD(ENTRY);
	CPD(EXPRESSION_ASSIST);
	return root;
}

PD(EXPRESSION_ASSIST)
{
	INIT;
	root->type = expression_assist;
	if (test(Token::OPERATOR_TOKEN.type, "+") || test(Token::OPERATOR_TOKEN.type, "-"))
	{
		CPD(ADD_OPERATOR);
		CPD(ENTRY);
		CPD(EXPRESSION_ASSIST);
	}
	else
	{
		AST* child = new AST();
		child->type = empty;
		root->children.push_back(child);
	}
	return root;

}
PD(ENTRY)
{
	INIT;
	root->type = entry;
	CPD(FACTOR);
	CPD(ENTRY_ASSIST);
	return root;
}
PD(ENTRY_ASSIST)
{
	INIT;
	root->type = entry_assist;
	if (test(Token::OPERATOR_TOKEN.type, "*") || test(Token::OPERATOR_TOKEN.type, "/"))
	{
		CPD(MULTI_OPERATOR);
		CPD(FACTOR);
		CPD(ENTRY_ASSIST);
	}
	else
	{
		AST* child = new AST();
		child->type = empty;
		root->children.push_back(child);
	}
	return root;
}
PD(ADD_OPERATOR)
{
	INIT;
	root->type = add_operator;
	if (test(Token::OPERATOR_TOKEN.type, "+"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "+"));
	else
		ADDT(expect(Token::OPERATOR_TOKEN.type, "-"));
	return root;
}
PD(MULTI_OPERATOR)
{
	INIT;
	root->type = multi_operator;
	if (test(Token::OPERATOR_TOKEN.type, "*"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "*"));
	else
		ADDT(expect(Token::OPERATOR_TOKEN.type, "/"));
	return root;
}
PD(FACTOR)
{
	INIT;
	root->type = factor;
	if (test(Token::IDENTIFIER_TOKEN.type))
		ADDT(expect(Token::IDENTIFIER_TOKEN.type));
	else if (test(Token::INTEGER_TOKEN.type))
		ADDT(expect(Token::INTEGER_TOKEN.type));
	else
	{
		ADDT(expect(Token::DELIMITER_TOKEN.type, "("));
		CPD(EXPRESSION);
		ADDT(expect(Token::DELIMITER_TOKEN.type, ")"));
	}
	return root;
}
PD(CONDITION)
{
	INIT;
	root->type = condition;
	CPD(EXPRESSION);
	CPD(RELATIONAL_OPERATOR);
	CPD(EXPRESSION);
	return root;
}
PD(RELATIONAL_OPERATOR)
{
	INIT;
	root->type = relational_operator;
	if (test(Token::OPERATOR_TOKEN.type, "="))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "="));
	else if (test(Token::OPERATOR_TOKEN.type, "<>"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "<>"));
	else if (test(Token::OPERATOR_TOKEN.type, "<"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "<"));
	else if (test(Token::OPERATOR_TOKEN.type, "<="))
		ADDT(expect(Token::OPERATOR_TOKEN.type, "="));
	else if (test(Token::OPERATOR_TOKEN.type, ">"))
		ADDT(expect(Token::OPERATOR_TOKEN.type, ">"));
	else
		ADDT(expect(Token::OPERATOR_TOKEN.type, ">="));
	return root;
}
#undef ADDT
#undef CPD
#undef PD
#undef INIT