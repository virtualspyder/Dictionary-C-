#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

struct DictionaryItem
{
	string item_type;
	string item_value;
};

struct DictionarySection
{
	string original_name;
	vector<DictionaryItem> items;  // Item type, Item value.
};

struct Distinct
{
	string item_type;
	map<string, bool> already_outputted;
};


bool sortItem(const DictionaryItem& a, const DictionaryItem& b);
string toLowerString(string s);
void loadAllData(string file, map<string, DictionarySection> &dictionary);
Distinct * checkOutputState(vector<Distinct> &output_states, DictionaryItem &item);

int main()
{
	map<string, DictionarySection> dictionary;
	loadAllData("./Data.CS.SFSU.txt", dictionary);

	cout << "----- DICTIONARY 340 C++ -----" << endl << endl;

	string tab = "    ";
	string output;

	while (true)
	{
		if (output.size())
		{
			cout << tab << "|" << endl;
			cout << output;
			cout << tab << "|" << endl;

			output = "";
		}

		string search;
		cout << "Search: ";
		getline(cin, search);

		if (search == "!Q")
		{
			cout << "\n";
			break;
		}

		string str;
		stringstream ss (search);
		vector<string> arguments;

		while (ss >> str) {
			arguments.push_back(str);
		}

		string argument1 = "", argument2 = "", argument3 = "";

		if (arguments.size() >= 1) { argument1 = toLowerString(arguments[0]); }
		if (arguments.size() >= 2) { argument2 = toLowerString(arguments[1]); }
		if (arguments.size() >= 3) { argument3 = arguments[2]; }

		// Dictionary section not found.
		if (dictionary[argument1].items.empty())
		{
			output = tab + "<Not found>\n";
			continue;
		}

		if (argument1 == "preposition" && argument2 == "propersition" && argument3 == "distinct")
		{
			output = tab + "<2nd argument must be a part of speech or \"distinct\">\n";
			continue;
		}

		// Just one argument. All items will be outputted.
		if (argument2.empty())
		{
			for (int i = 0; i < (int)dictionary[argument1].items.size(); i++) {
				
				DictionaryItem item = dictionary[argument1].items[i];	
				output += tab + dictionary[argument1].original_name + " [" + item.item_type + "] : " + item.item_value + "\n";
			}
			continue;
		}

		vector<Distinct> output_states;

		// The word "distinct" means duplicate item values are not outputted.
		if (argument2 == "distinct")
		{
			for (int i = 0; i < (int)dictionary[argument1].items.size(); i++) {

				DictionaryItem item = dictionary[argument1].items[i];
				Distinct *output_state = checkOutputState(output_states, item);

				if (output_state->already_outputted[item.item_value] == false)
				{
					output += tab + dictionary[argument1].original_name + " [" + item.item_type + "] : " + item.item_value + "\n";
					output_state->already_outputted[item.item_value] = true;
				}
			}
			continue;
		}

		int items_found = 0;

		// The third argument is empty or not "distinct".
		if (argument3 != "distinct")
		{
			for (int i = 0; i < (int)dictionary[argument1].items.size(); i++)
			{
				if (dictionary[argument1].items[i].item_type == toLowerString(argument2))
				{
					DictionaryItem item = dictionary[argument1].items[i];
					output += tab + dictionary[argument1].original_name + " [" + item.item_type + "] : " + item.item_value + "\n";

					items_found += 1;
				}
			}
		}
		else // Three arguments are specified, the last one is "distinct".
		{
			for (int i = 0; i < (int)dictionary[argument1].items.size(); i++) 
			{
				if (dictionary[argument1].items[i].item_type != toLowerString(argument2)) {
					continue;
				}

				DictionaryItem item = dictionary[argument1].items[i];
				Distinct *output_state = checkOutputState(output_states, item);

				if (output_state->already_outputted[item.item_value] == false)
				{
					output += tab + dictionary[argument1].original_name + " [" + item.item_type + "] : " + item.item_value + "\n";
					output_state->already_outputted[item.item_value] = true;

					items_found += 1;
				}
			}
		}

		if (items_found == 0)
		{
			output = tab + "<Not found>\n";
		}
	}

	cout << "-----THANK YOU-----\n";

	return 0;
}


// To handle the word "Distinct", each item type only contains unique values.
Distinct * checkOutputState(vector<Distinct> &output_states, DictionaryItem &item)
{
	Distinct *output_state = nullptr;
	for (int k = 0; k < (int)output_states.size(); ++k)
	{
		if (output_states[k].item_type == item.item_type)
		{
			output_state = &output_states[k];
			break;
		}
	}

	if (output_state == nullptr)
	{
		output_states.resize(output_states.size() + 1);
		output_states.back().item_type = item.item_type;

		output_state = &output_states.back();
	}
	return output_state;
}

bool sortItem(const DictionaryItem& a, const DictionaryItem& b) {
	return a.item_type < b.item_type;
}

string toLowerString(string s)
{
	for (int i = 0; i < (int)s.size(); ++i)
	{
		s[i] = tolower(s[i]);
	}
	return s;
}

void loadAllData(string file, map<string, DictionarySection> &dictionary)
{
	string data_line;

	cout << "! Opening data file... " << file << endl;
	ifstream ifs(file);

	if (!ifs)
	{
		cout << "Fatal Error: " << file << " not found." << endl;
		exit(0);
	}

	cout << "! Loading data..." << endl;

	while (getline(ifs, data_line))
	{
		stringstream ss(data_line);

		string block;
		string section_name;

		bool first = true;
		while (getline(ss, block, '|'))
		{
			if (first) {
				section_name = toLowerString(block);
				dictionary[section_name].original_name = block;
				first = false;
				continue;
			}

			string token_to_find = " -=>> ";
			int pos = (int)block.find(token_to_find);

			string item_type = block.substr(0, pos);
			string item_value = block.substr(pos + token_to_find.size());

			dictionary[section_name].items.push_back({ item_type, item_value });
		}
	}
	cout << "! Loading completed..." << endl;

	// Sort all dictionary items, ordered by item types.
	for (auto &it : dictionary) {
		sort(it.second.items.begin(), it.second.items.end(), sortItem);
	}


	cout << "! Closing data file... " << file << endl << endl;

	ifs.close();
}

