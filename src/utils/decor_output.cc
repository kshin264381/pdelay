#include "decor_output.h"

//
// Private Methods
//

// actually rendering outputs
void DecorOutput::render_output(
	const char& corner, const char& mid, const char& ends)
{
	// First line
	this->fill_line(corner, mid);

	// The stuff
	for (auto this_line : this->StringContainer) {
		StringList tempStrList;
		for (auto i=0; i < this_line.length(); i += (this->width-4)) {
			tempStrList.push_back(this_line.substr(i, (this->width-4)));
		}
		for (auto ts : tempStrList) {
			this->main_line(ts, ends);
		}
	}
	// Last line
	this->fill_line(corner, mid);
}

void DecorOutput::fill_line(const char& corner, const char& mid)
{
	cout << corner;
	if (this->width >= 2) {
		for (unsigned int i = 0; i < this->width - 2; i++) {
			cout << mid;
		}
	}
	cout << corner << endl;
}

void DecorOutput::main_line(string const& String, const char& ends)
{
	auto padding_length = this->width-4 - String.length();

	cout << ends << " ";
	cout << String;
	for (auto p=0; p < padding_length; p++) {
		cout << " ";
	}
	cout << " " << ends;
	cout << endl;
}

//
// Public Methods
//

// Set Decoration Type
int DecorOutput::SetType(const unsigned int& Type)
{
	this->decor_type = Type;
	return 0;
}

// Put String
int DecorOutput::PutString(string const& String)
{
	string str_part;
	stringstream ss(String);
	while(getline(ss, str_part, '\n')) {
		this->StringContainer.push_back(str_part);
	}
	return 0;
}

// Delete the text
void DecorOutput::FlushText()
{
	this->StringContainer.clear();
}

// Print the stuff!!
void DecorOutput::Print()
{
	switch (this->decor_type) {
		case 1:
			this->render_output('*', '*', '*');
			break;
		case 2:
			this->render_output('+', '-', '|');
			break;
		default:
			this->render_output('+', '=', '|');
			break;
	}
}

//
// Constructors and Destructors
//
DecorOutput::DecorOutput(const unsigned int& Type, const unsigned int& width)
{
	this->width = width;
	this->decor_type = Type;
}

DecorOutput::DecorOutput(string const& String, const unsigned int& Type, const unsigned int& width)
{
	this->width = width;
	this->decor_type = Type;
	this->StringContainer.push_back(String);
}

DecorOutput::~DecorOutput()
{
	// nothing to do...
}
