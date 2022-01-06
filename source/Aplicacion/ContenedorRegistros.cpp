#include "ContenedorRegistros.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "Aplicacion.h"
#include "Senales.h"
#include "Imagen.h"
#include "GestorRegistros.h"
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
    database.open( databaseFile );
	
	// Obtiene la clave actual de productos registrados
    string consulta = "select max( id_" + obtenerNombreSingular() + " ) as id from " + obtenerNombreSingular();
    database.query( consulta );
    if( results.size() > 0 ){
        try{
            claveActual = stoi( (* results.at( 0 ) )[ "id" ] );
        }
        catch( invalid_argument &ia ){
            claveActual = 0;
        }
    }
    else{
        claveActual = 0;
    }
	
    // Comando para la base de datos
    consulta = "select * from " + obtenerNombreSingular();
    database.query( consulta );
    database.close();
	
    //  ¿Hay resultados?
    if( results.size() > 0 ){
	// Crea la lista de autocompletado
	listaNombresRegistros = gtk_list_store_new( 1, G_TYPE_STRING );
		
	// Para cada uno de los renglones
	for( auto *renglon : results ){
			// Crea un nuevo registro
            Registro *registro = new Registro( renglon, obtenerNombreSingular() );
            
            // Iterador
            GtkTreeIter iterador;
            
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
	   database.open( databaseFile );
	   stringstream consulta;
	   consulta << "insert into " << obtenerNombrePlural() << " values( " << registro -> obtenerClave() << ", '" << registro -> obtenerNombre() << "' )";
	   database.query( consulta.str() );
	   database.close();
    }
    catch( invalid_argument &ia ){
	   delete registro;
	   --claveActual;
	   registro = nullptr;
	   throw invalid_argument( "Debe establecer un nombre válido para " + obtenerNombreSingular() );
    }
	
    // Returna el registro creado
    return registro;
}

// Actualiza los datos de un registro en la base de datos
void ContenedorRegistros::actualizarRegistro( Registro * registro )
{   
    // Verifica que el registro dado no sea nulo
    if( registro == nullptr ){
	    throw runtime_error( "Error, intento de establecer un registro nulo." );
    }
            
    // Conecta a la base de datos
    database.open( databaseFile );
    stringstream consulta;
    consulta << "update " << obtenerNombrePlural() << " set nombre_" << obtenerNombreSingular() << " = '" << registro -> obtenerNombre() << "' where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << ";";
    database.query( consulta.str() );
    database.close();
}

// Elimina los datos de un registro dado en la base de datos
void ContenedorRegistros::eliminarRegistro( Registro *registro )
{
    // Verifica que el registro dado no sea nulo
    if( registro == nullptr ){
	    throw invalid_argument( "Error, intento de establecer un registro nulo." );
    }
    
    database.open( databaseFile );
    
    // Elimina las referencias en los registros internos
    stringstream consulta;
    consulta << "delete from registros_internos where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << ";";
    
    // Elimina el registro de la base de datos
    consulta << "delete from " << obtenerNombrePlural() << " where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << ";";
    database.query( consulta.str() );
    
    // Elimina de la lista el registro dado
    registros.remove( registro );
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

// Actualiza la lista de registros en la interfaz de usuario
void ContenedorRegistros::actualizarListaRegistros(){
    // Limpia el contenedor
    limpiar_contenedor( "ContenedorRegistros" );
    
    // Itera a través de la lista de registros y los añade a la interfaz
    for( list< Registro * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        GError *error = nullptr;
        GtkBuilder *builder = gtk_builder_new();
        
        // Clave del registro
        stringstream clave;
        clave << setfill( '0' ) << setw( 7 ) << (*registro) -> obtenerClave();
        
        try{
            gtk_builder_add_from_file( builder, "../recursos/interfaces/ItemRegistro.glade", &error );
            gtk_widget_set_name( GTK_WIDGET( buscar_objeto( "ItemRegistro" ) ), to_string( (*registro) -> obtenerClave() ).c_str() );
            gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaClave" ) ), clave.str().c_str() );
            gtk_label_set_text( GTK_LABEL( buscar_objeto( "ItemEntradaNombre" ) ), (*registro) -> obtenerNombre().c_str() );
            gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistro" ) ), ("../resources/images/icons/" + obtenerNombreSingular() + "64.png").c_str() );
	    
            gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ), GTK_WIDGET( buscar_objeto( "ItemRegistro" ) ), (*registro) -> obtenerClave() );
	    }
	    catch( runtime_error &re ){
		    cerr << re.what() << endl;
	    }
	
	    gtk_widget_destroy( GTK_WIDGET( builder ) );
    }
	
}

// Obtiene el objeto de autocompletado
GtkEntryCompletion *ContenedorRegistros::obtenerCompletador() const
{
    return completador;
}
