#include "GestorRegistros.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Base de datos
Database database;

// Usuario
Usuario usuario;

// Contador de folio de registros internos y públicos
unsigned int folioActual;
unsigned int folioActualPublico;

// Lista de registros
std::list< Ticket * > registrosInternosPendientes;
std::list< Ticket * > registroConsultados;

// Lista de registros publicos
std::list< TicketPublico * > registrosPublicosPendientes;
std::list< TicketPublico * > registrosPublicosConsultados;

// Registros de los productos y empresas
ContenedorRegistros productos;
ContenedorRegistros empresas;

// Obtiene el folio actual en el que se encuentra el registro
void obtenerFolioActual()
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Obtiene la clave actual de productos registrados
    string consulta = "select max( folio ) from registros_internos";
    database.query( consulta );
    if( rows.size() > 0 ){
	// Obtiene el valor máximo en formato string
	string maxStr = rows.at( 0 ) -> columns.at( 0 );
		
	// Lo traduce a entero si el string no contiene la cadena "NULL"
	folioActual = ( maxStr.compare( "NULL" ) != 0 ? stoi( maxStr ) : 0 );
    }
    
    database.close();
}

// Obtiene el folio actual en el que se encuentran los registros de báscula pública
void obtenerFolioActualPublico()
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Obtiene la clave actual de productos registrados
    string consulta = "select max( folio ) from registros_publicos";
    database.query( consulta );
    if( rows.size() > 0 ){
	// Obtiene el valor máximo en formato string
	string maxStr = rows.at( 0 ) -> columns.at( 0 );
		
	// Lo traduce a entero si el string no contiene la cadena "NULL"
	folioActualPublico = ( maxStr.compare( "NULL" ) != 0 ? stoi( maxStr ) : 0 );
    }
    
    database.close();
}

// Obtiene los tickets registrados del día
void obtenerRegistrosPublicosPendientes()
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Obtiene los tickets del día
    string consulta = "select * from registros_publicos where pendiente = 1";
    database.query( consulta );
    if( rows.size() > 0 ){
        for( Row *row : rows ){
	    //Crea el nuevo registro
            TicketPublico *registroPublico = new TicketPublico();
		
	    try{
		// Establece los datos del registro
		establecerRegistroPublicoDesdeRenglon( registroPublico, row );
				
		// Lo agrega al campo de registros internos pendientes
		registrosPublicosPendientes.push_back( registroPublico );
	    }
	    catch( invalid_argument &ia ){
		cerr << ia.what() << endl;
	    }
        }
    }
    
    // Cierra la conexión
    database.close();
}

// Obtiene los tickets registrados del día
void obtenerRegistrosInternosPendientes()
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
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
    database.open( nombreArchivo );
    
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
             << ticket -> obtenerObservaciones() << "', " << ticket -> esEntradaManual() << ", " << ticket -> estaPendiente() << ", '" << ticket -> obtenerNombreBasculista() << "' )";
            
    // Inserta el nuevo ticket
    try{
	database.query( consulta.str() );
	if( ticket -> estaPendiente() ){
	    registrosInternosPendientes.push_back( ticket );
	}
	else{
	    ticket -> imprimir();
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
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_internos set peso_tara = " << ticket -> obtenerPesoTara() << ", tara_establecido = " << ticket -> estaPesoTaraEstablecido() 
	     << ", hora_salida = '" << ticket -> obtenerHoraSalida() << "', pendiente = 0"
	     << ", descuento = " << ( ticket -> estaDescuentoEstablecido() ? ticket -> obtenerDescuento() : 0 ) << ", descuento_establecido = " << ticket -> estaDescuentoEstablecido() 
	     << ", descuento_permitido = " << ticket -> permitirDescuento() << ", peso_neto = " << ticket -> obtenerPesoNeto() << ", neto_establecido = " << ticket -> estaPesoNetoCalculado()
	     << ", tipo_registro = " << ticket -> obtenerTipoRegistro() << ", observaciones = '" << ticket -> obtenerObservaciones() << "' where folio = " << ticket  -> obtenerFolio();
	    
    try{
	// Inserta el nuevo ticket
	database.query( consulta.str() );
	
	// Imprime el ticket
	ticket -> imprimir();
	
	// Remueve el ticket de los registros pendientes
	registrosInternosPendientes.remove( ticket );
	
	// Elimina el ticket
	delete ticket;
	ticket = nullptr;
    }
    catch( runtime_error &re ){
	cerr << re.what() << endl;
    }
    
    // Cierra la conexion
    database.close();
}

void finalizarRegistroPublico( TicketPublico *registroPublico )
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_publicos set peso_tara = " << registroPublico -> obtenerPesoTara() << ", tara_establecido = " << registroPublico -> estaPesoTaraEstablecido() 
	     << ", hora_salida = '" << registroPublico -> obtenerHoraSalida() << "', pendiente = 0"
	     << ", peso_neto = " << registroPublico -> obtenerPesoNeto() << ", neto_establecido = " << registroPublico -> estaPesoNetoEstablecido()
	     << ", tipo_viaje = " << registroPublico -> obtenerTipoViaje() << " where folio = " << registroPublico -> obtenerFolio();
	    
    try{
	// Inserta el nuevo ticket
	database.query( consulta.str() );
	
	// Imprime el ticket
	//ticket -> imprimir();
	
	// Remueve el ticket de los registros pendientes
	registrosPublicosPendientes.remove( registroPublico );
	
	// Elimina el ticket
	delete registroPublico;
	registroPublico = nullptr;
    }
    catch( runtime_error &re ){
	cerr << re.what() << endl;
    }
    
    // Cierra la conexion
    database.close();
}

void crearRegistroPublicoPendiente( TicketPublico *registroPublico )
{
    // Conecta con la base de datos
    database.open( nombreArchivo );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "insert into registros_publicos values( " << registroPublico -> obtenerFolio() << ", '" << registroPublico -> obtenerFechaRegistro() << "', " << registroPublico -> obtenerTipoViaje() << ", " 
             << registroPublico -> obtenerProducto() -> obtenerClave() << ", '" << registroPublico -> obtenerNumeroPlacas() << "', '" 
             << registroPublico -> obtenerNombreConductor() << "', " << ( registroPublico -> estaPesoBrutoEstablecido() ? "'" + registroPublico -> obtenerHoraEntrada() + "'" : "null" ) << ", "
             << ( registroPublico -> estaPesoTaraEstablecido() ? "'" + registroPublico -> obtenerHoraSalida() + "'" : "null" ) << ", "
             << ( registroPublico -> estaPesoBrutoEstablecido() ? registroPublico -> obtenerPesoBruto() : 0 ) << ", " 
             << ( registroPublico -> estaPesoTaraEstablecido() ? registroPublico -> obtenerPesoTara() : 0 ) << ", "
             << ( registroPublico -> estaPesoNetoEstablecido() ? registroPublico -> obtenerPesoNeto() : 0 ) << ", " 
	     << registroPublico -> estaPesoBrutoEstablecido() << ", " << registroPublico -> estaPesoTaraEstablecido() << ", " << registroPublico -> estaPesoNetoEstablecido() << ", '"
	     << registroPublico -> obtenerNombreBasculista() << "', " << registroPublico -> estaPendiente() << ", " << registroPublico -> esEntradaManual() << ")";
            
	    
    // Inserta el nuevo ticket
    try{
	// database.query( consulta.str() );
	cout << consulta.str() << endl;
	if( registroPublico -> estaPendiente() ){
	    registrosPublicosPendientes.push_back( registroPublico );
	}
	/*else{
	    ticket -> imprimir();
	}*/
    }
    catch( runtime_error &re ){
	cerr << re.what() << endl;
    }
    
    // Cierra la conexion
    database.close();
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

// Busca el registro por folio en la lista de registros publicos dada
TicketPublico *buscarRegistroPublicoPorFolio( unsigned int folio, std::list< TicketPublico * > registros )
{
    for( list< TicketPublico * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( (*registro) -> obtenerFolio() == folio ){
            return (*registro);
        }
    }
    
    return nullptr;
}

// Busca el registro por el número de placa en la lista de registros publicos dada
TicketPublico *buscarRegistroPublicoPorNumeroPlaca( std::string numeroPlacas, std::list< TicketPublico * > registros )
{
    for( list< TicketPublico * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
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
    registroInterno -> establecerNombreBasculista( row -> columns.at( 21 ) );
}

// Establece un registro publico desde un renglón dado
void establecerRegistroPublicoDesdeRenglon( TicketPublico *registroPublico, Row *row )
{
    // Se asegura que no se esté intento de establecer un registro nulo
    if( registroPublico == nullptr ){
	    throw invalid_argument( "Intento de asignación de información a un registro nulo." );
    }
    
    // Se establece los datos del registro interno
    registroPublico -> establecerFolio( static_cast< unsigned int >( stoi( row -> columns.at( 0 ) ) ) );
    registroPublico -> establecerFechaRegistro( row -> columns.at( 1 ) );
    registroPublico -> establecerTipoViaje( stoi( row -> columns.at( 2 ) ) );
    registroPublico -> establecerProducto( productos.buscarRegistroPorClave( stoi( row -> columns.at( 3 ) ) ) );
    registroPublico -> establecerNumeroPlacas( row -> columns.at( 4 ) );
    registroPublico -> establecerNombreConductor( row -> columns.at( 5 ) );
    registroPublico -> establecerHoraEntrada( row -> columns.at( 6 ) );
    registroPublico -> establecerHoraSalida( row -> columns.at( 7 ) );
    registroPublico -> establecerPesoBruto( row -> columns.at( 8 ) );
    registroPublico -> establecerPesoTara( row -> columns.at( 9 ) );
    registroPublico -> establecerPesoNeto( stod( row -> columns.at( 10 ) ) );
    registroPublico -> establecerPesoBrutoEstablecido( stoi( row -> columns.at( 11 ) ) );
    registroPublico -> establecerPesoTaraEstablecido( stoi( row -> columns.at( 12 ) ) );
    registroPublico -> establecerPesoNetoEstablecido( stoi( row -> columns.at( 13 ) ) );
    registroPublico -> establecerNombreBasculista( row -> columns.at( 14 ) );
    registroPublico -> establecerPendiente( stoi( row -> columns.at( 15 ) ) );
    registroPublico -> establecerEntradaManual( stoi( row -> columns.at( 16 ) ) );
}
