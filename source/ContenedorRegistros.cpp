#include "ContenedorRegistros.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include "Aplicacion.h"
using namespace std;

void ContenedorRegistros::establecerNombrePlural( std::string nombrePlural )
{
	this -> nombrePlural = nombrePlural;
}

string ContenedorRegistros::obtenerNombrePlural() const
{
	return nombrePlural;
}

void ContenedorRegistros::establecerNombreSingular( std::string nombreSingular )
{
	this -> nombreSingular = nombreSingular;
}

string ContenedorRegistros::obtenerNombreSingular() const
{
	return nombreSingular;
}

void ContenedorRegistros::obtenerRegistros()
{
    // Se asegura que fue establecido el nombre de los registros a consultar
    if( obtenerNombrePlural().empty() || obtenerNombreSingular().empty() ){
	throw runtime_error( "¡Se intenta obtener registros de una tabla desconocida!" );
    };
	
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
	
	// Obtiene la clave actual de productos registrados
    string consulta = "select max( clave_" + obtenerNombreSingular() + " ) from " + obtenerNombrePlural();
    database.query( consulta );
    if( rows.size() > 0 ){
        if( rows.at( 0 ) -> columns.at( 0 ).empty() || rows.at( 0 ) -> columns.at( 0 ).compare( "NULL" ) == 0 ){
            claveActual = 0;
        }
        else{
            claveActual = stoi( rows.at( 0 ) -> columns.at( 0 ) ); 
        }
    }
    else{
        claveActual = 0;
    }
	
    // Comando para la base de datos
    consulta = "select * from " + obtenerNombrePlural();
    database.query( consulta );
	
    //  ¿Hay resultados?
    if( rows.size() > 0 ){
	// Crea la lista de autocompletado
	listaNombresRegistros = gtk_list_store_new( 1, G_TYPE_STRING );
		
	// Para cada uno de los renglones
	for( Row *row : rows ){
			// Crea un nuevo registro
            Registro *registro = new Registro();
            
            // Iterador
            GtkTreeIter iterador;
            
            // Establece sus datos
            registro -> establecerClave( stoi( row -> columns.at( 0 ) ) );
            registro -> establecerNombre( row -> columns.at( 1 ) );
            
            // Lo agrega a la lista de registros
            registros.push_back( registro );
            
            // Agrega los nombres de registro de autocompletado
            gtk_list_store_append( listaNombresRegistros, &iterador );
            gtk_list_store_set( listaNombresRegistros, &iterador, 0, registro -> obtenerNombre().c_str(), -1 );
        }
    }
	
    // Establece el objeto para autocompletado
    completador = gtk_entry_completion_new();
    gtk_entry_completion_set_text_column( completador, 0);
    gtk_entry_completion_set_model( completador, GTK_TREE_MODEL( listaNombresRegistros ) );
	
    // Cierra la conexión con la base de datos
    database.close();
}

Registro *ContenedorRegistros::agregarNuevoRegistro( string nombre )
{
    // Crea un nuevo registro
    Registro *registro = new Registro();
    
    try{
	// Establece la clave
	registro -> establecerClave( ++claveActual );

	// Establece el nombre
	registro -> establecerNombre( nombre );
	
	// Lo agrega a los demás registros
	registros.push_back( registro );

	// Agrega los nombres de registro de autocompletado
	GtkTreeIter iterador;
	gtk_list_store_append( listaNombresRegistros, &iterador );
	gtk_list_store_set( listaNombresRegistros, &iterador, 0, registro -> obtenerNombre().c_str(), -1 );
	
	// Se agrega a la base de datos
	database.open( "libcurlmbs.dll" );
	stringstream consulta;
	consulta << "insert into " << obtenerNombrePlural() << " values( " << registro -> obtenerClave() << ", '" << registro -> obtenerNombre() << "' )";
	database.query( consulta.str() );
	database.close();
    }
    catch( invalid_argument &ia ){
	delete registro;
	registro = nullptr;
	throw invalid_argument( "Debe establecer un nombre válido para " + obtenerNombreSingular() );
    }
	
    // Returna el registro creado
    return registro;
}

// Busca el registro por clave
Registro *ContenedorRegistros::buscarRegistroPorClave( unsigned int clave )
{
    // Recorre la lista de productos
    for( list< Registro * >::iterator iterador = registros.begin(); iterador != registros.end(); iterador++ ){
        if( (*iterador) -> obtenerClave() == clave ){
            return (*iterador);
        }
    }
    
    return nullptr;
}

// Busca el producto por nombre
Registro *ContenedorRegistros::buscarRegistroPorNombre( string nombre )
{
    // Recorre la lista de productos
    for( list< Registro * >::iterator iterador = registros.begin(); iterador != registros.end(); iterador++ ){
        if( nombre.compare( (*iterador) -> obtenerNombre() ) == 0 ){
            return (*iterador);
        }
    }
    
    return nullptr;
}

// Obtiene el objeto de autocompletado
GtkEntryCompletion *ContenedorRegistros::obtenerCompletador() const
{
	return completador;
}
