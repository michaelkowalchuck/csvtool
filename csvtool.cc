
// Michael Kowalchuck



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


#include "csvtool.h"
#include "sqlparse.h"

/*

g++ `basecode-config --ccflags --cclibs` csvtool.cc -o csvtool

	-- for some reason you cannot combine --ccflags and --cclibs like
			that with gcc, g++ doesnt care though.

	-- for gcc usage, you need to do this
gcc `basecode-config --cflags` csv_get_col.c `basecode-config --clibs` -o csv_get_col

*/




namespace csvtool {

HeaderIndex::HeaderIndex(const std::string &raw)
{
	if(bc::is_integer(raw)) {

		_index = atoi(raw.c_str());
		_header = "";

	} else {

		_index = -1;
		_header = raw;

	}

}

bool HeaderIndex::is_header()const
{
	return _index==-1;
}

int HeaderIndex::get_index()const
{
	return _index;
}
void HeaderIndex::set_index(int index)
{
	_index = index;
}

std::string HeaderIndex::get_header()const
{
	return _header;
}


Output::Output()
{
	_delim = ",";
	_first_cell_of_line = true;
}

Output::Output(const std::string &ofname):_file(ofname)
{
	_file.open_for_writing_or_die();
	_delim = ",";
	_first_cell_of_line = true;
}

void Output::append(const std::string &cell)
{
	if(_first_cell_of_line) {
		_first_cell_of_line = false;
	} else {
		if(!_file.is_open())
			std::cout << _delim;
		else
			_file.write(_delim);
	}

	if(!_file.is_open())
		std::cout << cell;
	else
		_file.write(cell);
}

void Output::append(const std::vector<std::string> &cells)
{
	for(int i=0;i<cells.size();++i) {
		append(cells[i]);
	}
}


void Output::newline()
{
	_first_cell_of_line = true;
	if(!_file.is_open())
		std::cout << std::endl;
	else
		_file.write("\n");
}


Input::Input(const std::string &ifname):_file(ifname)
{
	_delim = ",";
	_file.open_readonly_or_die();
}

bool Input::get_next_row(std::vector<std::string> &cells)
{
	cells.clear();
	if(_buffered_cells.size()!=0) {
		bc::copy_vec(cells,_buffered_cells);
		_buffered_cells.resize(0);
		return true;
	}


	if(_file.is_open()) {

		std::string rawline;
		bool v = _file.getline(rawline);
		bc::tokenize(cells,rawline,_delim);
		return v;
	} else {

		std::cout << "Input::get_next_row() can only handle files for now!" << std::endl;
		exit(1);

	}

}

void Input::put_row_back(const std::vector<std::string> &cells)
{
	if(_buffered_cells.size()!=0) {
		std::cout << "Input::put_row_back() already have a buffered row, cannot put back twice" << std::endl;
		exit(1);
	}
	bc::copy_vec(_buffered_cells,cells);
}

void Input::close()
{
	_file.close();
}

void Input::seek(int position)
{
	_file.seek(position,bc::File::beg);
}

Tool::~Tool()
{

}

//////////////////////////////////////////////////////////////////////////////
// Below here we have class implementations for our Tool classes
//////////////////////////////////////////////////////////////////////////////


Demo::Demo()
{
}

void Demo::run()
{
	std::string s = "";

	s += "csvtool.exe col sql \"select * from tt.csv where a=1;\" tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool.exe col count tt.csv";
	s += "\n";
	s += "csvtool.exe row count tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool.exe col remove_empty tt.csv";
	s += "\n";
	s += "csvtool.exe row remove_empty tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col cut 3,2 tt.csv";
	s += "\n";
	s += "csvtool row cut 3,2 tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col insert 1 2 tt.csv";
	s += "\n";
	s += "csvtool row insert 1 2 tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col remove_empty tt.csv";
	s += "\n";
	s += "csvtool row remove_empty tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col unique_values 0,2 tt.csv";
	s += "\n";
	s += "csvtool row unique_values 0,2 tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col rm 0,2 tt.csv";
	s += "\n";
	s += "csvtool row rm 5 tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool col mv 0 2 tt.csv";
	s += "\n";
	s += "csvtool row mv 0 2 tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool rm_trailing_commas tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool transpose tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool join AaboveB a.csv b.csv [fileC.csv]";
	s += "\n";
	s += "csvtool join AnexttoB a.csv b.csv [fileC.csv]";
	s += "\n";
	s += "\n";
	s += "csvtool rotate_right tt.csv";
	s += "\n";
	s += "csvtool rotate_left tt.csv";
	s += "\n";
	s += "\n";
	s += "csvtool is_square tt.csv";
	s += "\n";
	s += "\n";


	std::cout << s << std::endl;


}

SimpleFileTool::SimpleFileTool()
{
	_input = 0;
	_output = 0;
}

SimpleFileTool::~SimpleFileTool()
{
// TODO close the input???
	delete _input;
	for(int i=0;i<_inputs.size();++i) {
		delete _inputs[i];
	}
	delete _output;
}

void SimpleFileTool::set_input(Input *input)
{
	_input = input;
}

void SimpleFileTool::set_inputs(const std::vector<Input*> &inputs)
{
	_inputs = inputs;
}

const std::vector<Input*>& SimpleFileTool::get_inputs()const
{
	return _inputs;
}

void SimpleFileTool::set_output(Output *output)
{
	_output = output;
}


void SimpleFileTool::operate_on_input(Input &input,Output &output)
{
	std::cout << "SimpleFileTool::operate_on_input() cannot be called from base class!" << std::endl;
	exit(1);
}

void SimpleFileTool::run()
{
	operate_on_input(*_input,*_output);
}

SqlInsertBuilder::SqlInsertBuilder(const std::string &table_name)
{
	_table_name = table_name;
}

void SqlInsertBuilder::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> header_cells;
	input.get_next_row(header_cells);

	std::vector<std::string> cells;

	std::string sqlcmd0 = "insert into " + _table_name + " values(";
	std::string sqlcmd2 = ");";

	while(input.get_next_row(cells)) {

		if(cells.size()==0)
			continue;

		if(cells.size()!=header_cells.size()) {
			std::cout << std::endl;
			std::cout << "SqlInsertBuilder::operate_on_input() unequal cell counts" << std::endl;
			std::cout << "\theader_cells.size(): " << header_cells.size() << std::endl;
			std::cout << "\tcells.size(): " << cells.size() << std::endl;
			std::cout << std::endl;
			exit(1);
		}

		std::string sqlcmd1 = "";
		for(int i=0;i<header_cells.size();++i) {
			sqlcmd1 += "\"" + cells[i] + "\"";
			if(i<header_cells.size()-1)
				sqlcmd1 += ",";
		}

		output.append(sqlcmd0 + sqlcmd1 + sqlcmd2);

		output.newline();

	}

}


ColCutter::ColCutter(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void ColCutter::operate_on_input(Input &input,Output &output)
{
	int max_ind = -1;

	std::vector<std::string> header_cells;
	input.get_next_row(header_cells);

	for(int i=0;i<_indices.size();++i) {

		if(_indices[i].is_header()) {
			for(int j=0;j<header_cells.size();++j) {

				if(header_cells[j] == _indices[i].get_header()) {
					_indices[i].set_index(j);
					if(j > max_ind)
						max_ind = j;
				}

			}
		} else {
			if(_indices[i].get_index() > max_ind)
				max_ind = _indices[i].get_index();
		}

	}

	input.put_row_back(header_cells);

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		if(max_ind < cells.size()) {

			for(int i=0;i<_indices.size();++i) {

				output.append(cells[_indices[i].get_index()]);

			}
		}

		output.newline();

	}

}

RowCutter::RowCutter(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void RowCutter::operate_on_input(Input &input,Output &output)
{
	int row_ind = 0;

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		for(int i=0;i<_indices.size();++i) {

			if(_indices[i].is_header() && cells.size()>0) {
				if(cells[0] == _indices[i].get_header()) {
					for(int j=0;j<cells.size();++j) {
						output.append(cells[j]);
					}
					output.newline();
				}
			} else if(_indices[i].get_index() == row_ind) {

				for(int j=0;j<cells.size();++j) {
					output.append(cells[j]);
				}
				output.newline();
			}
		}

		++row_ind;

	}

}


FindUniqueColValues::FindUniqueColValues(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void FindUniqueColValues::operate_on_input(Input &input,Output &output)
{
	int max_ind = -1;

	std::vector<std::string> header_cells;
	input.get_next_row(header_cells);



	for(int i=0;i<_indices.size();++i) {

		if(_indices[i].is_header()) {
			for(int j=0;j<header_cells.size();++j) {

				if(header_cells[j] == _indices[i].get_header()) {
					_indices[i].set_index(j);
					_unique_vals[_indices[i].get_index()] = std::vector<std::string>();
					if(j > max_ind)
						max_ind = j;
				}

			}
		} else {
			if(_indices[i].get_index() > max_ind)
				max_ind = _indices[i].get_index();
		}

	}

	input.put_row_back(header_cells);

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		if(max_ind < cells.size()) {

			for(int i=0;i<_indices.size();++i) {
				bc::append_unique(_unique_vals[_indices[i].get_index()],cells[_indices[i].get_index()]);

			}

		}

	}

	for(int i=0;i<_indices.size();++i) {
		int index = _indices[i].get_index();

		for(int j=0;j<_unique_vals[index].size();++j) {
			output.append(_unique_vals[index][j]);
		}
		output.newline();
	}

}

FindUniqueRowValues::FindUniqueRowValues(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void FindUniqueRowValues::operate_on_input(Input &input,Output &output)
{
	int row_ind = 0;

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		for(int i=0;i<_indices.size();++i) {

			if(_indices[i].is_header() && cells.size()>0) {
				if(cells[0] == _indices[i].get_header()) {
					for(int j=0;j<cells.size();++j) {
						bc::append_unique(_unique_vals[_indices[i].get_index()],cells[j]);
					}
				}
			} else if(_indices[i].get_index() == row_ind) {

				for(int j=0;j<cells.size();++j) {
					bc::append_unique(_unique_vals[_indices[i].get_index()],cells[j]);
				}
			}
		}

		++row_ind;

	}

	for(int i=0;i<_indices.size();++i) {
		int index = _indices[i].get_index();

		for(int j=0;j<_unique_vals[index].size();++j) {
			output.append(_unique_vals[index][j]);
		}
		output.newline();
	}

}


RmCol::RmCol(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void RmCol::operate_on_input(Input &input,Output &output)
{
	int max_ind = -1;

	std::vector<std::string> header_cells;
	input.get_next_row(header_cells);



	for(int i=0;i<_indices.size();++i) {

		if(_indices[i].is_header()) {
			for(int j=0;j<header_cells.size();++j) {

				if(header_cells[j] == _indices[i].get_header()) {
					_indices[i].set_index(j);
					if(j > max_ind)
						max_ind = j;
				}

			}
		} else {
			if(_indices[i].get_index() > max_ind)
				max_ind = _indices[i].get_index();
		}

	}

	input.put_row_back(header_cells);

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		for(int i=0;i<cells.size();++i) {
			bool skip_this_cell = false;
			for(int j=0;j<_indices.size();++j) {
				if(_indices[j].get_index()==i) {
					skip_this_cell = true;
					break;
				}
			}
			if(!skip_this_cell) {
				output.append(cells[i]);
			}
		}
		output.newline();
	}
}



RmRow::RmRow(const std::vector<HeaderIndex> &indices)
{
	_indices = indices;
}

void RmRow::operate_on_input(Input &input,Output &output)
{
	int row_ind = 0;

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		bool skip_this_row = false;

		for(int i=0;i<_indices.size();++i) {

			if(_indices[i].is_header() && cells.size()>0) {
				if(cells[0] == _indices[i].get_header()) {
					skip_this_row = true;
					break;
				}
			} else if(_indices[i].get_index()==row_ind) {
				skip_this_row = true;
				break;
			}
		}
		if(!skip_this_row) {
			for(int i=0;i<cells.size();++i) {
				output.append(cells[i]);
			}
			output.newline();
		}

		++row_ind;
	}
}

ColCount::ColCount()
{
}

void ColCount::operate_on_input(Input &input,Output &output)
{
	int max_col = 0;

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		if(cells.size() > max_col) {
			if(cells.size()==1) {
				if(cells[0]=="")
					continue;
			}
			max_col = cells.size();
		}

	}
	output.append(bc::toString(max_col));
	output.newline();
}

RowCount::RowCount()
{
}

void RowCount::operate_on_input(Input &input,Output &output)
{
	int row_ind = -1;

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		++row_ind;
	}
	output.append(bc::toString(row_ind));
	output.newline();
}


RemoveEmptyCols::RemoveEmptyCols()
{
}

void RemoveEmptyCols::operate_on_input(Input &input,Output &output)
{
	std::vector<int> empty_cols;

	std::vector<std::string> cells;

	input.get_next_row(cells);

	for(int i=0;i<cells.size();++i) {
		if(cells[i]=="") {
			empty_cols.push_back(i);
		}
	}

	while(input.get_next_row(cells)) {

		for(int i=0;i<cells.size();++i) {
			if(cells[i]!="") {

				// find i in empty_cols and remove it
				std::vector<int>::iterator b;
				b = std::find(empty_cols.begin(),empty_cols.end(),i);
				if(b != empty_cols.end()) {
					empty_cols.erase(b);
				}

			}
		}
	}

	input.seek(0);

	while(input.get_next_row(cells)) {

		for(int i=0;i<cells.size();++i) {

			if(empty_cols.end()==std::find(empty_cols.begin(),empty_cols.end(),i)) {
				output.append(cells[i]);
			}

		}
		output.newline();
	}

}

RemoveEmptyRows::RemoveEmptyRows()
{

}

void RemoveEmptyRows::operate_on_input(Input &input,Output &output)
{

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		if(cells.size()) {
			for(int i=0;i<cells.size();++i) {
				output.append(cells[i]);
			}
			output.newline();
		}
	}

}


RemoveEmptyCellAtRowEnd::RemoveEmptyCellAtRowEnd()
{

}

void RemoveEmptyCellAtRowEnd::operate_on_input(Input &input,Output &output)
{

	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		int index = cells.size()-1;
		while(cells[index]=="") {
			cells.erase(cells.begin()+index);
			index = cells.size()-1;
			if(cells.size()==0)
				break;
		}

		for(int i=0;i<cells.size();++i) {
			output.append(cells[i]);
		}
		output.newline();

	}

}


JoinAnexttoB::JoinAnexttoB()
{
}

void JoinAnexttoB::operate_on_input(Input &input_b,Output &output)
{
	std::vector<std::string> cells_a;

	bool done = false;
	while(!done) {

		for(int i=0;i<get_inputs().size();++i) {
			bool av = get_inputs()[i]->get_next_row(cells_a);
			if(av) {
				for(int j=0;j<cells_a.size();++j) {
					output.append(cells_a[j]);
				}
			} else {
				done = true;
			}
		}
	}
}


JoinAaboveB::JoinAaboveB()
{
}

void JoinAaboveB::operate_on_input(Input &input_b,Output &output)
{
	std::vector<std::string> cells;

	for(int i=0;i<get_inputs().size();++i) {
		while(get_inputs()[i]->get_next_row(cells)) {

			for(int j=0;j<cells.size();++j) {
				output.append(cells[j]);
			}
			output.newline();
		}
	}
}


Transpose::Transpose()
{

}

void Transpose::operate_on_input(Input &input,Output &output)
{
	std::vector<std::vector<std::string> > csv_data;
	std::vector<std::string> cells;

	int max_row = 0;
	int max_col = 0;

	while(input.get_next_row(cells)) {
		csv_data.push_back(cells);
		if(cells.size()>max_col)
			max_col = cells.size();
		++max_row;
	}

	for(int i=0;i<max_col;++i) {

		for(int j=0;j<max_row;++j) {

			if(csv_data.size() > j) {
				if(csv_data[j].size() > i) {
					output.append(csv_data[j][i]);
				}
			}

		}
		output.newline();
	}

}

IsSquare::IsSquare()
{

}

void IsSquare::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;

	int max_row = 0;
	int max_col = 0;

	bool v = input.get_next_row(cells);

	if(!v) {
		output.append(bc::toString("1"));
		output.newline();
		return;
	}

	++max_row;
	max_col = cells.size();

	while(input.get_next_row(cells)) {

		if(cells.size()!=max_col) {
			output.append(bc::toString("0"));
			output.newline();
			return;
		}
		++max_row;
	}

	if(max_row!=max_col) {
		output.append(bc::toString("0"));
		output.newline();
		return;
	}

	output.append(bc::toString("1"));
	output.newline();

}


InsertRow::InsertRow(int index,int insert_cnt,int col_cnt)
{
	_index = index;
	_insert_cnt = insert_cnt;
	_col_cnt = col_cnt;
}

void InsertRow::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;

	int row_ind = 0;

	// if we are prepending to an empty file
	if(0 == _index) {
		for(int m=0;m<_insert_cnt;++m) {
			for(int i=0;i<_col_cnt;++i) {
				output.append("");
			}
			output.newline();
		}
	}

	if(input.get_next_row(cells)) {
		output.append(cells);
		output.newline();
	}
	++row_ind;

	while(input.get_next_row(cells)) {

		if(row_ind == _index) {
			for(int m=0;m<_insert_cnt;++m) {
				for(int i=0;i<_col_cnt;++i) {
					output.append("");
				}
				output.newline();
			}
		}

		output.append(cells);
		output.newline();

		++row_ind;
	}

	// if we just want to append one row to the end
	if(row_ind == _index) {
		for(int m=0;m<_insert_cnt;++m) {
			for(int i=0;i<_col_cnt;++i) {
				output.append("");
			}
			output.newline();
		}
	}
}

InsertCol::InsertCol(int index,int insert_cnt)
{
	_index = index;
	_insert_cnt = insert_cnt;
}

void InsertCol::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;

	while(input.get_next_row(cells)) {

		if(cells.size()<_index) {
			output.append(cells);
		} else {

			for(int i=0;i<_index;++i) {
				output.append(cells[i]);
			}
			for(int i=0;i<_insert_cnt;++i) {
				output.append("");
			}
			for(int i=_index;i<cells.size();++i) {
				output.append(cells[i]);
			}
		}

		output.newline();
	}
}


MoveRow::MoveRow(int src_index,int dst_index)
{
	_src_index = src_index;
	_dst_index = dst_index;
}

void MoveRow::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;
	std::vector<std::string> src_cells;
	std::vector<std::vector<std::string> > saved_cells;

		// just print everyting out since nothing happens
	if(_src_index == _dst_index) {
		while(input.get_next_row(cells)) {
			output.append(cells);
			output.newline();
		}
		return;
	}

	int row_ind = 0;

	if(_src_index < _dst_index) {
		while(input.get_next_row(cells)) {

			if(row_ind == _src_index) {
				bc::copy_vec(src_cells,cells);
			} else if(row_ind == _dst_index) {
				output.append(src_cells);
				output.newline();
				output.append(cells);
				output.newline();
			} else {
				output.append(cells);
				output.newline();
			}

			++row_ind;
		}

			// move to the bottom of the file here
		if(row_ind == _dst_index) {
			output.append(src_cells);
			output.newline();
		}


	} else {
		while(input.get_next_row(cells)) {

			if(row_ind >= _dst_index && row_ind < _src_index) {
				saved_cells.push_back(std::vector<std::string>());
				bc::copy_vec(saved_cells[saved_cells.size()-1],cells);
			} else if(row_ind == _src_index) {
				output.append(cells);
				output.newline();
				for(int i=0;i<saved_cells.size();++i) {
					output.append(saved_cells[i]);
					output.newline();
				}
				saved_cells.clear();
			} else {
				output.append(cells);
				output.newline();
			}

			++row_ind;
		}

			// If the src is larger than the size of the file, just print out
			// everything saved because we could not do the move.
		if(saved_cells.size()!=0) {
			for(int i=0;i<saved_cells.size();++i) {
				output.append(saved_cells[i]);
				output.newline();
			}
		}

	}
}


MoveCol::MoveCol(int src_index,int dst_index)
{
	_src_index = src_index;
	_dst_index = dst_index;
}

void MoveCol::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;
	std::vector<std::string> saved_cells;
	std::string src_cell;

	while(input.get_next_row(cells)) {

		if(_src_index == _dst_index) {
			output.append(cells);
		} else if(_src_index < _dst_index) {

			for(int i=0;i<cells.size();++i) {

				if(i==_src_index) {
					src_cell = cells[i];
				} else if(i==_dst_index) {
					output.append(src_cell);
					output.append(cells[i]);
				} else {
					output.append(cells[i]);
				}

			}
				// if we want to move to the last column
			if(_dst_index == cells.size()) {
				output.append(src_cell);
			}

		} else if(_src_index > _dst_index) {
			for(int i=0;i<cells.size();++i) {

				if(i >= _dst_index && i < _src_index) {
					saved_cells.push_back(cells[i]);
				} else if(i == _src_index) {
					output.append(cells[i]);
					for(int i=0;i<saved_cells.size();++i) {
						output.append(saved_cells[i]);
					}
					saved_cells.clear();
				} else {
					output.append(cells[i]);
				}

			}
			if(saved_cells.size()!=0) {
				for(int i=0;i<saved_cells.size();++i) {
					output.append(saved_cells[i]);
				}
			}

		}

		output.newline();
	}
}

RotateRight::RotateRight()
{

}

void RotateRight::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;
	std::vector<std::vector<std::string> > csv_cells;

	while(input.get_next_row(cells)) {
		csv_cells.push_back(cells);
	}

	int max_col = 0;

	for(int i=0;i<csv_cells.size();++i) {
		if(csv_cells.size() > max_col)
			max_col = csv_cells.size();
	}

	for(int i=0;i<max_col;++i) {

		for(int j=0;j<csv_cells.size();++j) {

			if(csv_cells[csv_cells.size()-1-j].size() <= i) {
				output.append("");
			} else {
				output.append(csv_cells[csv_cells.size()-1-j][i]);
			}

		}
		output.newline();

	}

}


RotateLeft::RotateLeft()
{

}

void RotateLeft::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;
	std::vector<std::vector<std::string> > csv_cells;

	while(input.get_next_row(cells)) {
		csv_cells.push_back(cells);
	}

	int max_col = 0;

	for(int i=0;i<csv_cells.size();++i) {
		if(csv_cells.size() > max_col)
			max_col = csv_cells.size();
	}

	for(int i=0;i<max_col;++i) {

		for(int j=0;j<csv_cells.size();++j) {

			if(csv_cells[j].size() <= max_col-1-i) {
				output.append("");
			} else {
				output.append(csv_cells[j][max_col-1-i]);
			}

		}
		output.newline();

	}

}


SqlCol::SqlCol(const std::string &sqlcmd)
{
	_sqlcmd = sqlcmd;
	_select = 0;
	_select = SqlParse::Parse(_sqlcmd);
}

void SqlCol::operate_on_input(Input &input,Output &output)
{
	std::vector<std::string> cells;
	std::vector<std::string> header_cells;
	input.get_next_row(header_cells);

	std::vector<int> selected_col_indices;

	std::map<std::string,int> header_index_map;

	for(int i=0;i<header_cells.size();++i)
		header_index_map[header_cells[i]] = i;


	while(input.get_next_row(cells)) {

		std::map<std::string,std::string> csv_data_map;
		bc::vectors_to_map(csv_data_map,header_cells,cells);

		_select->evaluate(csv_data_map);

	}

}


SqlRow::SqlRow(const std::string &sqlcmd)
{
	_sqlcmd = sqlcmd;
	_select = 0;
	_select = SqlParse::Parse(_sqlcmd);
}

void SqlRow::operate_on_input(Input &input,Output &output)
{
}









csvtool::Tool* ToolFactory(int argc,char **argv)
{
	csvtool::Tool *tool;

	csvtool::Output *output = new csvtool::Output(); // MALLOC HERE

	std::vector<std::string> params;
	for(int i=1;i<argc;++i) {
		params.push_back(argv[i]);
	}

	if(params.size()==0 || params[0] == "demo") {

		return new csvtool::Demo();

	} else if(params[0] == "sql_insert") {

		if(params.size()!=2) {
			std::cout << "not enough params here." << std::endl;
			exit(1);
		}

		std::string table_name = params[1];

		return new csvtool::SqlInsertBuilder(table_name);

	} else if(params[0] == "row" || params[0] == "col") {

		if(params.size()<2) {
			std::cout << "not enough params here." << std::endl;
			exit(1);
		}

		std::string ifname = params[params.size()-1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		if(params[1] == "count") {

			if(params[0]=="col") {
				csvtool::ColCount *t = new csvtool::ColCount();
				t->set_input(input);
				t->set_output(output);
				return t;
			} else {
				csvtool::RowCount *t = new csvtool::RowCount();
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "remove_empty") {

			if(params.size()!=3) {
				std::cout << "incorrect number of params" << std::endl;
				exit(1);
			}

			if(params[0] == "col") {
				csvtool::RemoveEmptyCols *t = new csvtool::RemoveEmptyCols();
				t->set_input(input);
				t->set_output(output);
				return t;

			} else {
				csvtool::RemoveEmptyRows *t = new csvtool::RemoveEmptyRows();
				t->set_input(input);
				t->set_output(output);
				return t;

			}
		}

		if(params.size()<3) {
			std::cout << "not enough params here" << std::endl;
			exit(1);
		}

		if(params[1] == "cut") {

			std::vector<std::string> toks;
			bc::tokenize(toks,params[2],",");

			std::vector<HeaderIndex> indices;

			for(int j=0;j<toks.size();++j) {
				indices.push_back(HeaderIndex(toks[j]));
			}

			if(params[0] == "col") {
				csvtool::ColCutter *t = new csvtool::ColCutter(indices);
				t->set_input(input);
				t->set_output(output);
				return t;

			} else {
				csvtool::RowCutter *t = new csvtool::RowCutter(indices);
				t->set_input(input);
				t->set_output(output);
				return t;

			}

		} else if(params[1] == "insert") {


			if(params[0]=="col") {

				if(params.size()!=5) {
					std::cout << "not enough params here" << std::endl;
					std::cout << "insert index insert_count" << std::endl;
					exit(1);
				}

				int index = atoi(params[2].c_str());
				int insert_cnt = atoi(params[3].c_str());

				csvtool::InsertCol *t = new csvtool::InsertCol(index,insert_cnt);
				t->set_input(input);
				t->set_output(output);
				return t;

			} else {

				if(params.size()!=5) {
					std::cout << "not enough params here" << std::endl;
					std::cout << "insert index insert_count col_count" << std::endl;
					exit(1);
				}

				int index = atoi(params[2].c_str());
				int insert_cnt = atoi(params[3].c_str());
				int col_cnt = atoi(params[4].c_str());

				csvtool::InsertRow *t = new csvtool::InsertRow(index,insert_cnt,col_cnt);
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "remove_outer_empty") {
		} else if(params[1] == "unique_values") {
			std::vector<std::string> toks;
			bc::tokenize(toks,params[2],",");

			std::vector<HeaderIndex> indices;

			for(int j=0;j<toks.size();++j) {
				indices.push_back(HeaderIndex(toks[j]));
			}

			if(params[0] == "col") {
				csvtool::FindUniqueColValues *t = new csvtool::FindUniqueColValues(indices);
				t->set_input(input);
				t->set_output(output);
				return t;
			} else {
				csvtool::FindUniqueRowValues *t = new csvtool::FindUniqueRowValues(indices);
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "sql") {

			if(params.size()!=4) {
				std::cout << "sql \"select blah from blah where blah;\" file.csv" << std::endl;
				exit(1);
			}
/*
DDDD

-- put in a message that you must have same number of columns in each row as you have header cols
	-- maybe skip those rows?

DDDD
DDDD
*/
			std::string sqlcmd = params[2];

			if(params[0] == "col") {
				csvtool::SqlCol *t = new csvtool::SqlCol(sqlcmd);
				t->set_input(input);
				t->set_output(output);
				return t;
			} else {
				csvtool::SqlRow *t = new csvtool::SqlRow(sqlcmd);
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "rm") {

			if(params.size()!=4) {
				std::cout << "rm col1[,col2,col3] file.csv" << std::endl;
				exit(1);
			}

			std::vector<std::string> toks;
			bc::tokenize(toks,params[2],",");

			std::vector<HeaderIndex> indices;

			for(int j=0;j<toks.size();++j) {
				indices.push_back(HeaderIndex(toks[j]));
			}

			if(params[0] == "col") {
				csvtool::RmCol *t = new csvtool::RmCol(indices);
				t->set_input(input);
				t->set_output(output);
				return t;
			} else {
				csvtool::RmRow *t = new csvtool::RmRow(indices);
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "mv") {

			if(params.size() != 5) {
				std::cout << "not enough params here." << std::endl;
				std::cout << std::endl;
				std::cout << "mv src_index dst_index" << std::endl;
				exit(1);
			}

			int src_index = atoi(params[2].c_str());
			int dst_index = atoi(params[3].c_str());

			if(params[0] == "col") {
				csvtool::MoveCol *t = new csvtool::MoveCol(src_index,dst_index);
				t->set_input(input);
				t->set_output(output);
				return t;
			} else {
				csvtool::MoveRow *t = new csvtool::MoveRow(src_index,dst_index);
				t->set_input(input);
				t->set_output(output);
				return t;
			}

		} else if(params[1] == "cp") {

		} else {
			std::cout << "known param:" << params[0] << std::endl;
			std::cout << "unknown param:" << params[1] << std::endl;
			exit(1);
		}


	} else if(params[0] == "rm_trailing_commas") {

		if(params.size() != 2) {
			std::cout << "not enough params here." << std::endl;
			exit(1);
		}

		std::string ifname = params[params.size()-1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		csvtool::RemoveEmptyCellAtRowEnd *t  = new csvtool::RemoveEmptyCellAtRowEnd();
		t->set_input(input);
		t->set_output(output);
		return t;

	} else if(params[0] == "info") {

	} else if(params[0] == "transpose") {

		if(params.size() != 2) {
			std::cout << "not enough params here." << std::endl;
			exit(1);
		}

		std::string ifname = params[params.size()-1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		csvtool::Transpose *t  = new csvtool::Transpose();
		t->set_input(input);
		t->set_output(output);

		return t;

	} else if(params[0] == "join") {

		if(params.size()<4) {
			std::cout << "not enough params here." << std::endl;
			std::cout << "join AnexttoB fileA.csv fileB.csv [fileC.csv]" << std::endl;
			std::cout << "join AaboveB fileA.csv fileB.csv [fileC.csv]" << std::endl;
			exit(1);
		}

		std::vector<std::string> ifnames;
		std::vector<csvtool::Input*> inputs;

		for(int j=2;j<params.size();++j) {
			ifnames.push_back(params[j]);
			inputs.push_back(new csvtool::Input(params[j])); // MALLOC HERE
		}

		if(params[1]=="AnexttoB") {

			csvtool::JoinAnexttoB *t  = new csvtool::JoinAnexttoB();
			t->set_inputs(inputs);
			t->set_output(output);
			return t;

		} else if(params[1]=="AaboveB") {

			csvtool::JoinAaboveB *t  = new csvtool::JoinAaboveB();
			t->set_inputs(inputs);
			t->set_output(output);
			return t;
		}

	} else if(params[0] == "rotate_right") {

		if(params.size()!=2) {
			std::cout << "not enough params here." << std::endl;
			std::cout << "rotate_right tt.csv" << std::endl;
			exit(1);
		}

		std::string ifname = params[1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		csvtool::RotateRight *t  = new csvtool::RotateRight();
		t->set_input(input);
		t->set_output(output);
		return t;

	} else if(params[0] == "rotate_left") {

		if(params.size()!=2) {
			std::cout << "not enough params here." << std::endl;
			std::cout << "rotate_left tt.csv" << std::endl;
			exit(1);
		}

		std::string ifname = params[1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		csvtool::RotateLeft *t  = new csvtool::RotateLeft();
		t->set_input(input);
		t->set_output(output);
		return t;

	} else if(params[0] == "is_square") {

		if(params.size()!=2) {
			std::cout << "not enough params here." << std::endl;
			std::cout << "is_square tt.csv" << std::endl;
			exit(1);
		}

		std::string ifname = params[1];
		csvtool::Input *input = new csvtool::Input(ifname); // MALLOC HERE

		csvtool::IsSquare *t  = new csvtool::IsSquare();
		t->set_input(input);
		t->set_output(output);
		return t;

	} else {
		std::cout << "unknown param:" << params[0] << std::endl;
		exit(1);
	}

	return 0;
}

}; // namespace csvtool


int main(int argc,char **argv)
{
/*
	//std::string sqlstmt = "select a,d from tt.csv where ( a = 5 and (b = 3)) or c = 1 and d=  10 ;";
	std::string sqlstmt = "select a,d from tt.csv where (a = 'hi' and c=1);";
	SqlParse::SelectNode *select;
	select = SqlParse::Parse(sqlstmt);

	std::map<std::string,std::string> csv_data_map;

	//csv_data_map["a"] = "5";
	csv_data_map["a"] = "hi";
	csv_data_map["b"] = "3";
	csv_data_map["c"] = "1";
	csv_data_map["d"] = "1";

	select->evaluate(csv_data_map);

	return 0;
*/

	csvtool::Tool *tool = csvtool::ToolFactory(argc,argv);

	tool->run();

	delete tool;

	return 0;
}




