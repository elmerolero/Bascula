#include <iostream>
#include "Database.h"
using namespace std;

int main()
{
    Database database;
    string comando;

    database.open( "noc" );

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

