#include "Aplicacion.h"
#include "Database.h"
#include <gtk/gtk.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include "Widget.h"
#include "Funciones.h"
#include "Vistas.h"
#include <array>
using namespace std;

// Base de datos
Database database;

// Interfaz principal 
Widget interfaz;
string vistaActual;

void irHacia( GtkWidget *widget, gpointer ptr )
{
	mostrarVista( (char *)ptr );
}

// Completador a usar
GtkEntryCompletion * completador;

// Lista de tickets registrados del día en curso
unsigned int folioActual;
std::list< Ticket * > ticketsPendientes;
std::list< Ticket * > ticketsRegistrados;

//
ContenedorRegistros productos;
ContenedorRegistros empresas;

// Inicializa la aplicacion
void iniciar()
{
    try{
        // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
        interfaz.cargarWidget( "../resources/interfaces/Principal.glade" );
        interfaz.conectarSenal( "VentanaPrincipal", "destroy", G_CALLBACK( gtk_main_quit ), nullptr );
        
        // Establece la hora
        g_timeout_add( 1000, G_SOURCE_FUNC( actualizarTiempo ), nullptr );
        
        // Establece los parámetros correspondientes con cada vista
        interfaz.establecerBotonEtiqueta( "EnlaceRegistrarUsuarioRegresar", "¡Ya tengo un usuario!" );
        interfaz.establecerBotonEtiqueta( "EnlaceRecuperarContrasena", "No recuerdo mi constraseña" );
        interfaz.establecerBotonEtiqueta( "EnlaceRegistrarNuevoUsuario", "Crear un nuevo usuario" );
        interfaz.establecerBotonEtiqueta( "EnlaceRecuperarContrasenaRegresar", "Volver" );
        
        // Obtiene las empresas y los productos registrados en la base de datos
        productos.establecerNombrePlural( "productos" );
        productos.establecerNombreSingular( "producto" );
        productos.obtenerRegistros();
        interfaz.establecerCompletadorEntrada( "EntradaNombreProductoInterno", productos.obtenerCompletador() );
        
        empresas.establecerNombrePlural( "empresas" );
        empresas.establecerNombreSingular( "empresa" );
        empresas.obtenerRegistros();
        interfaz.establecerCompletadorEntrada( "EntradaNombreEmpresaInterno", empresas.obtenerCompletador() );
        
        // Obtiene los tickets registrados y pendientes
        obtenerTicketsRegistrados();
        
        // Manda a conectar todas las señales de las vistas
        conectarSenales();
        
        irHacia( nullptr, (void *)"IniciarSesion" );
    }
    catch( runtime_error &excepcion ){
        cerr << excepcion.what() << endl;
    }
}

// Obtiene los tickets registrados del día y el folio actual
void obtenerTicketsRegistrados()
{
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
    // Obtiene los tickets del día
    string consulta = "select * from tickets where fecha = date( 'now', 'localtime' ) or pendiente = 1";
    database.query( consulta );
    if( rows.size() > 0 ){
        for( Row *row : rows ){
            Ticket *ticket = new Ticket();
            ticket -> establecerFolio( stoi( row -> columns.at( 0 ) ) );
            ticket -> establecerFechaRegistro( row -> columns.at( 1 ) );
            ticket -> establecerEmpresa( empresas.buscarRegistroPorClave( stoi( row -> columns.at( 2 ) ) ) );
            ticket -> establecerProducto( productos.buscarRegistroPorClave( stoi( row -> columns.at( 3 ) ) ) );
            ticket -> establecerNumeroPlacas( row -> columns.at( 4 ) );
            ticket -> establecerNombreConductor( row -> columns.at( 5 ) );
            ticket -> establecerTipoRegistro( stoi( row -> columns.at( 6 ) ) );
            ticket -> establecerHoraEntrada( row -> columns.at( 7 ) );
            ticket -> establecerHoraSalida( row -> columns.at( 8 ) );
            ticket -> establecerPesoBruto( stod( row -> columns.at( 9 ) ) );
            ticket -> establecerPesoTara( stod( row -> columns.at( 10 ) ) );
            ticket -> establecerPesoNeto( stod( row -> columns.at( 11 ) ) );
            ticket -> establecerDescuento( stod( row -> columns.at( 12 ) ) );
            ticket -> establecerObservaciones( row -> columns.at( 13 ) );
            ticket -> establecerEntradaManual( stoi( row -> columns.at( 14 ) ) );
            ticket -> establecerPendiente( stoi( row -> columns.at( 15 ) ) );
            
            if( ticket -> estaPendiente() ){
                ticketsPendientes.push_back( ticket );
            }
            else{
                ticketsRegistrados.push_back( ticket );
            }
        }
    }
    
    // Obtiene la clave actual de productos registrados
    consulta = "select max( folio ) from tickets";
    database.query( consulta );
    if( rows.size() > 0 ){
        if( rows.at( 0 ) -> columns.at( 0 ).compare( "NULL" ) == 0 ){
            folioActual = 0;
        }
        else{
            folioActual = stoi( rows.at( 0 ) -> columns.at( 0 ) ); 
        }
    }
    else{
        folioActual = 0;
    }
}

// 
void agregarTicketPendiente( Ticket *ticket )
{
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
    // Consulta para el registro en la base de datos
    stringstream consulta;
    consulta << "insert into tickets values( " << ticket -> obtenerFolio() << ", '" << ticket -> obtenerFechaRegistro() << "', " << ticket -> obtenerEmpresa() -> obtenerClave() << ", "
             << ticket -> obtenerProducto() -> obtenerClave() << ", '" << ticket -> obtenerNumeroPlacas() << "', '" << ticket -> obtenerNombreConductor() << "', " << ticket -> obtenerTipoRegistro() 
             << ", '" << ticket -> obtenerHoraEntrada() << "', null, " << ticket -> obtenerPesoBruto() << ", 0, 0, " << ticket -> obtenerDescuento() 
             << ", '" << ticket -> obtenerObservaciones() << "', " << ticket -> esEntradaManual() << ", 1 )";
            
    // Inserta el nuevo ticket
    database.query( consulta.str() );
    ticketsPendientes.push_back( ticket );
    
    // Cierra la conexion
    database.close();
}

// Busca el ticket por folio
Ticket *buscarTicketPorFolio( unsigned int folio )
{
    for( list< Ticket * >::iterator ticket = ticketsPendientes.begin(); ticket != ticketsPendientes.end(); ticket++ ){
        if( (*ticket) -> obtenerFolio() == folio ){
            return (*ticket);
        }
    }
    
    return nullptr;
}

// Busca el ticket por el número de placa
Ticket *buscarTicketPorNumeroPlaca( std::string numeroPlacas )
{
    for( list< Ticket * >::iterator ticket = ticketsPendientes.begin(); ticket != ticketsPendientes.end(); ticket++ ){
        if( numeroPlacas.compare( (*ticket) -> obtenerNumeroPlacas() ) == 0 ){
            return (*ticket);
        }
    }
    
    return nullptr;
}

// Muestra la vista solicitada
void mostrarVista( string idVista )
{
    if( !vistaActual.empty() ){
        interfaz.ocultarElemento( vistaActual );
    }
    
    vistaActual = idVista;
    interfaz.mostrarElemento( vistaActual );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{
    // Señales de la vista de inicio de sesion
    interfaz.conectarSenal( "EnlaceRegistrarNuevoUsuario", "activate-link", G_CALLBACK( irHacia ), (void *)"RegistrarUsuario" );
    interfaz.conectarSenal( "EnlaceRecuperarContrasena", "activate-link", G_CALLBACK( irHacia ), (void *)"RecuperarContrasena" );
    interfaz.conectarSenal( "BotonIniciarSesion", "clicked", G_CALLBACK( irHacia ), (void *)"Inicio" );
    
    // Señales de la vista de registro de usuario
    interfaz.conectarSenal( "EnlaceRegistrarUsuarioRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    
    // Señales de recuperacion de contraseña
    interfaz.conectarSenal( "EnlaceRecuperarContrasenaRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    interfaz.conectarSenal( "BotonReemplazarContrasena", "clicked", G_CALLBACK( irHacia ), (void *)"ReemplazarContrasena" );
    
    // Vista ReemplazarContrasena
    interfaz.conectarSenal( "BotonCambiarContrasena", "clicked", G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    
    // Vista Inicio
    interfaz.conectarSenal( "BotonBascula", "clicked", G_CALLBACK( irHacia ), (void *)"Bascula" );
    
    // Vista bascula
    interfaz.conectarSenal( "BotonBasculaInterna", "clicked", G_CALLBACK( vistaBasculaInterna ), nullptr );
    
    // Vista báscula
    interfaz.conectarSenal( "EntradaSeguimiento", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "BotonBasculaRegresar", "clicked", G_CALLBACK( irHacia ), (void*)"Bascula" );
    
    // Nuevo para ticket interno
    interfaz.establecerBotonEtiqueta( "EnlaceRegresarInterno", "Regresar" );
    interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( irHacia ), (void *)"Tickets" );
    interfaz.conectarSenal( "EntradaNombreEmpresaInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNombreProductoInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNombreConductorInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNumeroPlacasInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "NoDescuentoInterno", "toggled", G_CALLBACK( habilitarDescuento ), nullptr );
    interfaz.conectarSenal( "RegistraEntrada", "toggled", G_CALLBACK( seleccionarTipoRegistro ), nullptr );
    interfaz.conectarSenal( "BotonLeerPesoTaraInterno", "clicked", G_CALLBACK( abrirLectorBascula ), (void *)LECTOR_PESO_TARA );
    interfaz.conectarSenal( "BotonPendienteInterno", "clicked", G_CALLBACK( registrarTicket ), nullptr );

    // Vista de registro de peso
    interfaz.conectarSenal( "BotonCancelarLectura", "clicked", G_CALLBACK( cerrarLectorBascula ), nullptr );
    interfaz.conectarSenal( "VentanaLectorPeso", "destroy", G_CALLBACK( cerrarLectorBascula ), nullptr );
    
    // Ventana que contiene un mensaje
    interfaz.conectarSenal( "BotonAceptar", "clicked", G_CALLBACK( aceptar ), nullptr );
}

// Obtiene la hora en un formato válido para la base de datos
string obtenerHora()
{
	// Obtiene el tiempo
    time_t t = time( NULL );
    struct tm tiempo = *localtime( &t );
    
	// Construye la hora en el formato de la base de datos
	stringstream hora;
	hora << setfill( '0' ) << setw( 2 ) << tiempo.tm_hour << ":" << setw( 2 ) << tiempo.tm_min << ":" << setw( 2 ) << tiempo.tm_sec << setfill( ' ' );
	
	// Devuelve la hora
	return hora.str();
}

// Obtiene la fecha en un formato válido para la base de datos
string obtenerFecha()
{
	// Obtiene el tiempo
    time_t t = time( NULL );
    struct tm tiempo = *localtime( &t );
    
    // Construye la fecha en el formato de la base de datos
    stringstream fecha;
    fecha << setfill( '0' ) << setw( 4 ) << (tiempo.tm_year + 1900) << "-" << setw( 2 ) << (tiempo.tm_mon + 1) << "-" << setw( 2 ) << tiempo.tm_mday << setfill( ' ' );
    
    // Devuelve la fecha
    return fecha.str();
}

// Actualiza el tiempo cada segundo
void actualizarTiempo( GtkWidget *widget, gpointer ptr )
{
    stringstream stringTiempo;
    string saludo;

    // Obtiene el tiempo
    time_t t = time( NULL );
    struct tm tiempo = *localtime( &t );
    
    // Fecha
    int dia = tiempo.tm_mday;
    int mes = tiempo.tm_mon;
    int anio = tiempo.tm_year + 1900;

    // Hora
    int hora = tiempo.tm_hour;
    int minuto = tiempo.tm_min;
    int segundo = tiempo.tm_sec;

    // Establece la hora
    stringTiempo << "Hora: " << setfill( '0' ) << setw( 2 ) << ( ( hora == 0 || hora == 12 ) ? 12 : hora % 12 ) << ":"
				 << setw( 2 ) << minuto << ":" << setw( 2 ) << segundo << ( ( hora < 12 ) ? " am" : " pm" ) << setfill( ' ' );
    interfaz.establecerTextoEtiqueta( "Hora", stringTiempo.str() );
	
    // Limpia el buffer
    stringTiempo.str( "" );
	
    // Establece la fecha
    stringTiempo << "Fecha: " << setfill( '0' ) << setw( 2 ) << dia << setfill( ' ' ) << " de " << meses.at( mes ) << " de " << anio;
	interfaz.establecerTextoEtiqueta( "Fecha", stringTiempo.str() );
    
    //¿La hora oscila entre las 12 y 11 de la mañana?
    if( hora < 12 ){
        // Establece el saludo a los buenos días
        saludo = "¡Buenos días! ¿Qué desea hacer hoy?";
    }
    // ¿La hora oscila entre las 12 y 8 de la tarde?
    else if( hora < 20 ){
        saludo = "¡Buenas tardes! ¿Qué desea hacer hoy?";
    }
    // La hora oscila entre las 9 de la noche y 4 de la mañana
    else{
        saludo = "¡Buenas noches! ¿Qué desea hacer hoy?";
    }
    
    // Establece el saludo
    interfaz.establecerTextoEtiqueta( "Saludo", saludo );
}
