#ifndef PARSER_H
#define PARSER_H
#include "AST.h"
#include "Lexer.h"
#include <stdexcept>
#define PD(name) AST* Parse##name()
#define STR(name) const string name
class SyntaxError : public logic_error
{
public:
	using logic_error::logic_error;
};

class Parser
{
public:
	Parser(Lexer *lex);
	~Parser();


private:
	STR(terminal) = "TM";
	STR(empty) = "EP";
	STR(program) = "PG";
	STR(program_head) = "PG_H";
	STR(program_body) = "PG_B";
	STR(const_declaration) = "C_DC";
	STR(var_declaration) = "V_DC";
	STR(statement_section) = "ST_S";
	STR(const_definition) = "C_DF";
	STR(statement) = "ST";
	STR(compound_statement) = "CP_ST";
	STR(assign_statement) = "AS_ST";
	STR(condition_statement) = "CD_ST";
	STR(loop_statement) = "LP_ST";
	STR(empty_statement) = "EP_ST";
	STR(expression) = "EXP";
	STR(expression_assist) = "EXP_AT";
	STR(entry) = "ET";
	STR(entry_assist) = "ET_AT";
	STR(add_operator) = "ADD_OP";
	STR(multi_operator) = "MUL_OP";
	STR(factor) = "FT";
	STR(condition) = "CD";
	STR(relational_operator) = "RL_OP";
	AST *_root;
	Lexer *_lex;
	AST* expect(const string &type, const string &value = "");
	bool test(const string &type, const string &value = "");
	void destroy(AST* root);
	PD(PROGRAM);
	PD(PROGRAM_HEAD);
	PD(PROGRAM_BODY);
	PD(CONST_DECLARATION);
	PD(VAR_DECLARATION);
	PD(STATEMENT_SECTION);
	PD(CONST_DEFINITION);
	PD(STATEMENT);
	PD(COMPOUND_STATEMENT);
	PD(ASSIGN_STATEMENT);
	PD(CONDITION_STATEMENT);
	PD(LOOP_STATEMENT);
	PD(EMPTY_STATEMENT);
	PD(EXPRESSION);
	PD(EXPRESSION_ASSIST);
	PD(ENTRY);
	PD(ENTRY_ASSIST);
	PD(ADD_OPERATOR);
	PD(MULTI_OPERATOR);
	PD(FACTOR);
	PD(CONDITION);
	PD(RELATIONAL_OPERATOR);
};
#undef PD
#endif // !PARSER_H
