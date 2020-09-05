#include "Database.h"
#include <sqlite3.h>
#include <stdexcept>
#include <vector>
#include <string>
using namespace std;

// Constructor
Database::Database():
	ptrDatabase( nullptr ), errorMessage( nullptr )
{
	// Nothing to do
}

// Destructor
Database::~Database()
{
	close();
}

// Connects with the database
void Database::open( string file )
{
	int status = sqlite3_open( file.c_str(), &ptrDatabase );
	if( status ){
		throw runtime_error( "Error trying to open database \'" + file + "\'."  );
	}
}

// Queries the database
void Database::query( string query )
{
	// Deletes the last results
	clear();
	int status = sqlite3_exec( ptrDatabase, query.c_str(), buildResults, nullptr, &errorMessage );
	if( status != SQLITE_OK ){
		throw runtime_error( "Error querying. Error: " + error() );
	}
}

// Gets the error
string Database::error() const
{
	string error = ( errorMessage != nullptr ? errorMessage : "Unknown error" );
	
	return error;
}

// Closes the database
void Database::close()
{
	// If is really open a database
	if( ptrDatabase != nullptr ){
		sqlite3_close( ptrDatabase );
	}
	
	ptrDatabase = nullptr;
}

// Deletes the last results
void Database::clear()
{
	for( Row *row : rows ){
		row -> columns.clear();
		delete row;
	}
	
	rows.clear();
	nameColumns.clear();
}

std::vector< std::string > nameColumns;
std::vector< Row * >rows;

// Called when a query is made
int buildResults( void * notUsed, int numberColums, char **columns, char **columnNames )
{
	// Creates a new Row
	Row *row = new Row();
	
	// Adds the columns found
	for( int count = 0; count < numberColums; ++count ){
		row -> columns.push_back( ( columns[ count ] != NULL ? columns[ count ] : "NULL" ) );
	}
	
	// Adds he row to vector of rows
	rows.push_back( row );
	
	// If first time it's called
	if( rows.size() == 1 ){
		// Adds the column names
		for( int count = 0; count < numberColums; ++count ){
			nameColumns.push_back( columnNames[ count ] );
		}
	}
	
	return 0;
}
