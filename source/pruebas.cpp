#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include <stringapiset.h>
#include "Database.h"
using namespace std;

int main()
{
    Database database;
    string comando;

	PWSTR path[256];
	SHGetKnownFolderPath( FOLDERID_RoamingAppData, 0, NULL, path );
	CW2A( path );
	string databasePath = 
    database.open( databasePath );

    while( true ){
    	cout << "?: ";
	getline( cin, comando );
	try{
	database.query( comando );
	if( rows.size() > 0 ){
	    // Imprime las columnas
	    for( string str : nameColumns ){
		cout << str << "\t";
	    }
	    
	    cout << endl;
	    
	    // Imprime los renglones
	    for( Row *row : rows ){
		for( string str : row -> columns ){
		    cout << str << "\t";
		}
		cout << endl;
	    }
	}
	}
	catch( runtime_error &e ){
	    cerr << e.what() << endl;
	}
    }

    database.close();

    return 0;
}

