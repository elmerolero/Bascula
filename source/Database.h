#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <vector>
#include <string>

struct Row
{
	std::vector< std::string > columns;
};

class Database
{
	public:
		// Constructor
		Database();
		
		// Destructor
		~Database();
		
		// Connects with the database
		void open( std::string file );
		
		// Queries the database
		void query( std::string query );
		
		// Gets the error found in the data base
		std::string error() const;
		
		// Closes the database
		void close();
	
	private:
		void clear();
		sqlite3 *ptrDatabase;
		char *errorMessage;
};


extern std::vector< std::string > nameColumns;
extern std::vector< Row * >rows;

// Called when a query is made
int buildResults( void * notUsed, int numberColums, char **columns, char **columnNames );

#endif
