#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Sesion.h"
#include "Vistas.h"
#include "sha256.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <random>
using namespace std;

// Base de datos
Database database;

// El usuario seleccionado a consultar
Usuario *usuarioConsultado;

// Contador de folio de registros internos y públicos
unsigned int folioActual;
unsigned int folioActualPublico;

// Lista de registros
std::list< Ticket * > registrosInternosPendientes;
std::list< Ticket * > registrosInternosConsultados;

// Lista de registros publicos
std::list< TicketPublico * > registrosPublicosPendientes;
std::list< TicketPublico * > registrosPublicosConsultados;

// usuarios registrados
list< Usuario * > registrosUsuarios;

// Registros de los productos y empresas
ContenedorRegistros productos;
ContenedorRegistros empresas;

// Nombre de la empresa
string empresa_razon_social;
string empresa_rfc;
string empresa_imagen;

// Numero de copias a imprimir en el ticket
unsigned int numeroFormatos = 0;
unsigned int numeroCopias = 0;

bool esInicio;

// Completador para el nombre del conductor
GtkListStore *listaNombresConductor = nullptr;
GtkEntryCompletion *completadorNombresConductor = nullptr;

// Completador 
GtkListStore *listaNumerosPlaca = nullptr;
GtkEntryCompletion *completadorNumerosPlaca = nullptr;

// Crea una nueva lista
void actualizarElementosLista( GtkListStore **listaNombresRegistro, GtkEntryCompletion **completador, std::string registro )
{
    // ¿La lista está vacía?
    if( *listaNombresRegistro == nullptr ){
        // Crea la nueva lista
        *listaNombresRegistro = gtk_list_store_new( 1, G_TYPE_STRING );
    }

    // Conecta con la base de datos
    database.open( databaseFile );

    // Limpia la lista
    gtk_list_store_clear( *listaNombresRegistro );

    // Obtiene la clave actual de productos registrados
    string consulta = "select distinct( " + registro + " ) as registro from registros_internos where " + registro + " != '';";
    database.query( consulta );
    if( results.size() > 0 ){
        for( auto *renglon : results ){
            string nombreRegistro = (* renglon)[ "registro" ];
            agregarElementoLista( listaNombresRegistro, nombreRegistro );
        }
    }
    
    database.close();

    // Agrega el completador
    *completador = gtk_entry_completion_new();
    gtk_entry_completion_set_text_column( *completador, 0);
    gtk_entry_completion_set_model( *completador, GTK_TREE_MODEL( *listaNombresRegistro ) );
}

// Agrega un elemento a la lista dada
void agregarElementoLista( GtkListStore **listaNombresRegistros, std::string nombre )
{
    // Agrega los nombres de registro de autocompletado
    GtkTreeIter iterador;
    gtk_list_store_append( *listaNombresRegistros, &iterador );
    gtk_list_store_set( *listaNombresRegistros, &iterador, 0, nombre.c_str(), -1 );
}

// Obtiene el folio actual en el que se encuentra el registro
void obtenerFolioActualInterno()
{
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Obtiene la clave actual de productos registrados
    string consulta = "select max( folio ) as folio from registros_internos;";
    database.query( consulta );
    if( results.size() > 0 ){
        // Obtiene el valor máximo en formato string
        string maxStr = (* results.at( 0 ))[ "folio" ];
        
        // Lo traduce a entero si el string no contiene la cadena "NULL"
        folioActual = ( maxStr.compare( "NULL" ) != 0 ? stoi( maxStr ) : 0 );
    }
    else{
        folioActual = 0;
    }
    
    database.close();
}

// Obtiene el folio actual en el que se encuentra el registro
void obtenerFolioActualPublico()
{
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Obtiene la clave actual de productos registrados
    string consulta = "select max( folio ) as folio from registros_publicos;";
    database.query( consulta );
    if( results.size() > 0 ){
        // Obtiene el valor máximo en formato string
        string maxStr = (* results.at( 0 ))[ "folio" ];
        
        // Lo traduce a entero si el string no contiene la cadena "NULL"
        folioActualPublico = ( maxStr.compare( "NULL" ) != 0 ? stoi( maxStr ) : 0 );
    }
    else{
        folioActualPublico = 0;
    }
    
    database.close();
}

// Obtiene los tickets registrados del día
void obtenerRegistrosPublicosPendientes()
{
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Obtiene los tickets del día
    string consulta = "select * from registros_publicos where pendiente = 1";
    database.query( consulta );
    if( results.size() > 0 ){
        for( auto renglon : results ){
    	    try{
                //Crea el nuevo registro
                TicketPublico *registroPublico = new TicketPublico( renglon, productos.buscarRegistroPorClave( stoi( ( *renglon )[ "clave_producto" ] ) ) );
    				
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
    database.open( databaseFile );
    
    // Obtiene los tickets del día
    string consulta = "select * from registros_internos where pendiente = 1";
    database.query( consulta );
    if( results.size() > 0 ){
        for( auto *renglon : results ){
    	    try{
                // Busca el producto y la empresa correspondiente
                Registro *producto = productos.buscarRegistroPorClave( stoi( (* renglon)[ "clave_producto" ] ) );
                Registro *empresa = empresas.buscarRegistroPorClave( stoi( (* renglon)[ "clave_empresa" ] ) );

    			//Crea el nuevo registro
                Ticket *ticket = new Ticket( renglon, producto, empresa );	
                
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

// Obtiene los usuarios registrados
void obtenerUsuariosRegistrados()
{
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Obtiene los tickets del día
    string consulta = "select * from usuarios";
    database.query( consulta );
    if( results.size() > 0 ){
        for( auto *renglon : results ){            
            try{
                //Crea el nuevo registro
                Usuario *usuario = new Usuario( renglon );
                    
                // Lo agrega al campo de registros internos pendientes
                registrosUsuarios.push_back( usuario );
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
    database.open( databaseFile );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "insert into registros_internos values( " << ticket -> obtenerFolio() << ", '" << ticket -> obtenerFecha() << "', " << ticket -> obtenerTipoRegistro() << " ," 
             << ticket -> obtenerEmpresa() -> obtenerClave() << ", " << ticket -> obtenerProducto() -> obtenerClave() << ", '" << ticket -> obtenerNumeroPlacas() << "', '" 
             << ticket -> obtenerNombreConductor() << "', " << ( ticket -> estaPesoBrutoEstablecido() ? "'" + ticket -> obtenerHoraEntrada() + "'" : "null" ) << ", "
             << ( ticket -> estaPesoTaraEstablecido() ? "'" + ticket -> obtenerHoraSalida() + "'" : "null" ) << ", "
             << ticket -> obtenerPesoBruto() << ", " << ticket -> estaPesoBrutoEstablecido() << ", " 
             << ticket -> obtenerPesoTara() << ", " << ticket -> estaPesoTaraEstablecido() << ", "
             << ticket -> obtenerDescuento() << ", " << ticket -> estaDescuentoEstablecido() << ", " << ticket -> permitirDescuento() << ", "
             << ticket -> obtenerPesoNeto() << ", " << ticket -> estaPesoNetoEstablecido() << ", '"
             << ticket -> obtenerObservaciones() << "', " << ticket -> esEntradaManual() << ", " << ticket -> estaPendiente() << ", '" << ticket -> obtenerNombreBasculista() << "' )";

    try{
        // Inserta el nuevo ticket
	    database.query( consulta.str() );
    }
    catch( runtime_error &re ){
	    cerr << re.what() << endl;
    }
    
    // Cierra la conexion
    database.close();

    // Agrega el ticket a la lista de registros pendientes
    registrosInternosPendientes.push_back( ticket );

    // Actualiza los registros actuales
    actualizarElementosLista( &listaNombresConductor, &completadorNombresConductor, "nombre_conductor" );
    actualizarElementosLista( &listaNumerosPlaca, &completadorNumerosPlaca, "numero_placas" );
}

void actualizarRegistroPendiente( Ticket *ticket )
{
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_internos set fecha = '" << ticket -> obtenerFecha() << "', "
             << "tipo_registro = " << ticket -> obtenerTipoRegistro() << ", " 
             << "clave_empresa = " << ticket -> obtenerEmpresa() -> obtenerClave() << ", " 
             << "clave_producto = " << ticket -> obtenerProducto() -> obtenerClave() << ", "
             << "numero_placas = '" << ticket -> obtenerNumeroPlacas() << "', "
             << "nombre_conductor = '" << ticket -> obtenerNombreConductor() << "', "
             << "hora_entrada = " << ( ticket -> estaPesoBrutoEstablecido() ? "'" + ticket -> obtenerHoraEntrada() + "'" : "null" ) << ", "
             << "hora_salida = " << ( ticket -> estaPesoTaraEstablecido() ? "'" + ticket -> obtenerHoraSalida() + "'" : "null" ) << ", "
             << "peso_bruto = " << ticket -> obtenerPesoBruto() << ", bruto_establecido = " << ticket -> estaPesoBrutoEstablecido() << ", " 
             << "peso_tara = " << ticket -> obtenerPesoTara() << ", tara_establecido = " << ticket -> estaPesoTaraEstablecido() << ", "
             << "descuento = " << ticket -> obtenerDescuento() << ", descuento_establecido = " << ticket -> estaDescuentoEstablecido() << ", " << "descuento_permitido = " << ticket -> permitirDescuento() << ", "
             << "peso_neto = " << ticket -> obtenerPesoNeto() << ", neto_establecido = " << ticket -> estaPesoNetoEstablecido() << ", "
             << "observaciones = '" << ticket -> obtenerObservaciones() << "', " 
             << "manual_activado = " << ticket -> esEntradaManual() << ", pendiente = " << ticket -> estaPendiente() << " "
             << "where folio = " << ticket -> obtenerFolio() << ";";

    try{
        // Inserta el nuevo ticket
	    database.query( consulta.str() );
    }
    catch( runtime_error &re ){
	    cerr << re.what() << endl;
    }
    
    // Cierra la conexion
    database.close();
}

void finalizarRegistro( Ticket *ticket, bool esNuevo )
{
    // Si es nuevo
    if( esNuevo ){
        crearRegistroPendiente( ticket );
    }

    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_internos set fecha = '" << ticket -> obtenerFecha() << "', "
             << "tipo_registro = " << ticket -> obtenerTipoRegistro() << ", " 
             << "clave_empresa = " << ticket -> obtenerEmpresa() -> obtenerClave() << ", " 
             << "clave_producto = " << ticket -> obtenerProducto() -> obtenerClave() << ", "
             << "numero_placas = '" << ticket -> obtenerNumeroPlacas() << "', "
             << "nombre_conductor = '" << ticket -> obtenerNombreConductor() << "', "
             << "hora_entrada = " << ( ticket -> estaPesoBrutoEstablecido() ? "'" + ticket -> obtenerHoraEntrada() + "'" : "null" ) << ", "
             << "hora_salida = " << ( ticket -> estaPesoTaraEstablecido() ? "'" + ticket -> obtenerHoraSalida() + "'" : "null" ) << ", "
             << "peso_bruto = " << ticket -> obtenerPesoBruto() << ", bruto_establecido = " << ticket -> estaPesoBrutoEstablecido() << ", " 
             << "peso_tara = " << ticket -> obtenerPesoTara() << ", tara_establecido = " << ticket -> estaPesoTaraEstablecido() << ", "
             << "descuento = " << ticket -> obtenerDescuento() << ", descuento_establecido = " << ticket -> estaDescuentoEstablecido() << ", " << "descuento_permitido = " << ticket -> permitirDescuento() << ", "
             << "peso_neto = " << ticket -> obtenerPesoNeto() << ", neto_establecido = " << ticket -> estaPesoNetoEstablecido() << ", "
             << "observaciones = '" << ticket -> obtenerObservaciones() << "', " 
             << "manual_activado = " << ticket -> esEntradaManual() << ", pendiente = " << ticket -> estaPendiente() << " "
             << "where folio = " << ticket -> obtenerFolio() << ";";
	    
    try{
        // Inserta el nuevo ticket
        database.query( consulta.str() );
        
        // Imprime el ticket
        ticket -> imprimir( empresa_razon_social, numeroFormatos, numeroCopias );
        
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

void crearRegistroPublicoPendiente( TicketPublico *registroPublico )
{    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "insert into registros_publicos values( " << registroPublico -> obtenerFolio() << ", '" << registroPublico -> obtenerFecha() << "', " << registroPublico -> obtenerTipoViaje() << ", " 
             << registroPublico -> obtenerProducto() -> obtenerClave() << ", '" << registroPublico -> obtenerNumeroPlacas() << "', '" 
             << registroPublico -> obtenerNombreConductor() << "', '" << registroPublico -> obtenerHoraEntrada() << "', '"
             << registroPublico -> obtenerHoraSalida() << "', "
             << registroPublico -> obtenerPesoBruto() << ", " 
             << registroPublico -> obtenerPesoTara() << ", "
             << registroPublico -> obtenerPesoNeto() << ", " 
             << registroPublico -> estaPesoBrutoEstablecido() << ", " 
             << registroPublico -> estaPesoTaraEstablecido() << ", " 
             << registroPublico -> estaPesoNetoEstablecido() << ", '"
             << registroPublico -> obtenerNombreBasculista() << "', " 
             << registroPublico -> estaPendiente() << ", " << registroPublico -> esEntradaManual() << ")";
    
    // Realiza la consulta definida anteriormente en la base de datos
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Se agrega el registro a los registros publicos pendientes
    registrosPublicosPendientes.push_back( registroPublico );
}

void actualizarRegistroPublicoPendiente( TicketPublico *registroPublico )
{
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_publicos set "
             << "tipo_viaje = " << registroPublico -> obtenerTipoViaje() << ", "
             << "clave_producto = " << registroPublico -> obtenerProducto() -> obtenerClave() << ", "
             << "numero_placas = '" << registroPublico -> obtenerNumeroPlacas() << "', "
             << "nombre_conductor = '" << registroPublico -> obtenerNombreConductor() << "', "
             << "hora_entrada = '" << registroPublico -> obtenerHoraEntrada() << "', "
             << "hora_salida = '" << registroPublico -> obtenerHoraSalida() << "', "
             << "peso_bruto = " << registroPublico -> obtenerPesoBruto() << ", " 
             << "peso_tara = " << registroPublico -> obtenerPesoTara() << ", "
             << "peso_neto = " << registroPublico -> obtenerPesoNeto() << ", " 
             << "bruto_establecido = " << registroPublico -> estaPesoBrutoEstablecido() << ", " 
             << "tara_establecido = " << registroPublico -> estaPesoTaraEstablecido() << ", " 
             << "neto_establecido = " << registroPublico -> estaPesoNetoEstablecido() << ", "
             << "nombre_basculista = '" << registroPublico -> obtenerNombreBasculista() << "', "
             << "pendiente = " << registroPublico -> estaPendiente() << ", "
             << "entrada_manual = " << registroPublico -> esEntradaManual() << " "
             << "where folio = " << registroPublico -> obtenerFolio();
    
    // Realiza la consulta definida anteriormente en la base de datos
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();
}

void finalizarRegistroPublico( TicketPublico *registroPublico, bool esNuevo )
{
    // ¿Es en registro nuevo?
    if( esNuevo ){
        // Se registra como pendiente
        crearRegistroPublicoPendiente( registroPublico );
    }

    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "update registros_publicos set "
             << "tipo_viaje = " << registroPublico -> obtenerTipoViaje() << ", "
             << "clave_producto = " << registroPublico -> obtenerProducto() -> obtenerClave() << ", "
             << "numero_placas = '" << registroPublico -> obtenerNumeroPlacas() << "', "
             << "nombre_conductor = '" << registroPublico -> obtenerNombreConductor() << "', "
             << "hora_entrada = '" << registroPublico -> obtenerHoraEntrada() << "', "
             << "hora_salida = '" << registroPublico -> obtenerHoraSalida() << "', "
             << "peso_bruto = " << registroPublico -> obtenerPesoBruto() << ", " 
             << "peso_tara = " << registroPublico -> obtenerPesoTara() << ", "
             << "peso_neto = " << registroPublico -> obtenerPesoNeto() << ", " 
             << "bruto_establecido = " << registroPublico -> estaPesoBrutoEstablecido() << ", " 
             << "tara_establecido = " << registroPublico -> estaPesoTaraEstablecido() << ", " 
             << "neto_establecido = " << registroPublico -> estaPesoNetoEstablecido() << ", "
             << "nombre_basculista = '" << registroPublico -> obtenerNombreBasculista() << "', "
             << "pendiente = " << registroPublico -> estaPendiente() << ", "
             << "entrada_manual = " << registroPublico -> esEntradaManual() << " "
             << "where folio = " << registroPublico -> obtenerFolio();
    
    // Realiza la consulta definida anteriormente en la base de datos
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();
    
    // Imprime el ticket
    registroPublico -> imprimir( empresa_razon_social );
    
    // Remueve el ticket de los registros pendientes
    registrosPublicosPendientes.remove( registroPublico );
    
    // Elimina el ticket
    delete registroPublico;
    registroPublico = nullptr;
}

void cargarOpcionesImpresion()
{
    ifstream archivo;

    // Intenta abrir el archivo
    archivo.open( "../resources/data/impresion.dat" );
    if( !archivo.is_open() ){
        numeroFormatos = 1;
        numeroCopias = 0;
        return;
    }

    // Lee el numero de copias
    archivo >> numeroFormatos;
    archivo >> numeroCopias;

    archivo.close();
}

void actualizarOpcionesImpresion()
{
    try{
        numeroFormatos = stoi( interfaz.obtenerTextoEntrada( "OpcionesImpresionFormatos" ) );
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "Numero de formatos no válido." );
    }

    try{
        numeroCopias = stoi( interfaz.obtenerTextoEntrada( "OpcionesImpresionCopias" ) );
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "Numero de copias no válido." );
    }

    // Archivo de configuración de impresión
    ofstream archivoConfig;

    // Se abre el archivo
    archivoConfig.open( "../resources/data/impresion.dat", ios_base::out );
    if( !archivoConfig ){
        throw runtime_error( "No se pudieron guardar los opciones de impresion." );
    }

    archivoConfig << (int)numeroFormatos << endl
                  << (int)numeroCopias << endl;

    archivoConfig.close();
}

// Guarda la configuración registrada
void guardarConfiguracion()
{
    try{
        actualizarOpcionesImpresion();
    }
    catch( exception &ia ){
        app_mostrar_error( ia.what() );
    }

    app_mostrar_mensaje( "Configuración actualizada." );
    irHacia( nullptr, (void *)"Inicio" );
}

// Limpia la lista de tickets dada
void limpiarRegistrosInternos( std::list< Ticket * > &registros )
{
    // Libera los elementos contenidos en la lista
    for( list< Ticket * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( ( *registro ) != nullptr ){
            delete ( *registro );
            ( *registro ) = nullptr;
        }
    }

    // Vacía la lista
    registros.clear();
}

// Limpia la lista de tickets dada
void limpiarRegistrosPublicos( std::list< TicketPublico * > &registros )
{
    // Libera los elementos contenidos en la lista
    for( list< TicketPublico * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( ( *registro ) != nullptr ){
            delete ( *registro );
            ( *registro ) = nullptr;
        }
    }

    // Vacía la lista
    registros.clear();
}

// 
void limpiarUsuariosRegistrados( std::list< Usuario * > &registros )
{
    // Libera los elementos contenidos en la lista
    for( list< Usuario * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( ( *registro ) != nullptr ){
            delete ( *registro );
            ( *registro ) = nullptr;
        }
    }

    // Vacía la lista
    registros.clear();
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

// Busca el usuario por su nombre de usuario (valga la redundancia)
Usuario *buscarUsuarioPorNombreUsuario( std::string nombreUsuario, std::list< Usuario * > &registros )
{
    for( list< Usuario * >::iterator registro = registros.begin(); registro != registros.end(); registro++ ){
        if( nombreUsuario.compare( (*registro) -> obtenerNombreUsuario() ) == 0 ){
            return (*registro);
        }
    }
    
    return nullptr;
}

// Actualiza si el admin es admin
void actualizarEstadoAdministrador()
{
    // Conecta con la base de datos
    database.open( databaseFile );

    unsigned int opcion = 0;
    if( interfaz.obtenerEstadoBotonToggle( "UsuarioAdministrador" ) == TRUE ){
        opcion = 1;
    }
    else{
        opcion = 0;
    }

    string consulta = "update usuarios set administrador = " + to_string( opcion ) + " where nombre_usuario = '" + usuarioConsultado -> obtenerNombreUsuario() + "';";
    database.query( consulta );

    // Cierra la conexión
    database.close();

    // Muestra mensaje
    if( opcion ){
        app_mostrar_mensaje( "Es administrador." );
    }
    else{
        app_mostrar_mensaje( "No es administrador." );
    }
}

// Elimina el usuario
void eliminarUsuario()
{
    database.open( databaseFile );

    string consulta = "select count( administrador ) from usuarios where administrador = 1;";
    database.query( consulta );
    if( results.size() > 0 ){
        // Obtiene la cantidad de usuarios que son administrador
        try{
            unsigned int numeroAdministradores = stoi( results.at( 0 ) -> at( 0 ) );

            // Si solo hay un administrador o menos
            if( numeroAdministradores <= 1 ){
                app_mostrar_mensaje( "Es necesario que al menos exista un administrador.");
                return;
            }
            
            // Procede a eliminar el usuario
            consulta = "delete from usuarios where nombre_usuario = '" + usuarioConsultado -> obtenerNombreUsuario() + "'";
            database.query( consulta );

            interfaz.ocultarElemento( "MensajeAlerta" );

            // ¿El usuario que se eliminó es el usuario actual?
            if( usuarioConsultado -> obtenerNombreUsuario().compare( usuario.obtenerNombreUsuario() ) == 0 ){
                app_mostrar_mensaje( "Su usuario ha sido eliminado. Una vez abandonado el programa o\n"
                                "cerrado sesión, no será capaz de acceder al programa.\n"
                                "En caso de volver a registrarse, no contará con permisos de\n"
                                "administrador. ¡Hasta pronto!" );
            }
            else{
                app_mostrar_mensaje( "Usuario eliminado correctamente." ); 
            }
            
            registrosUsuarios.remove( usuarioConsultado );
            delete usuarioConsultado;
            actualizarRegistrosUsuarios( registrosUsuarios, "ContenedorUsuarios" );
            irHacia( nullptr, (void *)"ConsultarUsuarios" );
        }
        catch( invalid_argument &ia ){
            app_mostrar_mensaje( "Ha ocurrido un error al intentar\neliminar el usuario." );
        }
    }
}

// Actualiza los registros publicos pendientes
void actualizarRegistrosUsuarios( list< Usuario * > &usuarios, std::string idContenedor )
{
    // Limpia el contenedor
    interfaz.removerElementosHijos( idContenedor );

    // Itera a través de la lista de tickets pendientes y crea los tickets necesarios
    unsigned int contador = 0;
    for( list< Usuario * >::iterator usuario = usuarios.begin(); usuario != usuarios.end(); usuario++, contador++ ){
        // Crea un elemento que será añadido a la interfaz
        Widget *elemento = new Widget();

        try{
            elemento -> cargarWidget( "../recursos/interfaces/ElementoUsuario.glade" );
            elemento -> establecerTextoEtiqueta( "ItemEntradaNombre", (*usuario) -> obtenerNombre() + " " + (*usuario) -> obtenerApellidos() );
            elemento -> establecerTextoEtiqueta( "ItemEntradaUsuario", (*usuario) -> obtenerNombreUsuario() );
            interfaz.insertarElementoAGrid( elemento, "ItemUsuario", idContenedor, 0, contador, 1, 1 );
        }
        catch( runtime_error &re ){
            cerr << re.what() << endl;
        }
        
        delete elemento;
    }
}

void generarCodigoRecuperacion()
{
    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio( 65, 90 );

    // Nombre de usuario
    string usuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "NombreUsuarioConsultado" ) );

    // Código
    stringstream codigo;
    for( size_t contador = 0; contador < 6; ++contador ){
        codigo << (char)intAleatorio( motor );
    }

    // Muestra el código generado
    app_mostrar_mensaje( "El código de recuperación es: " + codigo.str() );

    // Crea el hash para el código de recuperación
    SHA256 hashTool;
    string inputHash = hashTool( codigo.str() );

    // Registra el código en la base de datos
    database.open( databaseFile );
    database.query( "delete from codigos_recuperacion where id_usuario = '" + usuario + "'" );
    database.query( "insert into codigos_recuperacion values( '" + inputHash + "', '" + usuario + "')" );

    // Cierra la base de datos
    database.close();
}

// Valida el codigo de recuperación
void validarCodigoRecuperacion()
{
    try{
        string entradaUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaRecuperacionUsuario" ) );
        string codigoSeguridad = interfaz.obtenerTextoEntrada( "EntradaRecuperacionCodigoSeguridad" );

        // Obtiene el código en base el nombre de usuario
        database.open( databaseFile );
        database.query( "select codigo from codigos_recuperacion where nombre_usuario = '" + entradaUsuario + "';" );
        database.close();
        if( results.size() > 0 ){
            // Obtiene el código dado
            string codigo = results.at( 0 ) -> at( 0 );

            // Crea el hash para el código de recuperación introducido y lo compara
            SHA256 hashTool;
            codigoSeguridad = hashTool( codigoSeguridad );
            
            // Compara el hash del código introducido con el hash de la base de datos
            if( codigoSeguridad.compare( codigo ) != 0 ){
                app_mostrar_error( "El código introducido no coincide con ningún código de\nrecuperación asignado al usuario indicado." );
                return;
            }

            // Obtiene el usuario del que se desea reemplazar la contraseña
            nombreUsuario = entradaUsuario;
            codigoRecuperacion = codigoSeguridad;

            // Redirige a la vista para el reemplazo de contraseña
            irA( "ReemplazarContrasena", false );
        }
        else{
            app_mostrar_error( "No se encontró un código de recuperación." );
        }
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}