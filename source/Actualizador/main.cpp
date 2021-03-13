#include <iostream>
#include <fstream>
#include<Windows.h>
#include<string>
#include <zip.h>
#pragma comment(lib, "urlmon.lib")
using namespace std;

int main()
{
    string dwnld_URL = "http://minmax-project.000webhostapp.com/software/scb/programa.zip";
    string savepath = "C:\\tmp\\filename.txt";
    HRESULT result = URLDownloadToFile(NULL, dwnld_URL.c_str(), "..\\Actualizador\\programa.zip", 0, NULL);

    if( result != S_OK ){
        cout << "F" << endl;
        return 0;
    }

    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open("programa.zip", 0, &err);

    //Search for the file of given name
    const char *name = "build/aplicacion.exe";
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(z, name, 0, &st);

    //Alloc memory for its uncompressed contents
    char *contents = new char[st.size];

    //Read the compressed file
    zip_file *f = zip_fopen(z, name, 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    //And close the archive
    zip_close(z);

    //
    ofstream archivo;
    archivo.open( name, ofstream::binary );
    if( !archivo ){
        cout << "chingo a su madre el america." << endl;
    }

    archivo.write( contents, st.size );

    archivo.close();

    //Do something with the contents
    //delete allocated memory
    delete[] contents;

    return 0;
}