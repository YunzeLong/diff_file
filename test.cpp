#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <functional>
#include <algorithm>
#include <iterator>
#include <cassert>
#include "HashTable.h"

using namespace std;

string read(istream& fin)
{
	string file;
	if (!fin) {
		cerr << "Cannot open the file" << endl;
		exit(1);
	}

	char ch, temp;
	while (fin >> ch)
	{
		file.push_back(ch);//this gets the normal characters

		while (fin.get(temp) && (temp == ' ' || temp == '\n' || temp == '\r' || temp == '\t')) //and this deals with special characters.
		{
			file.push_back(temp);
		}

		fin.unget(); //back it off so we continue to read in characters.


	}//as of now, read in is complete.

	return file;

}


void createDiff(istream& fold, istream& fnew, ostream& fdiff)
{
	const int PARTITION = 8; //this is the N as referred to in the specs
	HashTable HashTbl;
	string oldstring;
	string newstring;
	int add_length = 0;
	string add_str;

	char ch;
	oldstring = read(fold); //read in old string 
	newstring = read(fnew);//read in new string	

	//read in complete. 

	//partition the old file one character by one character and put into HashTable 

	
	for (int i = 0; i < oldstring.size() - PARTITION; i++)
	{
		string insertString = oldstring.substr(i, PARTITION); //chop up the old string
		HashTbl.insert(make_pair(i, insertString)); //pair the offset and insert as pair into the HashTable. 
	}

	//Insertion into HashTable is complete.
	//now that the HashTable contains every consecutive excerpt of length PARTITION, and is ready to be compared.

	//Start to examine the newstring
	string newexcerpt;

	for (int j = 0; j < newstring.size();)
	{
		newexcerpt = newstring.substr(j, PARTITION);
		vector<int>positions = HashTbl.find(newexcerpt);
		if (!positions.empty())//one or more matches found.
		{
			//clear the add_str first.
			if (add_length != 0) //this gets rid of A0:'s
			{
				fdiff << 'A' << add_length << ':' << add_str;
				add_length = 0; //reset length
				add_str.clear();//reset string
			}
			//examine the matches
			vector<int> extralength;
			int largest_index = 0;
			int advance = 0;
			for (int x = 0; x < positions.size(); x++)
			{
				advance = 0;
				while (oldstring[positions[x] + PARTITION + advance] == newstring[j + PARTITION + advance] &&
					j+PARTITION+advance < newstring.size() && positions[x]+PARTITION+advance < oldstring.size())
				{
					advance++;
					
				}
				

				extralength.push_back(advance);
			}

			for (int m = 0; m < extralength.size(); m++)
			{
				if (extralength[m] > extralength[largest_index])
					largest_index = m;
			}
			//here, we have the index of longest advance, along with how many advances made.
			//so we create a diff copy command
			fdiff << 'C' << PARTITION + extralength[largest_index] << ',' << positions[largest_index];
			j += (PARTITION + extralength[largest_index]);//advance j.
			

		}//end of the case of one or more matches.
		//from here, we would move-on to the next jth element in newString.

		//else, j move to the next position to check again.
		else
		{
			add_str.push_back(newstring[j]);
			add_length++;
			j++;//move to the next element.
		}

	}

	if (add_length != 0) //if anything left in the add_str, add it.
	{
		fdiff << 'A' << add_length << ':' << add_str;
		add_length = 0; //reset length
		add_str.clear();//reset string
	}



}


bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, int& length, int& offset)
{
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	char ch;
	switch (cmd)
	{
	case 'A':
		return getInt(inf, length) && inf.get(ch) && ch == ':';
	case 'C':
		return getInt(inf, length) && inf.get(ch) && ch == ',' && getInt(inf, offset);
	case '\r':
	case '\n':
		return true;
	}
	return false;
}


bool applyDiff(istream& fold, istream& fdiff, ostream& fnew) {
//	fdiff.seekg(0, ios::end);
//	int diff_length = fdiff.tellg();//get the length of fdiff.
//	fdiff.seekg(0, ios::beg);
	char cmd,ch,temp;
	int operation_length = 0;
	int offset = 0;
	string cpy;
	while (true)//while end of fdiff is yet not reached
	{
		//read in commands
		if (!getCommand(fdiff, cmd, operation_length, offset))//wrong command or wrong syntax
			return false;
		else //a valid cmd is entered.
		{
			switch (cmd)
			{
			case 'A':

				cpy.clear();//clear cpy
				for (int i = 0; i < operation_length;)
				{
					while (fdiff.get(ch) && (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t')) //and this deals with special characters.
					{
						cpy.push_back(ch);
						i++;
					}
					
					fdiff.unget(); //back it off so we continue to read in characters.

					fdiff >> ch;//get the char from fdiff

					cpy.push_back(ch);//this adds the normal characters

					while (fdiff.get(temp) && (temp == ' ' || temp == '\n' || temp == '\r' || temp == '\t')) //and this deals with special characters.
					{
						cpy.push_back(temp);
						i++;
					}
					
					fdiff.unget(); //back it off so we continue to read in characters.
					i++;
					
				}
				fnew << cpy;//insert the string into fnew
			

				break;

			case'C':
				if (operation_length < 0 || offset < 0)//invalid
					return false;
				cpy.clear();//clear cpy

//				cerr << "tellg right after: " << fold.tellg() << endl;
//				cerr << "int offset: " << offset << endl;
				fold.clear();
				fold.seekg(offset);//put get ptr to offset.
				
				for (int i = 0; i < operation_length;)
				{
					while(fold.get(ch) && (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') && i<operation_length) //this deals with special characters at the start.
					{
						cpy.push_back(ch);
						i++;
					}
					fold.unget();

					fold >> ch;//get the char from fold

					cerr << (fold.tellg())<<endl;

					cpy.push_back(ch);//this adds the normal characters
					i++;

					while (fold.get(temp) && (temp == ' ' || temp == '\n' || temp == '\r' || temp == '\t') && i<operation_length) //and this deals with special characters.
					{
						cpy.push_back(temp);
						i++;
					}
					
					fold.unget(); //back it off so we continue to read in characters.
					
					
				}
				fnew << cpy;//insert the string into fnew

				break;

			case '\n':
			case '\r':
				break;

			case 'x':
				return true;

			default:
				break;
				//we don't really need a default here because all cases other than the ones listed above have been excluded in the getCommand() funciton.

			};
		
		
		
		}//end else
	
	}//end while

}




bool runtest(string oldName, string newName, string diffName, string newName2)
{
	if (diffName == oldName || diffName == newName ||
		newName2 == oldName || newName2 == diffName ||
		newName2 == newName)
	{
		cerr << "Files used for output must have names distinct from other files" << endl;
		return false;
	}
	ifstream oldFile(oldName, ios::binary);
	if (!oldFile)
	{
		cerr << "Cannot open " << oldName << endl;
		return false;
	}
	ifstream newFile(newName, ios::binary);
	if (!newFile)
	{
		cerr << "Cannot open " << newName << endl;
		return false;
	}
	ofstream diffFile(diffName, ios::binary);
	if (!diffFile)
	{
		cerr << "Cannot create " << diffName << endl;
		return false;
	}
	createDiff(oldFile, newFile, diffFile);
	diffFile.close();

	oldFile.clear();   // clear the end of file condition
	oldFile.seekg(0);  // reset back to beginning of the file
	ifstream diffFile2(diffName, ios::binary);
	if (!diffFile2)
	{
		cerr << "Cannot read the " << diffName << " that was just created!" << endl;
		return false;
	}
	ofstream newFile2(newName2, ios::binary);
	if (!newFile2)
	{
		cerr << "Cannot create " << newName2 << endl;
		return false;
	}
	assert(applyDiff(oldFile, diffFile2, newFile2));
	newFile2.close();

	newFile.clear();
	newFile.seekg(0);
	ifstream newFile3(newName2, ios::binary);
	if (!newFile)
	{
		cerr << "Cannot open " << newName2 << endl;
		return false;
	}
	if (!equal(istreambuf_iterator<char>(newFile), istreambuf_iterator<char>(),
		istreambuf_iterator<char>(newFile3), istreambuf_iterator<char>()))
	{

		cerr << newName2 << " is not identical to " << newName
			<< "; test FAILED" << endl;
		return false;
	}
	return true;
}

int main()
{
	assert(runtest("smallmart1.txt", "smallmart2.txt", "mydifffile.txt", "mynewfile2.txt"));
	cerr << "Test PASSED" << endl;
}

/*
//An old version, I wrote it before I found out that there was getInt and getCommand given, I am keeping this here in comment just for reference.
bool applyDiff(istream& fold, istream& fdiff, ostream& fnew) {
	char ch;
	int len;
	int pos;
	string insert;
	while (fdiff >> ch) {
		//read in commands.
		switch (ch) {
		case 'A':	//addition
			fdiff >> ch;
			if (ch >= '0'&&ch <= '9')
			{
				fdiff.unget();
				fdiff >> len;//gets the length
			}
			else//not a number, wrong format
			{
				return false;
			}
			fdiff >> ch;
			if (ch != ':')//if not a colon, format is wrong
				return false;
			else 
			{
				for (int i = 0; i < len; i++)
				{
					fdiff >> ch;
					insert.push_back(ch);
				}
			}
			break;

		case 'C': //copy
			fdiff >> ch;//examine the length
			if (ch >= '0'&&ch <= '9')
			{
				fdiff.unget();
				fdiff >> len;//gets the length
			}
			else//not a number, wrong format
			{
				return false;
			}
			fdiff >> ch;//get rid of the comma
			if (ch != ',')//format is wrong
				return false;
			else {
				fdiff >> ch;//examine the position.
				if (ch >= '0'&&ch <= '9')
				{
					fdiff.unget();
					fdiff >> pos;//gets the position
				}
				else//not a number, wrong format
				{
					return false;
				}
			}
			//now we acquired the info we need.
			//start to copy.
			//in copy, we push the things to be copied to the back of insert.
			fold.seekg(pos, ios::beg);//put the get to the right position.
			for (int i = 0; i < len; i++)
			{
				fold >> ch;
				insert.push_back(ch);
			}
			break;

		default:
			cerr << "Input is not a command." << endl;
			return false;
		};

		//after we have created the string for insertion, simply push the string to the fnew.
		fnew << insert;

		cerr << "Insertion complete." << endl;

		return true;

	}
}
*/