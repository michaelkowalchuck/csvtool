
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <new>

#include "bc_cc.h"

#include "sqlparse.h"

namespace SqlParse
{

Lexable::Lexable(const std::string &itype,const std::string ilexeme)
{
	_type = itype;
	_lexeme = ilexeme;
	_csv_data_map  = 0;
}

std::string Lexable::get_lexeme()const
{
	return _lexeme;
}

void Lexable::set_lexeme(const std::string &lexeme)
{
	_lexeme = lexeme;
}

std::string Lexable::get_type()const
{
	return _type;
}

void Lexable::set_data(const std::map<std::string,std::string> &csv_data_map)
{
	//std::cout << "Lexable::set_data() ::::::: type:::" << get_type() << "     " << get_lexeme() << std::endl;

	_csv_data_map = &csv_data_map;

}

void Lexable::get_csv_data_map_iterators(
								std::map<std::string,std::string>::const_iterator &b,
								std::map<std::string,std::string>::const_iterator &e
							)const
{
	if(_csv_data_map) {
		b = _csv_data_map->begin();
		e = _csv_data_map->end();
	} else {
		error_die("Lexable::get_csv_data_map_iterators() failed because _csv_data_map is not set!");
	}
}

TextNode::TextNode(const std::string &ilexeme)
	: Lexable("text_node",ilexeme)
{
}

StringNode::StringNode(const std::string &ilexeme)
	: Lexable("string_node",ilexeme)
{
}

NumberNode::NumberNode(const std::string &ilexeme)
	: Lexable("number_node",ilexeme)
{
}

float NumberNode::toFloat()const
{
	return atof(get_lexeme().c_str());
}

int NumberNode::toInt()const
{
	return atoi(get_lexeme().c_str());
}

bool NumberNode::toBool()const
{
	std::string s = get_lexeme();
	return bc::string_to_bool(s);
}

BoolNode::BoolNode(const std::string &itype,const std::string ilexeme)
	: Lexable(itype,ilexeme)
{
}

bool BoolNode::evaluate()const
{
	error_die("BoolNode::evaluate() cannot be called!");
}

BoolVal::BoolVal(BoolVal *lhs,BoolOp *op,BoolVal *rhs)
	: BoolNode("BoolVal","no_lexeme")
{
	_child = 0;
	_left = lhs;
	_op = op;
	_right = rhs;
}

BoolVal::BoolVal(BoolNode *child)
	: BoolNode("BoolVal","no_lexeme")
{
	_child = child;
	_left = 0;
	_right = 0;
	_op = 0;
}

void BoolVal::set_data(const std::map<std::string,std::string> &csv_data_map)
{
	if(_child)
		_child->set_data(csv_data_map);
	if(_op) {
		_left->set_data(csv_data_map);
		_right->set_data(csv_data_map);
	}
}

bool BoolVal::evaluate()const
{
	if(_child)
		return _child->evaluate();
	else if(_op)
		return _op->op_eval(_left,_right);
	error_die("BoolVal::evaluate() impossible");
}


BoolOp::BoolOp(const std::string &lexeme)
	: BoolNode("BoolOp",lexeme)
{

}

bool BoolOp::evaluate()const
{
	error_die("BoolOp::evaluate() should not be called, use op_eval() instead");
}

bool BoolOp::op_eval(const BoolVal *lhs,const BoolVal *rhs)const
{
	std::string lexeme = get_lexeme();
	if(bc::toLower(lexeme) == "and")
		return lhs->evaluate() && rhs->evaluate();
	else if(bc::toLower(lexeme) == "or")
		return lhs->evaluate() || rhs->evaluate();
	else {
		error_die("BoolOp::op_eval() unknown op lexeme:" + lexeme);
	}
}

NodeDBExprOp::NodeDBExprOp(const std::string &lexeme)
	: DBNode("nodedbexprop",lexeme)
{
//std::cout << "NodeDBExprOp::NodeDBExprOp()" << std::endl;
}

bool NodeDBExprOp::op_eval(const NodeDBVal *lhs,const NodeDBVal *rhs)const
{
	std::string resolution_type = get_resolution_type(lhs->get_resolution_type(),rhs->get_resolution_type());
	std::string lexeme = get_lexeme();

	if(lexeme == "=" || lexeme == "==") {
		if(resolution_type == "string") {
			std::string s1 = lhs->toString();
			std::string s2 = rhs->toString();
			return s1 == s2;
			//return lhs->toString() == rhs->toString();
		}
		else if(resolution_type == "bool")
			return lhs->toBool() == rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() == rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() == rhs->toFloat();
	}	else if(lexeme == "!=") {

		if(resolution_type == "string")
			return lhs->toString() != rhs->toString();
		else if(resolution_type == "bool")
			return lhs->toBool() != rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() != rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() != rhs->toFloat();

	} else if(lexeme == "<") {

		if(resolution_type == "string")
			return lhs->toString() < rhs->toString();
		else if(resolution_type == "bool")
			return lhs->toBool() < rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() < rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() < rhs->toFloat();

	} else if(lexeme == "<=") {

		if(resolution_type == "string")
			return lhs->toString() <= rhs->toString();
		else if(resolution_type == "bool")
			return lhs->toBool() <= rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() <= rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() <= rhs->toFloat();

	} else if(lexeme == ">") {

		if(resolution_type == "string")
			return lhs->toString() > rhs->toString();
		else if(resolution_type == "bool")
			return lhs->toBool() > rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() > rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() > rhs->toFloat();

	} else if(lexeme == ">=") {

		if(resolution_type == "string")
			return lhs->toString() >= rhs->toString();
		else if(resolution_type == "bool")
			return lhs->toBool() >= rhs->toBool();
		else if(resolution_type == "int")
			return lhs->toInt() >= rhs->toInt();
		else if(resolution_type == "float")
			return lhs->toFloat() >= rhs->toFloat();

	} else {
		error_die("NodeDBExprOp::op_eval() unknown op lexeme:" + lexeme);
	}
}

LookupNode::LookupNode(const std::string &ilexeme)
	: Lexable("lookup_node",ilexeme)
{
//WHY ISNT IT FaILING ON UNFINISHED LOOKUP EVALUAGE????
}

std::string LookupNode::lookup()const
{
	std::map<std::string,std::string>::const_iterator b,e;
	get_csv_data_map_iterators(b,e);

	while(b!=e) {
		if(b->first == get_lexeme()) {
//std::cout << "FOUND LEXY" << std::endl;
			return b->second;
		}
		++b;
	}
	error_die("LookupNode::lookup() failed because lexeme missing from database: " + get_lexeme());
}

float LookupNode::toFloat()const
{
	std::string s = lookup();
	return atof(s.c_str());
}

int LookupNode::toInt()const
{
	std::string s = lookup();
	return atoi(s.c_str());
}

bool LookupNode::toBool()const
{
	std::string s = lookup();
	return bc::string_to_bool(s);
}


NodeDBExpr::NodeDBExpr(NodeDBVal *lhs,NodeDBExprOp *op,NodeDBVal *rhs)
	: BoolNode("NodeDBExpr","no_lexeme")
{
//std::cout << "NodeDBExpr::NodeDBExpr()" << std::endl;
	_left = lhs;
	_right = rhs;
	_op = op;
}

void NodeDBExpr::set_data(const std::map<std::string,std::string> &csv_data_map)
{
	//std::cout << "NodeDBExpr::set_data() ::::::: type:::" << get_type() << "     " << get_lexeme() << std::endl;

	//std::cout << "left::" << _left->get_type() << "     " << _left->get_lexeme() << std::endl;
	//std::cout << "right::" << _right->get_type() << "     " << _right->get_lexeme() << std::endl;

	_left->set_data(csv_data_map);
	_right->set_data(csv_data_map);
}

bool NodeDBExpr::evaluate()const
{
	return _op->op_eval(_left,_right);
}

DBNode::DBNode(const std::string &itype,const std::string &lexeme)
	: Lexable(itype,lexeme)
{
/*
	if(string_flag)
		_resolution_type = "string";

	if(get_lexeme().size()==0) // is it even possible to parse this?
		error_die("DBNode::DBNode() empty lexeme that is not flagged as string!");

	if(get_lexeme().find('.')==get_lexeme().size())
		_resolution_type = "float";
	else if(isdigit(get_lexeme()[0]))
		_resolution_type = "int";
	else
		error_die("DBNode::DBNode() could not find resolution_type");
*/
}


NodeDBVal::NodeDBVal(Lexable *child)
	: DBNode("nodedbval","nodedbval")
{
//std::cout << "NodeDBVal::NodeDBVal()" << std::endl;
//std::cout << "\tchild->get_lexeme(): " << child->get_lexeme() << std::endl;
	_child = child;
}

void NodeDBVal::set_data(const std::map<std::string,std::string> &csv_data_map)
{
	if(_child)
		_child->set_data(csv_data_map);
	else
		error_die("NodeDBVal::set_data() failed because _child is not set");
}


int NodeDBVal::toInt()const
{
	if(_child->get_type() == "number_node") {
		NumberNode *n = (NumberNode*)_child;
		return n->toInt();
	} else if(_child->get_type() == "lookup_node") {
		LookupNode *n = (LookupNode*)_child;
		return n->toInt();
	} else {
		error_die("NodeDBVal::toInt() do not know how to convert child node: " + _child->get_type());
	}
}

float NodeDBVal::toFloat()const
{
	if(_child->get_type() == "number_node") {
		NumberNode *n = (NumberNode*)_child;
		return n->toFloat();
	} else if(_child->get_type() == "lookup_node") {
		LookupNode *n = (LookupNode*)_child;
		return n->toFloat();
	} else {
		error_die("NodeDBVal::toFloat() do not know how to convert child node: " + _child->get_type());
	}
}

bool NodeDBVal::toBool()const
{
	if(_child->get_type() == "number_node") {
		NumberNode *n = (NumberNode*)_child;
		return n->toBool();
		return _child->get_lexeme() == "1";
	} else if(_child->get_type() == "lookup_node") {
		LookupNode *n = (LookupNode*)_child;
		return n->toBool();
	} else {
		error_die("NodeDBVal::toBool() do not know how to convert child node: " + _child->get_type());
	}
}

std::string NodeDBVal::toString()const
{
	std::string resolution_type = get_resolution_type();

	if(resolution_type == "string")
		return _child->get_lexeme();
	else if(resolution_type == "lookup")
		return ((LookupNode*)_child)->lookup();
}

std::string NodeDBVal::lookup()const
{
	error_die("NodeDBVal::lookup() CALL CHILD VERSION INSTEAD!");
/*
	std::string key = _child->get_lexeme();

	std::map<std::string,std::string>::const_iterator b,e;
	_child->get_csv_data_map_iterators(b,e);
	while(b!=e) {
		if(b->first == key)
			return b->second;
		++b;
	}

	error_die("NodeDBVal::lookup() could not find key: " + key);
*/
}

std::string NodeDBVal::get_resolution_type()const
{
	if(_child->get_type() == "string_node")
		return "string";
	else if(_child->get_type() == "lookup_node")
		return "lookup";
	else if(_child->get_type() == "number_node") {

		if(_child->get_lexeme().find('.')==_child->get_lexeme().size())
			return "float";
		else if(isdigit(_child->get_lexeme()[0]))
			return "int";
		else
			error_die("NodeDBVal::get_resolution_type() could not resolve numeric type.  Value: " + _child->get_lexeme());
	} else
		error_die("NodeDBVal::get_resolution_type() unknown child type: " + _child->get_type());
}



std::string get_resolution_type(const std::string &lhs,const std::string &rhs)
{
	std::string resolution_type;

	if(lhs=="string") {
		resolution_type = "string";

	} else if(lhs=="float") {
		resolution_type = "float";

	} else if(lhs=="bool") {
		resolution_type = "bool";

	} else if(lhs=="int") {
		resolution_type = "int";

	} else if(lhs=="lookup") {

		if(rhs=="string") {
			resolution_type = "string";

		} else if(rhs=="float") {
			resolution_type = "float";

		} else if(rhs=="bool") {
			resolution_type = "bool";

		} else if(rhs=="int") {
			resolution_type = "int";

		} else if(rhs=="lookup") {
			// if both are lookup, treat them as string
			resolution_type = "string";

		} else {
			std::cout << "cannot find resolution type___!" << std::endl;
			exit(1);
		}

	} else {
		std::cout << "cannot find resolution type_!" << std::endl;
		exit(1);
	}

	return resolution_type;
}

void error_die(const std::string &msg)
{
	std::cout << msg << std::endl;
	exit(1);
}

Stream::Stream(const std::string &s)
{
	_src = s;
	_index = 0;

	_whitespaces.push_back(" ");
	_whitespaces.push_back("\t");
	_whitespaces.push_back("\n");
	_whitespaces.push_back("\r"); // TODO this too?????
}

bool Stream::end_of_input()const
{
	if(_index >= _src.size())
		return true;
	return false;
}

char Stream::getnext()
{
	char ch = _src[_index++];
//std::cout << "getnext(): " << ch << std::endl;
//if(ch == ';')
//	std::cout << "D" << std::endl;
	return ch;
	//return _src[_index++];
}

void Stream::putback(const std::string &s)
{
	_index -= s.size();
	if(_index < 0) {
		error_die("putback() Deadly Error: negative ind");
	}
}

char Stream::parse_out_whitespace()
{
	char ch = getnext();
	while(bc::exist_in_vec(_whitespaces,bc::toString(ch)) && !end_of_input())
		ch = getnext();

	return ch;
}

std::string Stream::parse_only_these_chars(const std::string &allowed)
{
	char ch = getnext();
	std::string token = "";
	while(-1!=allowed.find(ch) && !end_of_input()) {
		token += bc::toString(ch);
		ch = getnext();
	}
	putback(bc::toString(ch));
	return token;
}

const std::vector<std::string>& Stream::get_whitespaces()const
{
	return _whitespaces;
}

std::string Stream::parse_until_terminator(const std::vector<std::string> &terminators)
{
	char ch = getnext();
	std::string token = "";
	int term_ind = -1;
	token += ch;
	while((term_ind = bc::exist_in_string(terminators,token))==-1) {
		token += getnext();
	}

	putback(terminators[term_ind]);

	token = token.substr(0,token.find(terminators[term_ind]));

	return token;
}

bool chain_test(const std::vector<Lexable*> &chain,const std::vector<std::string> &chain_tester)
{
	if(chain.size() < chain_tester.size())
		return false;

	for(int i=0;i<chain.size();++i) {
		if(false == bc::exist_in_vec(chain_tester,chain[i]->get_type()))
			return false;
	}
	return true;
}

TextNode* parse_text(Stream &stream,char ch)
{
	std::vector<std::string> db_expr_ops;
	db_expr_ops.push_back("=");
	db_expr_ops.push_back(">");
	db_expr_ops.push_back("<");
	db_expr_ops.push_back(">=");
	db_expr_ops.push_back("<=");

	std::vector<std::string> whitespaces = stream.get_whitespaces();

	std::vector<std::string> terminators;

	terminators = db_expr_ops;
	bc::append_to_vector(terminators,whitespaces);

	std::string token = bc::toString(ch);
	token += stream.parse_until_terminator(terminators);

	return new TextNode(token);// TODO MEM ALLOC
}

// This blindly copies the stream until the delim is found.
StringNode* parse_string(Stream &stream,const std::string &delim)
{
	std::vector<std::string> terminators;
	terminators.push_back(delim);
	std::string token = stream.parse_until_terminator(terminators);

	for(int i=0;i<delim.size();++i) // throw away the delimitor
		stream.getnext();

	return new StringNode(token); // TODO MEM ALLOC
}

NumberNode* parse_number(Stream &stream,char ch)
{
	std::string token = bc::toString(ch);
	token += stream.parse_only_these_chars("0123456789");
	return new NumberNode(token); // TODO MEM ALLOC
}

LookupNode* parse_lookup(Stream &stream,char ch)
{
	TextNode *n = parse_text(stream,ch);
	std::string token = n->get_lexeme();
	delete n;
	return new LookupNode(token); // TODO MEM ALLOC
}

NodeDBExprOp* lex_dbexprop(Stream &stream)
{
//std::cout << "lex_dbexprop()" << std::endl;

	char ch = stream.parse_out_whitespace();
	stream.putback(bc::toString(ch));

	std::string allowed = "=><";
//TODO need to add suppor tfor <= and >=
	std::string token = stream.parse_only_these_chars(allowed);

	std::vector<std::string> db_expr_ops;
	db_expr_ops.push_back("=");
	db_expr_ops.push_back(">");
	db_expr_ops.push_back("<");
	db_expr_ops.push_back(">=");
	db_expr_ops.push_back("<=");

	if(false == bc::exist_in_vec(db_expr_ops,token))
		error_die("lex_dbexprop() bad parse: " + token);

	return new NodeDBExprOp(token); // TODO MEM ALLOC
}

NodeDBVal* lex_dbval(Stream &stream)
{
	char ch = stream.parse_out_whitespace();

	if(ch == '\'' || ch == '\"') {
		std::string delim = bc::toString(ch);
		StringNode *n = parse_string(stream,delim);
		return new NodeDBVal(n); // TODO MEM ALLOC
	}

	if(isdigit(ch)) {
		NumberNode *n = parse_number(stream,ch);
		return new NodeDBVal(n); // TODO MEM ALLOC
	}

	LookupNode *n = parse_lookup(stream,ch);
	return new NodeDBVal(n); // TODO MEM ALLOC
}


NodeDBExpr* lex_dbexpr(Stream &stream)
{
//std::cout << "lex_dbexpr() begin" << std::endl;
	std::vector<Lexable*> chain;

	while(1) {

		if(chain.size()==3) {
//std::cout << "\tlex_dbexpr() chain.size()==3" << std::endl;
//std::cout << "\tlex_dbexpr() chain[0] type: " << chain[0]->get_type() << std::endl;
//std::cout << "\tlex_dbexpr() chain[1] type: " << chain[1]->get_type() << std::endl;
//std::cout << "\tlex_dbexpr() chain[2] type: " << chain[2]->get_type() << std::endl;

			std::vector<std::string> chain_tester;
			chain_tester.push_back("nodedbval");
			chain_tester.push_back("nodedbexprop");
			chain_tester.push_back("nodedbval");

			if(chain_test(chain,chain_tester)) {
				return new NodeDBExpr((NodeDBVal*)chain[0],(NodeDBExprOp*)chain[1],(NodeDBVal*)chain[2]); // TODO MEM ALLOC

			} else
				error_die("lex_dbexpr() bad chain");
		}

		if(chain.size()==0) {
//std::cout << "\tlex_dbexpr() chain.size()==0" << std::endl;

			NodeDBVal *n = lex_dbval(stream);
			chain.push_back(n);

		} else if(chain.size()==1) {
//std::cout << "\tlex_dbexpr() chain[0] lexeme: " << chain[0]->get_lexeme() << std::endl;
			NodeDBExprOp *op = lex_dbexprop(stream);
			chain.push_back(op);
//std::cout << "\tlex_dbexpr() chain[1] lexeme: " << chain[1]->get_lexeme() << std::endl;

		} else if(chain.size()==2) {

			NodeDBVal *n = lex_dbval(stream);
			chain.push_back(n);
		}

	}
}



BoolOp* lex_boolop(Stream &stream)
{
	char ch = stream.parse_out_whitespace();

	TextNode *text_node = parse_text(stream,ch);

	text_node->set_lexeme(bc::toLower(text_node->get_lexeme()));

	std::vector<std::string> bool_ops;
	bool_ops.push_back("or");
	bool_ops.push_back("and");

	if(false == bc::exist_in_vec(bool_ops,text_node->get_lexeme()))
		error_die("lex_boolop(): bad boolops");

	return new BoolOp(text_node->get_lexeme()); // TODO MEM ALLOC
}

BoolVal* lex_boolval(Stream &stream)
{
//std::cout << "lex_boolval() begin" << std::endl;

	std::vector<Lexable*> chain;

	while(!stream.end_of_input()) {

		char ch = stream.parse_out_whitespace();

		if(ch == ';') {
			if(chain.size()!=1)
				error_die("lex_boolval() incorrect chain size: " + bc::toString(chain.size()));

//std::cout << "valid termination" << std::endl;

			stream.putback(bc::toString(ch));
			return (BoolVal*)chain[0];

		} else if(ch == '(') {
			BoolVal *bv = lex_boolval(stream);
			chain.push_back(bv);


		} else if(ch == ')') {
			if(chain.size()!=1)
				error_die("lex_boolval2() bad chain length after parsing parenthesis");
			return (BoolVal*)chain[0];

		} else if(chain.size()) {

			stream.putback(bc::toString(ch));
			BoolOp *op = lex_boolop(stream);
			chain.push_back(op);
			BoolVal *bv = lex_boolval(stream);
			chain.push_back(bv);

			std::vector<std::string> chain_tester;
			chain_tester.push_back("BoolVal");
			chain_tester.push_back("BoolOp");
			chain_tester.push_back("BoolBal");

			if(false == chain_test(chain,chain_tester))
				error_die("lex_boolval() could not form voltron!");

			bv = new BoolVal((BoolVal*)chain[0],(BoolOp*)chain[1],(BoolVal*)chain[2]); // TODO MEM ALLOC HERE!!!
			chain.clear();
			chain.push_back(bv);

		} else {

			stream.putback(bc::toString(ch));
			NodeDBExpr *n = lex_dbexpr(stream);

			chain.push_back(new BoolVal((BoolNode*)n)); // TODO MEM ALLOC

		}

	}

	error_die("lex_boolval() impossible situation");

	return 0;
}

BoolVal* ParseExpression(const std::string &input_s)
{
//	std::cout << "input_s: " << input_s << std::endl;

	if(input_s.size()==0)
		return 0;

	BoolVal *bv;

	if(input_s[input_s.size()-1] != ';') {
		std::string s = input_s + ';';
		Stream stream(s);
		bv = lex_boolval(stream);
	} else {
		Stream stream(input_s);
		bv = lex_boolval(stream);
	}

	return bv;
}

WhereNode::WhereNode(BoolVal *bv)
	: Lexable("where_node","where_node")
{
	_boolval = bv;
}


bool WhereNode::evaluate(const std::map<std::string,std::string> &datamap)
{
	_boolval->set_data(datamap);
	return _boolval->evaluate();
}

FromNode::FromNode(const std::string &fromtxt)
	: Lexable("from_node",fromtxt)
{
}

std::string FromNode::get_tablename()const
{
	return get_lexeme();
}

SelectNode::SelectNode(const std::vector<std::string> &headerlist)
	: Lexable("select_node","select_node")
{
	_headerlist = headerlist;
	_from_node = 0;
	_where_node = 0;
}

bool SelectNode::valid()const
{
	return (_from_node!=0 && _where_node!=0 && _headerlist.size());
}

void SelectNode::set_from(FromNode *f)
{
	_from_node = f;
}

void SelectNode::set_where(WhereNode *w)
{
	_where_node = w;
}

void SelectNode::evaluate(std::map<std::string,std::string> data_map)const
{
	if(_headerlist.size()==0)
		return;


	if(_where_node->evaluate(data_map)) {

		std::string comma = "";

		if(_headerlist[0]=="*") {
			std::map<std::string,std::string>::const_iterator b,e;
			b = data_map.begin();
			e = data_map.end();
			while(b!=e) {
				std::cout << comma << b->second;
				comma = ",";
				++b;
			}
		} else {
			for(int i=0;i<_headerlist.size();++i) {
				std::cout << comma << data_map[_headerlist[i]];
				comma = ",";
			}
		}
		std::cout << std::endl;
	}
}

WhereNode* lex_select_where(Stream &stream)
{
//std::cout << "lex_select_where() begin" << std::endl;

	std::string token;

	char ch = stream.parse_out_whitespace();

	token = "";
	token += ch;

	std::vector<std::string> terminators;
	terminators.push_back(";");

	token += bc::toLower(stream.parse_until_terminator(terminators));
//std::cout << "where: " << token << std::endl;

	BoolVal *bv = ParseExpression(token);

	return new WhereNode(bv);
}

FromNode* lex_select_from(Stream &stream)
{
//std::cout << "lex_select_from() begin" << std::endl;

	std::string token;

	char ch = stream.parse_out_whitespace();

	token = "";
	token += ch;

	std::vector<std::string> terminators = stream.get_whitespaces();
	terminators.push_back(";");

	token += bc::toLower(stream.parse_until_terminator(terminators));

	return new FromNode(token);
}

std::vector<std::string> lex_select_headerlist(Stream &stream)
{
//std::cout << "lex_select_headerlist() begin" << std::endl;

	std::vector<std::string> ret;

	char ch = stream.parse_out_whitespace();

	if(ch == '*') {
		ret.push_back("*");
		return ret;
	} else if(ch == ',') {
		error_die("lex_select_headerlist() comma where it should not be");
	}

	std::string token = "";
	token += ch;
	token = bc::toLower(token);
	while(!stream.end_of_input()) {

		ch = stream.getnext();

			// this if terminates on from or where followed by whitespace
		std::string s = ""; s += ch;
		if(bc::exist_in_vec(stream.get_whitespaces(),s)) {

			if(token == "from" || token == "where") {
				stream.putback(token + ch);
//std::cout << "lex_select_headerlist() end" << std::endl;
				return ret;
			} else {
				// If we have no fear of termination then just skip the whitespace.
				// But save the token
				if(token.size()) {
					ret.push_back(token);
					token = "";
				}

				continue;
			}
		}

		// if we hit a comma, then save the token, and start over
		if(ch == ',') {
			ret.push_back(token);
			token = "";

		} else {
			// default to just building up the token
			std::string s = ""; s += ch;
			token += bc::toLower(s);
		}
	}

	// if we hit the end of input before encountering "from" or "where" then die
	error_die("lex_select_headerlist() reached impossible place");
}

SelectNode* lex_select(Stream &stream)
{
//std::cout << "lex_select() begin" << std::endl;

	std::vector<Lexable*> chain;
	while(!stream.end_of_input()) {
		char ch = stream.parse_out_whitespace();

		if(ch == ';') {
	//		if(chain.size()!=1)
				//error_die("lex_select() incorrect chain size: " + bc::toString(chain.size()));

			SelectNode *select = (SelectNode*)chain[0];

			if(select->valid()==false) {
				error_die("lex_select() invalid select node built, probably missing from or where");
			}

//std::cout << "lex_select() end" << std::endl;
			stream.putback(bc::toString(ch));
			return select;

		} else if(chain.size()==1) {

			std::string from_or_where = ch + bc::toLower(stream.parse_until_terminator(stream.get_whitespaces()));

			if(from_or_where == "from") {

				FromNode *f = lex_select_from(stream);
				((SelectNode*)chain[0])->set_from(f);

			} else if(from_or_where == "where") {

				WhereNode *w = lex_select_where(stream);
				((SelectNode*)chain[0])->set_where(w);

			} else {

//std::cout << "touch: "<< from_or_where  << std::endl;
				error_die("lex_select() could not find \"from\" or \"where\" where they belong");
			}

		} else {

			std::string select_text = ch + bc::toLower(stream.parse_until_terminator(stream.get_whitespaces()));
			if(select_text!="select") {
//std::cout << "SS: " << select_text << std::endl;
				error_die("lex_select() could not find \"select\" where it belongs.");
			}

			std::vector<std::string> header_list;
			header_list = lex_select_headerlist(stream);

			chain.push_back(new SelectNode(header_list));
		}
	}

	error_die("lex_select() impossible situation");

	return 0;
}


SelectNode* Parse(const std::string &input_s)
{
	//std::cout << "input_s: " << input_s << std::endl;

	if(input_s.size()==0)
		return 0;

	std::string s = input_s;
	if(s[s.size()-1]!=';')
		 s += ';';

	Stream stream(s);

	SelectNode *select = lex_select(stream);

	return select;
}


}; // namespace SqlParse


