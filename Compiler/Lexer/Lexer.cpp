#include "Lexer.h"
#define LookUp(dict, key) ((dict.find(key) != dict.end()))


bool issymbol(char ch)
{
	switch (ch)
	{
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':
		case '>':
		case '<':

		case '(':
		case ')':
		case ';':
		case ',':

		case ':':
			return true;
		default:
			return false;
	}
}

char Lexer::peek()
{
	return _fin.peek();
}

char Lexer::advance()
{
	char ch = _fin.get();
	if (ch == '\n')
	{
		++_row;
		_column = 0;
	}
	else
	{
		++_column;
	}
	return ch;
}

void Lexer::reverse()
{
	if (_column != 0)
		--_column;
	else if (_row != 0)
		--_row;
	else
		return;
	_fin.unget();
}

Lexer::Lexer(string filename)
{
	_fin.open(filename);
	_row = 1;
	_column = 0;

	_keyword_dict.insert("PROGRAM");
	_keyword_dict.insert("BEGIN");
	_keyword_dict.insert("END");
	_keyword_dict.insert("CONST");
	_keyword_dict.insert("VAR");
	_keyword_dict.insert("WHILE");
	_keyword_dict.insert("DO");
	_keyword_dict.insert("IF");
	_keyword_dict.insert("THEN");

	_operator_dict.insert("+");
	_operator_dict.insert("-");
	_operator_dict.insert("*");
	_operator_dict.insert("/");
	_operator_dict.insert(":=");
	_operator_dict.insert("=");
	_operator_dict.insert("<>");
	_operator_dict.insert(">");
	_operator_dict.insert(">=");
	_operator_dict.insert("<");
	_operator_dict.insert("<=");

	_delimiter_dict.insert('(');
	_delimiter_dict.insert(')');
	_delimiter_dict.insert(';');
	_delimiter_dict.insert(',');

	while (true)
	{
		Token token = NextToken();
		_token_queue.push(token);
		if (token.type == Token::ERROR_TOKEN.type)
			break;
		if (token.type == Token::EOF_TOKEN.type)
			break;
	}
}


bool Lexer::ScanComments()
{
	while (isspace(peek()))
		advance();

	/*记录起始位置*/

	char ch = peek();
	if (ch == '{')
	{
		while (advance() != '}')
		{
			if (_fin.eof())
			{
				return false;
			}
		}
		goto found;
	}
	else if (ch == '/')
	{
		advance();
		if ('/' == peek())
		{
			advance();				// get the second '/'
			char ch;
			while ((ch = advance()) != '\n' && !_fin.eof())
				;
			goto found;
		}
		else
		{
			reverse();
		}
	}
	else if (ch == '(')
	{
		advance();
		if ('*' == peek())
		{
			advance();				// get the '*'
			int state = 0;
			bool loop = true;
			while (loop)
			{
				if (_fin.eof())
				{
					return false;
				}
				switch (state)
				{
				case 0:
					if (advance() == '*')
						state = 1;
					break;
				case 1:
					if (advance() == ')')
						loop = false;
					else
						state = 0;
					break;
				default:
					break;
				}
			}
			goto found;
		}
		else
		{
			reverse();
		}
	}
	return true;
found:
	ScanComments();
	return true;
}

Token Lexer::NextToken()
{
	int row = _row, column = _column;
	if (!ScanComments())
	{
		Token token = Token::ERROR_TOKEN;
		token.location = pair<int, int>(row, column);
		return token;
	}
	while (isspace(peek()))
		advance();
	row = _row, column = _column; // get location again
	if (_fin.eof())
	{
		Token token = Token::EOF_TOKEN;
		token.location = pair<int, int>(row, column);
		return token;
	}
	char ch = peek();
	if (isalpha(ch))
	{
		Token token = ScanIdentifierOrKeyword();       // return indetifier or keyword 
		token.location = pair<int, int>(row, column);
		return token;
	}
	if (isdigit(ch))
	{

		Token token = ScanNumber();
		token.location = pair<int, int>(row, column);
		return token;
	}
	if (LookUp(_delimiter_dict, ch))
	{
		Token token = ScanDelimiter();
		token.location = pair<int, int>(row, column);
		return token;
	}
	Token token = ScanOperator();             // if not a operator, return ERROR token
	token.location = pair<int, int>(row, column);
	return token;
}


Token Lexer::ScanDelimiter()
{
	Token token = Token::DELIMITER_TOKEN;
	string buf(1, advance());
	token.value.s = new char[buf.length() + 1];
	strcpy(token.value.s, buf.c_str());
	return token;
}

Token Lexer::ScanOperator()
{
	string buf(1, advance());

	/* match double-char operator first*/
	if (LookUp(_operator_dict, buf + (char)peek()))
		buf += advance();

	if (!LookUp(_operator_dict, buf))
		return Token::ERROR_TOKEN;

	Token token = Token::OPERATOR_TOKEN;
	token.value.s = new char[buf.length() + 1];
	strcpy(token.value.s, buf.c_str());
	return token;
}

Token Lexer::ScanIdentifierOrKeyword()
{
	string buf;
	while (isalpha(peek()) || isdigit(peek()))
	{
		buf += advance();
	}

	if (LookUp(_keyword_dict, buf))
	{
		Token token = Token::KEYWORD_TOKEN;
		token.value.s = new char[buf.length() + 1];
		strcpy(token.value.s, buf.c_str());
		return token;
	}
	else
	{
		Token token = Token::IDENTIFIER_TOKEN;
		token.value.s = new char[buf.length() + 1];
		strcpy(token.value.s, buf.c_str());
		return token;
	}
}

Token Lexer::ScanNumber()
{
	enum State
	{
		FAIL, EMPTY, ZERO, INTEGER, FLOAT, DOT
	};

	int base = 10;
	double power = 0.1;
	int integer = 0;
	double decimal = 0;
	State current_state = EMPTY;
	bool loop = true;

	while (loop)
	{
		char ch = peek();
		if (issymbol(ch) || isspace(ch) || _fin.eof())
			break;
		advance();

		switch (current_state)
		{
		case FAIL:
			loop = false;
			break;

		case EMPTY:
			if (ch == '0')
			{
				current_state = ZERO;
			}
			else if (isdigit(ch))
			{
				integer = integer * base + ch - '0';
				current_state = INTEGER;
			}
			else
			{
				current_state = FAIL;
			}
			break;

		case ZERO:
			if (ch == '.')
			{
				current_state = DOT;
			}
			else
			{
				current_state = FAIL;
			}
			break;

		case INTEGER:
			if (isdigit(ch))
			{
				integer = integer * base + ch - '0';
			}
			else if (ch == '.')
			{
				current_state = DOT;
			}
			else
			{
				current_state = FAIL;
			}
			break;

		case FLOAT:
			if (isdigit(ch))
			{
				power /= 10;
				decimal += (ch - '0') * power;
			}
			else
			{
				current_state = FAIL;
			}
			break;

		case DOT:
			if (isdigit(ch))
			{
				current_state = FLOAT;
				decimal += (ch - '0') * power;
			}
			else
			{
				current_state = FAIL;
			}
			break;

		default:
			break;
		}
	}

	switch (current_state)
	{
		case ZERO:
		case INTEGER:
		{
			Token token = Token::INTEGER_TOKEN;
			token.value.i = integer;
			return token;
		}
		case FLOAT:
		{
			Token token = Token::DOUBLE_TOKEN;
			token.value.d = integer + decimal;
			return token;
		}
		default:
			return Token::ERROR_TOKEN;
	}
}

Token Lexer::PeekToken()
{
	Token token = _token_queue.front();
	return token;
}

void Lexer::AdvanceToken()
{
	_token_queue.pop();
}

bool Lexer::NoMoreToken()
{
	return _token_queue.empty();
}
Lexer::~Lexer()
{
	_fin.close();
}
