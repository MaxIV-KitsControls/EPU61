#include "DataItem.h"

#include <tango.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

using namespace std;


/**
 * Utility function, reads an item from a CSV file.
 *
 * Expects that entries are ';' separated.
 *
 * @param is the input stream
 * @param result the data to be output
 * @param last indicates whether this item is last in line, followed by end-of-line character instead of ';'
 */
template<class T>
void read_item(istream &is, T &result, bool last=false)
{
	char delim=0;

	result=0;
	is >> result;

	// Ignore reading if stream is eof or fail
	if (!is.good()) return;

	if (!last)
	{
		is >> delim;
		if (delim!=';')
			Tango::Except::throw_exception("Invalid CSV format!","",__PRETTY_FUNCTION__);
	} else
	{
		int next = is.peek();
		// It should be either end of line or end of stream.
		if (next!=-1 && next!='\n' && next!='\r')
			Tango::Except::throw_exception("Invalid CSV format!","",__PRETTY_FUNCTION__);
	}
}

/**
 * Specialization of the read_item utility function for handling DataItem::PhaseMode type.
 *
 * @param is the input stream
 * @param result the data to be output
 * @param last indicates whether this item is last in line, followed by end-of-line character instead of ';'
 */
template<>
void read_item<DataItem::PhaseMode>(istream &is, DataItem::PhaseMode &result, bool last)
{
	char modeChar = 0;

	read_item(is,modeChar,last);
	// Ignore reading if stream is eof or fail
	if (!is.good()) return;


	if (modeChar<'A' || modeChar>'D')
		Tango::Except::throw_exception("Invalid Phase Mode string in the file!","",__PRETTY_FUNCTION__);

	result = (DataItem::PhaseMode) (modeChar-'A');
}

/**
 * Output stream << operator overload for DataItem::PhaseMode type
 *
 * @param os
 * @param pm
 * @return
 */
ostream& operator <<(ostream &os, const DataItem::PhaseMode &pm)
{
	int ipm = (int) pm;
	cout << (char) ('A'+ipm);
	return os;
}


ostream& operator <<(ostream &os, const DataItem &d)
{
	os << d.gap << "  " << d.phase << "  " << d.mode << "  " <<
		d.coils[0] << "  " << d.coils[1] << "  " <<
		d.coils[2] << "  " << d.coils[3] <<  endl;

	return os;
}



istream& operator >>(istream &is, DataItem &d)
{
	read_item(is,d.gap);
	read_item(is,d.phase);
	read_item(is,d.mode);
	read_item(is,d.coils[0]);
	read_item(is,d.coils[1]);
	read_item(is,d.coils[2]);
	read_item(is,d.coils[3],true);

	return is;
}
