#include "data_utils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
//using namespace glw;
namespace glw {

void File2Str(string &str, const string &file_name)
{
    ifstream t(file_name);
    if(!t.is_open()){
        str.clear();
        return;
    }
    t.seekg(0, ios::end);
    str.resize((long)t.tellg() + 1);
    t.seekg(0, ios::beg);

    str.assign((istreambuf_iterator<char>(t)),
            istreambuf_iterator<char>());
    //str.at(str.size()) = '\0';
    return;
}

void File2Str(string &str, FILE * fp)
{
    assert(fp);
    fseek(fp, 0, SEEK_END);
    long int fsize = ftell(fp);
    rewind(fp);
    str.resize(fsize + 1);
    fread(&str[0], 1, fsize, fp);
    str.at(fsize) = '\0';
}
}
