#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include <ostream>

bool issymbol(char ch);

class Token
{
public:
	static const Token ERROR_TOKEN;
	static const Token EOF_TOKEN;
	static const Token KEYWORD_TOKEN;
	static const Token IDENTIFIER_TOKEN;
	static const Token DELIMITER_TOKEN;
	static const Token OPERATOR_TOKEN;
	static const Token INTEGER_TOKEN;
	static const Token DOUBLE_TOKEN;
	const std::string type;
	std::pair<int, int> location;
	friend std::ostream& operator<<(std::ostream& output, const Token& rhs);
	union
	{
		int i;
		char *s;
		double d;
	}value;
private:
	Token() = default;
	Token(std::string type);
};
#endif