#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "Database.h"
using namespace std;



const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

int main(){
    Database database;
    /*database.open( databaseFile );
    database.query( "select max( clave ) as clave from basculas" );
    database.close();

    if( results.size() > 0 ){
        // Gets the headers
		auto result = results.at( 0 );
		for( auto nameColumn : nameColumns ){
			cout << nameColumn << ", ";
		}
		cout << endl;

		// For each row
		for( auto result : results ){
			// For each column
			for( auto column : nameColumns ){
				cout << (* result)[ column ] << ", ";
			}
			cout << endl;
		}
    }*/

    /*string consulta = "select * from basculas";
    database.open( databaseFile );
    database.query( consulta );
    database.close();
    
    if( results.size() > 0 ){
        for( auto *renglon : results ){
            /*cout << "Clave: " << (* renglon)[ "clave" ] << endl
                 << "Nombre: " << (* renglon)[ "nombre" ] << endl
                 << "Puerto: " << (* renglon)[ "puerto" ] << endl
                 << "Baudrate: " << (* renglon)[ "baudrate" ] << endl
                 << "Bytesize: " << (* renglon)[ "bytesize" ] << endl
                 << "Parity: " << stoi( (* renglon)[ "parity" ] ) << endl
                 << "StopBits: " << (* renglon)[ "stopbits" ] << endl
                 << "Bytes ignorades: " << (* renglon)[ "ignoredbytes" ] << endl << endl;*/
            // Crea el objeto para ese registro
            //Bascula *bascula = new Bascula( renglon );

            // Lo añade al grupo de básculas
            //basculasRegistradas.push_back( bascula );
        /*}
    }
    
    try{
        // Obtiene el codigo máximo registrado
        consulta = "select max( clave ) as clave from basculas";
        database.open( databaseFile );
        database.query( consulta );

        //cout << (results.size() > 0) << endl;
        //cout << ( results.size() > 0 ? stoi( (* results.at( 0 ))[ "clave" ] ) : 0 ) << endl;
    }
    catch( exception &e ){
        //codigoBasculaActual = 0;
    }

    // Cierra la conexión a la base de datos
    database.close();*/

    //cout << "llegue aqui" << endl;
    // Obtiene la hora en un formato válido para la base de datos
    /*cout << "Hora: " << tiempo_leer_hora( 1 ) << endl;
    cout << "Fecha corta: " << tiempo_leer_fecha_corta() << endl;
    cout << "Fecha larga: " << tiempo_leer_fecha_larga() << endl;
    cout << "Fecha const: " << tiempo_contruir_fecha( 1, 3, 2021 ) << endl;*/

    return 0;
}
