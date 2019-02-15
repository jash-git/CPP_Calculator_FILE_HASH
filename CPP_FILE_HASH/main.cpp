#include <iostream>
#include <string>
#include <cstdio>
#include "md5_wrapper.h"

using namespace std;
void pause()
{
    printf("Press Enter key to continue...");
    fgetc(stdin);
}

int main(int argc, const char* argv[])
{
    md5wrapper md5;

    std::string str;
    std::string str_md5;

    if(argc <= 1)
    {
        //cout <<"Not input file name\n";
        str ="CPP_FILE_HASH.exe";
    }
    else
    {
        str = argv[1];
    }

    str_md5 = md5.getHashFromString(str);

    cout << "MD5 Hash: " << str_md5 << endl;

    pause();
    return 0;
}
