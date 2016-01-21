
#ifndef SQL_PARSE_CSV_TOOL_HH_HH_
#define SQL_PARSE_CSV_TOOL_HH_HH_

namespace SqlParse
{
	class Lexable
	{
		public:
			Lexable(const std::string &itype,const std::string ilexeme);
			std::string get_lexeme()const;
			void set_lexeme(const std::string &lexeme);

			std::string get_type()const;
			virtual void set_data(const std::map<std::string,std::string> &csv_data_map);

			void get_csv_data_map_iterators(
							std::map<std::string,std::string>::const_iterator &b,
							std::map<std::string,std::string>::const_iterator &e
						)const;

		private:
			std::string _type;
			std::string _lexeme;
			const std::map<std::string,std::string> *_csv_data_map;
	};

	class TextNode : public Lexable
	{
		public:
			TextNode(const std::string &ilexeme);
		private:
	};

	class StringNode : public Lexable
	{
		public:
			StringNode(const std::string &ilexeme);
		private:
	};

	class NumberNode : public Lexable
	{
		public:
			NumberNode(const std::string &ilexeme);

			float toFloat()const;
			int toInt()const;
			bool toBool()const;
		private:
	};

	class BoolNode : public Lexable
	{
		public:
			BoolNode(const std::string &itype,const std::string ilexeme);

			virtual bool evaluate()const=0;
		private:
	};

	class BoolOp;
	class BoolVal : public BoolNode
	{
		public:
			BoolVal(BoolVal *lhs,BoolOp *op,BoolVal *rhs);
			BoolVal(BoolNode *child);

			void set_data(const std::map<std::string,std::string> &csv_data_map);
			bool evaluate()const;
		private:
			BoolNode *_child;
			BoolVal *_left;
			BoolVal *_right;
			BoolOp *_op;
	};

	class BoolOp : public BoolNode
	{
		public:
			BoolOp(const std::string &lexeme);

			bool evaluate()const;
			bool op_eval(const BoolVal *lhs,const BoolVal *rhs)const;

		private:
	};



	class DBNode : public Lexable
	{
		public:
			DBNode(const std::string &itype,const std::string &lexeme);
		private:
	};




	class NodeDBVal : public DBNode
	{
		public:
			NodeDBVal(Lexable *child); // child should be LookupNode, StringNode, NumberNode or TextNode
			void set_data(const std::map<std::string,std::string> &csv_data_map);

			int toInt()const;
			float toFloat()const;
			bool toBool()const;
			std::string toString()const;
			std::string lookup()const;
			std::string get_resolution_type()const;

		private:
			Lexable *_child;
	};

	class LookupNode : public Lexable
	{
		public:
			LookupNode(const std::string &ilexeme);

			std::string lookup()const;
			float toFloat()const;
			int toInt()const;
			bool toBool()const;

		private:
	};


	class NodeDBExprOp : public DBNode
	{
		public:
			NodeDBExprOp(const std::string &lexeme);
			bool op_eval(const NodeDBVal *lhs,const NodeDBVal *rhs)const;
		private:
	};

	class NodeDBExpr : public BoolNode
	{
		public:
			NodeDBExpr(NodeDBVal *lhs,NodeDBExprOp *op,NodeDBVal *rhs);

			void set_data(const std::map<std::string,std::string> &csv_data_map);
			bool evaluate()const;

		private:

			NodeDBVal *_left;
			NodeDBVal *_right;
			NodeDBExprOp *_op;
	};


	std::string get_resolution_type(const std::string &lhs,const std::string &rhs);
	void error_die(const std::string &msg);

	class Stream
	{
		public:
			Stream(const std::string &input_s);

			bool end_of_input()const;
			char getnext();
			void putback(const std::string &s);

			char parse_out_whitespace();
			std::string parse_only_these_chars(const std::string &allowed);

			const std::vector<std::string>& get_whitespaces()const;
			std::string parse_until_terminator(const std::vector<std::string> &terminators);
		private:
			std::string _src;
			int _index;

			std::vector<std::string> _whitespaces;
	};



	class WhereNode : public Lexable
	{
		public:
			WhereNode(BoolVal *bv);

			bool evaluate(const std::map<std::string,std::string> &datamap);

		private:
			BoolVal *_boolval;
	};

	class FromNode : public Lexable
	{
		public:
			FromNode(const std::string &fromtxt);

			std::string get_tablename()const;
		private:
	};

	class SelectNode : public Lexable
	{
		public:
			SelectNode(const std::vector<std::string> &headerlist);

			bool valid()const;
			void set_from(FromNode *f);
			void set_where(WhereNode *w);

			void evaluate(std::map<std::string,std::string> data_map)const;

		private:
			FromNode *_from_node;
			WhereNode *_where_node;
			std::vector<std::string> _headerlist;
	};


	bool chain_test(const std::vector<Lexable*> &chain,const std::vector<std::string> &chain_tester);
	TextNode* parse_text(Stream &stream,char ch);

	StringNode* parse_string(Stream &stream,const std::string &delim);
	NumberNode* parse_number(Stream &stream,char ch);
	LookupNode* parse_lookup(Stream &stream,char ch);

	NodeDBVal* lex_dbval(Stream &stream);
	NodeDBExpr* lex_dbexpr(Stream &stream);
	BoolOp* lex_boolop(Stream &stream);
	BoolVal* lex_boolval(Stream &stream);

	BoolVal* ParseExpression(const std::string &input_s);


	WhereNode* lex_select_where(Stream &stream);
	FromNode* lex_select_from(Stream &stream);
	std::vector<std::string> lex_select_headerlist(Stream &stream);
	SelectNode* lex_select(Stream &stream);

	SelectNode* Parse(const std::string &input_s);

};



// SQL_PARSE_CSV_TOOL_HH_HH_
#endif

