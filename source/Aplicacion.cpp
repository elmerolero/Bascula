#include "Aplicacion.h"
#include "Database.h"
#include <gtk/gtk.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include "Widget.h"
#include "Funciones.h"
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

// Lista de empresas
std::list< Registro * > empresas;
GtkListStore *listaEmpresas;
unsigned int claveEmpresaActual;

// Lista de productos
std::list< Registro * > productos;
GtkListStore *listaProductos;
unsigned int claveProductoActual;

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
        
        // Establece el completador que será usado
        completador = gtk_entry_completion_new();
        interfaz.establecerCompletadorEntrada( "EntradaCampo", completador );
        gtk_entry_completion_set_text_column( completador, 0);
        
        // Obtiene los tickets registrados y pendientes
        obtenerTicketsRegistrados();
        
        // Obtiene los productos registrados en la base de datos
        obtenerProductosRegistrados();
        
        // Manda a conectar todas las señales de las vistas
        conectarSenales();
        
        irHacia( nullptr, (void *)"BasculaPublica" );
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
    string consulta = "select * from tickets where fecha = date( 'now', 'localtime' )";
    database.query( consulta );
    if( rows.size() > 0 ){
        for( Row *row : rows ){
            Ticket *ticket = new Ticket();
            ticket -> establecerFolio( stoi( row -> columns.at( 0 ) ) );
            ticket -> establecerFechaRegistro( row -> columns.at( 1 ) );
            ticket -> establecerEmpresa( buscarRegistro( stoi( row -> columns.at( 3 ) ), empresas ) );
            ticket -> establecerProducto( buscarRegistro( stoi( row -> columns.at( 4 ) ), productos ) );
            ticket -> establecerNumeroPlacas( row -> columns.at( 5 ) );
            ticket -> establecerNombreConductor( row -> columns.at( 6 ) );
            ticket -> establecerHoraEntrada( row -> columns.at( 7 ) );
            ticket -> establecerHoraSalida( row -> columns.at( 8 ) );
            ticket -> establecerPesoBruto( stod( row -> columns.at( 9 ) ) );
            ticket -> establecerPesoTara( stod( row -> columns.at( 10 ) ) );
            ticket -> establecerPesoNeto( stod( row -> columns.at( 11 ) ) );
            ticket -> establecerObservaciones( row -> columns.at( 12 ) );
            ticket -> establecerPendiente( stoi( row -> columns.at( 13 ) ) );
            
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
            folioActual = 1;
        }
        else{
            folioActual = stoi( rows.at( 0 ) -> columns.at( 0 ) ); 
        }
    }
    else{
        folioActual = 1;
    }
}

// Obtiene la lista de empresas registradas
void obtenerEmpresasRegistradas()
{
    ;
}

// Obtiene la lista de productos registrados
void obtenerProductosRegistrados()
{
    // Conecta con la base de datos
    database.open( "libcurlmbs.dll" );
    
    // Obtiene la lista de productos registrados
    string consulta = "select * from productos";
    database.query( consulta );
    if( rows.size() > 0 ){
        for( Row *row : rows ){
            Registro *registro = new Registro();
            registro -> establecerClave( stoi( row -> columns.at( 0 ) ) );
            registro -> establecerNombre( row -> columns.at( 1 ) );
            
            productos.push_back( registro );
        }
    }
    
    // Obtiene la clave actual de productos registrados
    consulta = "select max( clave_producto ) from productos";
    database.query( consulta );
    if( rows.size() > 0 ){
        if( rows.at( 0 ) -> columns.at( 0 ).compare( "NULL" ) == 0 ){
            claveProductoActual = 1;
        }
        else{
            claveProductoActual = stoi( rows.at( 0 ) -> columns.at( 0 ) ); 
        }
    }
    else{
        claveProductoActual = 1;
    }
    
    listaProductos = gtk_list_store_new( 1, G_TYPE_STRING );
    GtkTreeIter iterador;
    for( Registro *reg : productos ){
        gtk_list_store_append( listaProductos, &iterador );
        gtk_list_store_set( listaProductos, &iterador, 0, reg -> obtenerNombre().c_str(), -1 );
    }
    
    // Cierra la conexión
    database.close();
}

// Busca el registro por clave
Registro *buscarRegistro( unsigned int clave, list< Registro * > &registros )
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
Registro *buscarRegistro( string nombre, list< Registro * > &registros )
{
    // Recorre la lista de productos
    for( list< Registro * >::iterator iterador = registros.begin(); iterador != registros.end(); iterador++ ){
        if( nombre.compare( (*iterador) -> obtenerNombre() ) == 0 ){
            return (*iterador);
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
    
    // Báscula pública
    interfaz.conectarSenal( "BotonBasculaPublicaNuevo", "clicked", G_CALLBACK( basculaPublicaNuevo ), nullptr );
    
    // Nuevo para ticket interno
    interfaz.conectarSenal( "EntradaNombreEmpresaInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNombreProductoInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNombreConductorInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EntradaNumeroPlacasInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.establecerBotonEtiqueta( "EnlaceRegresarInterno", "Regresar" );
    interfaz.conectarSenal( "EnlaceRegresarInterno", "activate-link", G_CALLBACK( irHacia ), (void *)"BasculaPublica" );
    //interfaz.conectarSenal( "BotonLeerPesoBrutoInterno", G_CALLBACK( irHacia ), (v ), 

    // Vista de registro de peso
    interfaz.conectarSenal( "BotonRegistrarPeso", "clicked", G_CALLBACK( gtk_widget_hide ), nullptr );
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
    fecha << setfill( '0' ) << setw( 4 ) << (tiempo.tm_year + 1900) << "-" << setw( 2 ) << tiempo.tm_mon << "-" << setw( 2 ) << tiempo.tm_mday << setfill( ' ' );
    
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
