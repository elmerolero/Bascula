#include "GestorRegistros.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Base de datos
Database database;

// Usuario
Usuario usuario;

// Lista de registros de peso del día en curso
unsigned int folioActual;
std::list< Ticket * > registrosInternosPendientes;
std::list< Ticket * > registroConsultados;

// Registros de los productos y empresas
ContenedorRegistros productos;
ContenedorRegistros empresas;

// Obtiene el folio actual en el que se encuentra el registro
void obtenerFolioActual()
{
	// Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
	// Obtiene la clave actual de productos registrados
    string consulta = "select max( folio ) from registros_internos";
    database.query( consulta );
    if( rows.size() > 0 ){
		// Obtiene el valor máximo en formato string
		string maxStr = rows.at( 0 ) -> columns.at( 0 );
		
		// Lo traduce a entero si el string no contiene la cadena "NULL"
		folioActual	= ( maxStr.compare( "NULL" ) != 0 ? stoi( maxStr ) : 0 );
    }
    
    database.close();
}

// Obtiene los tickets registrados del día
void obtenerRegistrosInternosPendientes()
{
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
    // Obtiene los tickets del día
    string consulta = "select * from registros_internos where pendiente = 1";
    database.query( consulta );
    if( rows.size() > 0 ){
        for( Row *row : rows ){
			//Crea el nuevo registro
            Ticket *ticket = new Ticket();
		
			try{
				// Establece los datos del registro
				establecerRegistroInternoDesdeRenglon( ticket, row );
				
				// Lo agrega al campo de registros internos pendientes
				registrosInternosPendientes.push_back( ticket );
			}
			catch( invalid_argument &ia ){
				cerr << ia.what() << endl;
			}
        }
    }
    
    // Cierra la conexión
    database.close();
}

// 
void crearRegistroPendiente( Ticket *ticket )
{
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "insert into registros_internos values( " << ticket -> obtenerFolio() << ", '" << ticket -> obtenerFechaRegistro() << "', " << ticket -> obtenerTipoRegistro() << " ," 
             << ticket -> obtenerEmpresa() -> obtenerClave() << ", " << ticket -> obtenerProducto() -> obtenerClave() << ", '" << ticket -> obtenerNumeroPlacas() << "', '" 
             << ticket -> obtenerNombreConductor() << "', " << ( ticket -> estaPesoBrutoEstablecido() ? "'" + ticket -> obtenerHoraEntrada() + "'" : "null" ) << ", "
             << ( ticket -> estaPesoTaraEstablecido() ? "'" + ticket -> obtenerHoraSalida() + "'" : "null" ) << ", "
             << ( ticket -> estaPesoBrutoEstablecido() ? ticket -> obtenerPesoBruto() : 0 ) << ", " << ticket -> estaPesoBrutoEstablecido() << ", " 
             << ( ticket -> estaPesoTaraEstablecido() ? ticket -> obtenerPesoTara() : 0 ) << ", " << ticket -> estaPesoTaraEstablecido() << ", "
             << ( ticket -> estaDescuentoEstablecido() ? ticket -> obtenerDescuento() : 0 ) << ", " << ticket -> estaDescuentoEstablecido() << ", " << ticket -> permitirDescuento() << ", "
             << ( ticket -> estaPesoNetoCalculado() ? ticket -> obtenerPesoNeto() : 0 ) << ", " << ticket -> estaPesoNetoCalculado() << ", '"
             << ticket -> obtenerObservaciones() << "', " << ticket -> esEntradaManual() << ", " << ticket -> estaPendiente() << " )";
             
    cout << consulta.str() << endl;
            
    // Inserta el nuevo ticket
    try{
		database.query( consulta.str() );
		if( ticket -> estaPendiente() ){
			registrosInternosPendientes.push_back( ticket );
		}
    }
    catch( runtime_error &re ){
		cerr << re.what() << endl;
	}
    
    // Cierra la conexion
    database.close();
}

void finalizarRegistro( Ticket *ticket )
{
    
}

// Busca el registro por folio en la lista de registros internos dada
Ticket *buscarRegistroInternoPorFolio( unsigned int folio, std::list< Ticket * > registros )
{
    for( list< Ticket * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( (*registro) -> obtenerFolio() == folio ){
            return (*registro);
        }
    }
    
    return nullptr;
}

// Busca el registro por el número de placa en la lista de registros internos dada
Ticket *buscarRegistroInternoPorNumeroPlaca( std::string numeroPlacas, std::list< Ticket * > registros )
{
    for( list< Ticket * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( numeroPlacas.compare( (*registro) -> obtenerNumeroPlacas() ) == 0 ){
            return (*registro);
        }
    }
    
    return nullptr;
}

// Establece un registro interno desde un renglón dado
void establecerRegistroInternoDesdeRenglon( Ticket *registroInterno, Row *row )
{
	// Se asegura que no se esté intento de establecer un registro nulo
	if( registroInterno == nullptr ){
		throw invalid_argument( "Intento de asignación de información a un registro nulo." );
	}
	
	// Se establece los datos del registro interno
	registroInterno -> establecerFolio( static_cast< unsigned int >( stoi( row -> columns.at( 0 ) ) ) );
	registroInterno -> establecerFechaRegistro( row -> columns.at( 1 ) );
	registroInterno -> establecerTipoRegistro( stoi( row -> columns.at( 2 ) ) );
	registroInterno -> establecerEmpresa( empresas.buscarRegistroPorClave( stoi( row -> columns.at( 3 ) ) ) );
	registroInterno -> establecerProducto( productos.buscarRegistroPorClave( stoi( row -> columns.at( 4 ) ) ) );
	registroInterno -> establecerNumeroPlacas( row -> columns.at( 5 ) );
	registroInterno -> establecerNombreConductor( row -> columns.at( 6 ) );
	registroInterno -> establecerHoraEntrada( row -> columns.at( 7 ) );
	registroInterno -> establecerHoraSalida( row -> columns.at( 8 ) );
	registroInterno -> establecerPesoBruto( row -> columns.at( 9 ) );
	registroInterno -> establecerPesoBrutoEstablecido( stoi( row -> columns.at( 10 ) ) );
	registroInterno -> establecerPesoTara( row -> columns.at( 11 ) );
	registroInterno -> establecerPesoTaraEstablecido( stoi( row -> columns.at( 12 ) ) );
	registroInterno -> establecerDescuento( row -> columns.at( 13 ) );
	registroInterno -> establecerDescuentoEstablecido( stoi( row -> columns.at( 14 ) ) );
	registroInterno -> permitirDescuento( stoi( row -> columns.at( 15 ) ) );
	registroInterno -> establecerPesoNeto( stod( row -> columns.at( 16 ) ) );
	registroInterno -> establecerPesoNetoCalculado( stoi( row -> columns.at( 17 ) ) );
	registroInterno -> establecerObservaciones( row -> columns.at( 18 ) );
	registroInterno -> establecerEntradaManual( stoi( row -> columns.at( 19 ) ) );
	registroInterno -> establecerPendiente( stoi( row -> columns.at( 20 ) ) );
}
