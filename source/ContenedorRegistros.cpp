#include "ContenedorRegistros.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "Aplicacion.h"
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
    database.open( nombreArchivo );
	
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
	database.open( nombreArchivo );
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
	throw invalid_argument( "Error, intento de establecer un registro nulo." );
    }
    
    // Obtiene el nombre introducido
    string nuevoNombre = interfaz.obtenerTextoEntrada( "EntradaNombreRegistro" );
    
    // Busca que no exista un producto que no se llame igual
    Registro *coincidencia = buscarRegistroPorNombre( nuevoNombre );
    if( coincidencia != nullptr ){
	throw invalid_argument( "Ya existe un registro con ese nombre." );
    }
    
    try{
	// Establece el nuevo nombre del programa
	registro -> establecerNombre( interfaz.obtenerTextoEntrada( "EntradaNombreRegistro" ) );
	    
	// Conecta a la base de datos
	database.open( nombreArchivo );
	stringstream consulta;
	consulta << "update " << obtenerNombrePlural() << " set nombre_" << obtenerNombreSingular() << " = '" << nuevoNombre << "' where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << " limit 1;";
	database.query( consulta.str() );
	database.close();
    }
    catch( invalid_argument &ia ){
	throw invalid_argument( ia.what() );
    }
}

// Elimina los datos de un registro dado en la base de datos
void ContenedorRegistros::eliminarRegistro( Registro *registro )
{
    // Verifica que el registro dado no sea nulo
    if( registro == nullptr ){
	throw invalid_argument( "Error, intento de establecer un registro nulo." );
    }
    
    database.open( nombreArchivo );
    
    // Elimina las referencias en los registros internos
    stringstream consulta;
    consulta << "delete from registros_internos where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << ";";
    
    // Elimina el registro de la base de datos
    consulta << "delete from " << obtenerNombrePlural() << " where clave_" << obtenerNombreSingular() << " = " << registro -> obtenerClave() << " limit 1;";
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
void ContenedorRegistros::actualizarListaRegistros()
{
    // Limpia el contenedor
    interfaz.removerElementosHijos( "ContenedorRegistros" );
    
    // Itera a través de la lista de registros y los añade a la interfaz
    for( list< Registro * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        // Crea un elemento que será añadido a la interfaz
        Widget *elemento = new Widget();
        
        stringstream clave;
        clave << setfill( '0' ) << setw( 7 ) << (*registro) -> obtenerClave();
        
        try{
	    elemento -> cargarWidget( "../resources/interfaces/ItemRegistro.glade" );
	    elemento -> establecerTextoEtiqueta( "ItemEntradaClave", clave.str() );
	    elemento -> establecerTextoEtiqueta( "ItemEntradaNombre", (*registro) -> obtenerNombre() );
	    elemento -> establecerImagen( "ImagenRegistro", "../resources/images/icons/" + obtenerNombreSingular() + "64.png" );
	    
	    interfaz.insertarElementoAGrid( elemento, "ItemRegistro", "ContenedorRegistros", 0, (*registro) -> obtenerClave(), 1, 1 );
	}
	catch( runtime_error &re ){
		cerr << re.what() << endl;
	}
	
	delete elemento;
    }
	
}

// Obtiene el objeto de autocompletado
GtkEntryCompletion *ContenedorRegistros::obtenerCompletador() const
{
    return completador;
}
