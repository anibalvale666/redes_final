#ifndef AUXILIARYFUNCTIONS_H
#define AUXILIARYFUNCTIONS_H


#include <string>
#include <iostream>
#include <vector>

using namespace std;

vector<string> split_into_words(string full_text)
{
	vector<string> res;
    int ianterior = 0;
    for(int i = 0; i < full_text.size(); i++)
    {
        i = full_text.find(" ", i);
        if(i != -1)
        {
            res.push_back(full_text.substr(ianterior, i - ianterior));
            ianterior = i+1;
        }
        else
        {
            res.push_back(full_text.substr(ianterior));
            i = full_text.size();
        }
    }

    return res;
}

string size_to_string(int p_number, int p_size_string)
{
    string converted_number = to_string(p_number);

    if(converted_number.size() < p_size_string)
    {
        converted_number = string(p_size_string - converted_number.size(), '0') + converted_number;
    }

    return converted_number;
}

#endif
