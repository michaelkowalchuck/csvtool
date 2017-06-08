
// Michael Kowalchuck



#ifndef CSVTOOL_HH_HH_H
#define CSVTOOL_HH_HH_H

#include "bc_cc.h"

#include "sqlparse.h"

namespace csvtool {

class HeaderIndex
{
	public:
		HeaderIndex(const std::string &raw);

		bool is_header()const;
		int get_index()const;
		void set_index(int index);
		std::string get_header()const;

	private:

		std::string _header;
		int _index;

};


class Output
{
	public:
	Output();
	Output(const std::string &ofname);
	void append(const std::string &cell);
	void append(const std::vector<std::string> &cells);
	void newline();
	private:
	bc::File _file;

	bool _first_cell_of_line;
	std::string _delim;
};

class Input
{
	public:
	Input(const std::string &ifname,const std::string &delim);

	// returns false when we are done with input
	bool get_next_row(std::vector<std::string> &cells);
	void put_row_back(const std::vector<std::string> &cells);

	void close();
	void seek(int position);

	private:

	bc::File _file;
	std::vector<std::string> _buffered_cells;
	std::string _delim;
};

class Tool
{
	public:

	virtual void run() = 0;
	virtual ~Tool();

	private:
};

class Demo : public Tool
{
	public:
	Demo();
	void run();
	private:

};

class SimpleFileTool : public Tool
{
	public:

	SimpleFileTool();
	virtual void operate_on_input(Input &input,Output &output);
	void set_input(Input *input);
	void set_inputs(const std::vector<Input*> &inputs);
	const std::vector<Input*>& get_inputs()const;
	void set_output(Output *output);
	void run();
	virtual ~SimpleFileTool(); // TODO I remember something special about destructors being virtual.

	private:
		Input *_input; // these should have been malloc'd outside and passed in
		std::vector<Input*> _inputs; // same as these
		Output *_output; // they are delete()'d in destructor here
};

class SqlInsertBuilder : public SimpleFileTool
{
	public:
	SqlInsertBuilder(const std::string &table_name);
	void operate_on_input(Input &input,Output &output);
	private:

	std::string _table_name;
};

class ColCutter : public SimpleFileTool
{
	public:
	ColCutter(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	void set_trim_quotes(bool trim_quotes);
	private:
	bool _trim_quotes;
	std::vector<HeaderIndex> _indices;
};

class RowCutter : public SimpleFileTool
{
	public:
	RowCutter(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	private:

	std::vector<HeaderIndex> _indices;
};


class FindUniqueColValues : public SimpleFileTool
{
	public:
	FindUniqueColValues(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	private:

	std::vector<HeaderIndex> _indices;

	std::map<int,std::vector<std::string> > _unique_vals;

};

class FindUniqueRowValues : public SimpleFileTool
{
	public:
	FindUniqueRowValues(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	private:

	std::vector<HeaderIndex> _indices;

	std::map<int,std::vector<std::string> > _unique_vals;

};

class RmCol : public SimpleFileTool
{
	public:
	RmCol(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	private:

	std::vector<HeaderIndex> _indices;
};

class RmRow : public SimpleFileTool
{
	public:
	RmRow(const std::vector<HeaderIndex> &indices);
	void operate_on_input(Input &input,Output &output);
	private:

	std::vector<HeaderIndex> _indices;
};

class ColCount : public SimpleFileTool
{
	public:
	ColCount();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RowCount : public SimpleFileTool
{
	public:
	RowCount();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RemoveEmptyCols : public SimpleFileTool
{
	public:
	RemoveEmptyCols();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RemoveEmptyRows : public SimpleFileTool
{
	public:
	RemoveEmptyRows();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RemoveEmptyCellAtRowEnd: public SimpleFileTool
{
	public:
	RemoveEmptyCellAtRowEnd();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RemoveSurroundingQuotes: public SimpleFileTool
{
	public:
	RemoveSurroundingQuotes();
	void operate_on_input(Input &input,Output &output);
	private:
};


class JoinAnexttoB: public SimpleFileTool
{
	public:
	JoinAnexttoB();
	void operate_on_input(Input &input,Output &output);
	private:
};

class JoinAaboveB: public SimpleFileTool
{
	public:
	JoinAaboveB();
	void operate_on_input(Input &input,Output &output);
	private:
};

class Transpose: public SimpleFileTool
{
	public:
	Transpose();
	void operate_on_input(Input &input,Output &output);
	private:
};

class IsSquare: public SimpleFileTool
{
	public:
	IsSquare();
	void operate_on_input(Input &input,Output &output);
	private:
};

class InsertRow: public SimpleFileTool
{
	public:
	InsertRow(int index,int insert_cnt,int col_cnt);
	void operate_on_input(Input &input,Output &output);
	private:
		int _col_cnt;
		int _index;
		int _insert_cnt;
};

class InsertCol: public SimpleFileTool
{
	public:
	InsertCol(int index,int insert_cnt);
	void operate_on_input(Input &input,Output &output);
	private:
		int _index;
		int _insert_cnt;
};

class MoveRow: public SimpleFileTool
{
	public:
	MoveRow(int src_index,int dst_index);
	void operate_on_input(Input &input,Output &output);
	private:
		int _src_index;
		int _dst_index;
};

class MoveCol: public SimpleFileTool
{
	public:
	MoveCol(int src_index,int dst_index);
	void operate_on_input(Input &input,Output &output);
	private:
		int _src_index;
		int _dst_index;
};

class RotateRight: public SimpleFileTool
{
	public:
	RotateRight();
	void operate_on_input(Input &input,Output &output);
	private:
};

class RotateLeft: public SimpleFileTool
{
	public:
	RotateLeft();
	void operate_on_input(Input &input,Output &output);
	private:
};

class SqlCol: public SimpleFileTool
{
	public:
	SqlCol(const std::string &sqlcmd);
	void operate_on_input(Input &input,Output &output);
	private:

	SqlParse::SelectNode *_select;
	std::string _sqlcmd;
};

class SqlRow: public SimpleFileTool
{
	public:
	SqlRow(const std::string &sqlcmd);
	void operate_on_input(Input &input,Output &output);
	private:

	SqlParse::SelectNode *_select;
	std::string _sqlcmd;
};

csvtool::Tool* ToolFactory(int argc,char **argv);

}; // namespace csvtool




/* CSVTOOL_HH_HH_H */
#endif


