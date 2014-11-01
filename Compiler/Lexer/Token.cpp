#include "Token.h"
#include <iomanip>
#undef EOF
#define DEFINE_TOKEN(NAME) const Token Token::NAME##_TOKEN = Token(#NAME)
DEFINE_TOKEN(ERROR);
DEFINE_TOKEN(EOF);
DEFINE_TOKEN(KEYWORD);
DEFINE_TOKEN(IDENTIFIER);
DEFINE_TOKEN(DELIMITER);
DEFINE_TOKEN(OPERATOR);
DEFINE_TOKEN(INTEGER);
DEFINE_TOKEN(DOUBLE);
#undef DEFINE_TOKEN
using namespace std;

Token::Token(std::string type)
	:type(type)
{
}

std::ostream& operator<<(std::ostream& output, const Token& rhs)
{
	if (rhs.type == Token::ERROR_TOKEN.type)
	{
		output << "* ERROR at " << rhs.location.first << " row," << rhs.location.second << "column *" << endl;
		return output;
	}
	output << setw(10) << left << rhs.type << "   ";
	if (rhs.type == Token::INTEGER_TOKEN.type)
		output << setw(10) << left << rhs.value.i;
	else if (rhs.type == Token::DOUBLE_TOKEN.type)
		output << setw(10) << left << setprecision(10) << rhs.value.d;
	else
		output << setw(10) << left << rhs.value.s;
	output << "location at " << rhs.location.first << " row," << rhs.location.second << "column" << endl;
	return output;

}
