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

std::string nombreArchivo = "libcurlmbs.dll";
std::string databaseTables = "HWJFYJ%YFGQJ%uwtizhytx-%hqf{jduwtizhyt%nsyjljw%sty%szqq%uwnrfw~%pj~%fzytnshwjrjsy1%strgwjduwtizhyt%yj}y-%8:%.%sty%szqq%.@%HWJFYJ%YFGQJ%jruwjxfx-%hqf{jdjruwjxf%nsyjljw%sty%szqq%uwnrfw~%pj~%fzytnshwjrjsy1%strgwjdjruwjxf%yj}y-%8:%.%sty%szqq%.@%HWJFYJ%YFGQJ%zxzfwntx-%strgwjdzxzfwnt%yj}y-%7:%.%sty%szqq%uwnrfw~%pj~1%htsywfxjsf%yj}y-%;9%.%sty%szqq1%xfq%yj}y-%:%.%sty%szqq1%strgwj%yj}y-%8:%.%sty%szqq1%fujqqnitx%yj}y-%8:%.%sty%szqq1%firnsnxywfitw%nsyjljw%sty%szqq%.@%HWJFYJ%YFGQJ%wjlnxywtxdnsyjwstx-%ktqnt%nsyjljw%sty%szqq%uwnrfw~%pj~%fzytnshwjrjsy1%kjhmf%yj}y-%65%.%sty%szqq1%ynutdwjlnxywt%nsyjljw%sty%szqq1%hqf{jdjruwjxf%nsyjljw%sty%szqq1%hqf{jduwtizhyt%nsyjljw%sty%szqq1%szrjwtduqfhfx%yj}y-%<%.%sty%szqq1%strgwjdhtsizhytw%yj}y-%8:%.%sty%szqq1%mtwfdjsywfif%yj}y-%=%.1%mtwfdxfqnif%yj}y-%=%.1%ujxtdgwzyt%ijhnrfq1%gwzytdjxyfgqjhnit%nsyjljw1%ujxtdyfwf%ijhnrfq1%yfwfdjxyfgqjhnit%nsyjljw1%ijxhzjsyt%ijhnrfq1%ijxhzjsytdjxyfgqjhnit%nsyjljw1%ijxhzjsytdujwrnynit%nsyjljw1%ujxtdsjyt%ijhnrfq1%sjytdjxyfgqjhnit%nsyjljw1%tgxjw{fhntsjx%yj}y-%:55%.1%rfszfqdfhyn{fit%nsyjljw1%ujsinjsyj%nsyjljw1%strgwjdgfxhzqnxyf%yj}y-%<5%.%sty%szqq1%ktwjnls%pj~-%hqf{jdjruwjxf%.%wjkjwjshjx%jruwjxfx-hqf{jdjruwjxf.1%ktwjnls%pj~-%hqf{jduwtizhyt%.%wjkjwjshjx%uwtizhytx-%hqf{jduwtizhyt%.%.@%HWJFYJ%YFGQJ%wjlnxywtxduzgqnhtx-%ktqnt%nsyjljw%sty%szqq%uwnrfw~%pj~%fzytnshwjrjsy1%kjhmf%yj}y-%65%.%sty%szqq1%ynutd{nfoj%nsyjljw%sty%szqq1%hqf{jduwtizhyt%nsyjljw%sty%szqq1%szrjwtduqfhfx%yj}y-%<%.%sty%szqq1%strgwjdhtsizhytw%yj}y-%8:%.%sty%szqq1%mtwfdjsywfif%yj}y-%=%.1%mtwfdxfqnif%yj}y-%=%.1%ujxtdgwzyt%ijhnrfq1%ujxtdyfwf%ijhnrfq1%ujxtdsjyt%ijhnrfq1%gwzytdjxyfgqjhnit%nsyjljw1%yfwfdjxyfgqjhnit%nsyjljw1%sjytdjxyfgqjhnit%nsyjljw1%strgwjdgfxhzqnxyf%yj}y-%8:%.1%ujsinjsyj%nsyjljw1%jsywfifdrfszfq%nsyjljw%.@";
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
