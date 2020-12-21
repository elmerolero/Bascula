#include "Aplicacion.h"
#include <gtk/gtk.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Widget.h"
#include "Funciones.h"
#include "Vistas.h"
#include "LectorBascula.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Database.h"
#include "Sesion.h"
using namespace std;

// Interfaz principal 
Widget interfaz;
string vistaActual;

// Aplicacion activa
bool aplicacionActiva;

int numeroMagico = 5;

void irHacia( GtkWidget *widget, gpointer ptr )
{
	mostrarVista( (char *)ptr );
}

// Inicializa la aplicacion
void iniciar()
{
    try{
        // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
        interfaz.cargarWidget( "../resources/interfaces/Principal.glade" );
        interfaz.conectarSenal( "VentanaPrincipal", "destroy", G_CALLBACK( gtk_main_quit ), nullptr );
        actualizarTiempo( nullptr, nullptr );
        
        //
        GtkCssProvider *provider = gtk_css_provider_new ();
    	GdkDisplay *display = gdk_display_get_default();
    	GdkScreen *screen = gdk_display_get_default_screen(display);
    	gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    	gtk_css_provider_load_from_data ( provider, "#IniciarSesion{background-color: #000000;}", -1, NULL );

        // ¿Primer inicio?
        primerInicio();

        // Obtiene las empresas y los productos registrados en la base de datos
        productos.establecerNombrePlural( "productos" );
        productos.establecerNombreSingular( "producto" );
        productos.obtenerRegistros();
        
        empresas.establecerNombrePlural( "empresas" );
        empresas.establecerNombreSingular( "empresa" );
        empresas.obtenerRegistros();
        
        // Obtiene los tickets registrados y pendientes
        obtenerFolioActualInterno();
        obtenerFolioActualPublico();
        obtenerRegistrosInternosPendientes();
        obtenerRegistrosPublicosPendientes();

        // Establece la hora
        g_timeout_add( 1000, G_SOURCE_FUNC( actualizarTiempo ), nullptr );

        // Conecta la señales base
        conectarSenalesBase();

        // Carga la configuración de la bascula
        lectorBascula.cargarConfiguracion();
        cargarOpcionesImpresion();
        
        if( !esInicio ){
            irHacia( nullptr, (void *)"IniciarSesion" );
        }
    }
    catch( runtime_error &excepcion ){
        cerr << excepcion.what() << endl;
    }
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

// Son necesarias para que funcione el inicio de sesión
void conectarSenalesBase()
{
    // Señales de la vista de inicio de sesion
    interfaz.conectarSenal( "EnlaceRegistrarNuevoUsuario", "activate-link", G_CALLBACK( irHacia ), (void *)"RegistrarUsuario" );
    interfaz.conectarSenal( "EnlaceRecuperarContrasena", "activate-link", G_CALLBACK( irHacia ), (void *)"RecuperarContrasena" );
    interfaz.conectarSenal( "BotonIniciarSesion", "clicked", iniciarSesion, nullptr );
    interfaz.conectarSenal( "EntradaContrasena", "activate", iniciarSesion, nullptr );
    
    // Señales de la vista de registro de usuario
    interfaz.conectarSenal( "BotonRegistrarUsuario", "clicked", G_CALLBACK( registrarUsuario ), nullptr );
    interfaz.conectarSenal( "EnlaceRegistrarUsuarioRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    
    // Señales de recuperacion de contraseña
    interfaz.conectarSenal( "EnlaceRecuperarContrasenaRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    interfaz.conectarSenal( "BotonReemplazarContrasena", "clicked", G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( "EntradaRecuperacionCodigoSeguridad", "activate", G_CALLBACK( validarCodigoRecuperacion ), nullptr );

    // Ventana que contiene un mensaje
    interfaz.conectarSenal( "BotonAceptar", "clicked", G_CALLBACK( aceptar ), nullptr );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{   
    // Vista Inicio
    interfaz.conectarSenal( "BotonBascula", "clicked", G_CALLBACK( irHacia ), (void *)"Bascula" );
    interfaz.conectarSenal( "BotonConfiguracion", "clicked", G_CALLBACK( vistaConfiguracion ), nullptr );

    // Enlace hacia los créditos
    interfaz.conectarSenal( "EnlaceCreditos", "activate-link", G_CALLBACK( irHacia ), (void *)"Creditos" );
    interfaz.conectarSenal( "EnlaceCreditosRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Inicio" );
    
    // Barra de usuario
    interfaz.conectarSenal( "EnlaceCuenta", "activate-link", G_CALLBACK( vistaCuenta ), nullptr );
    interfaz.conectarSenal( "EnlaceCuentaRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( "BotonActualizarCuenta", "clicked", G_CALLBACK( autorizarCambios ), nullptr );

    // Vista seleccion servicio
    interfaz.conectarSenal( "EnlaceTipoBasculaRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( "BotonBasculaPublica", "clicked", G_CALLBACK( vistaBasculaPublica ), nullptr );
    interfaz.conectarSenal( "BotonBasculaInterna", "clicked", G_CALLBACK( vistaBasculaInterna ), nullptr );
    
    // Uso bascula de uso interno
    interfaz.conectarSenal( "EntradaSeguimiento", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "EnlaceBasculaRegresar", "activate-link", G_CALLBACK( irHacia ), (void*)"Bascula" );
    
    // Nuevo para ticket interno
    interfaz.conectarSenal( "EntradaNumeroPlacasInterno", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( "BotonCalcularDescuento", "clicked", G_CALLBACK( internoActualizarPesoNeto ), nullptr );
    interfaz.conectarSenal( "NoDescuentoInterno", "toggled", G_CALLBACK( internoHabilitarDescuento ), nullptr );
    interfaz.conectarSenal( "RegistraEntrada", "toggled", G_CALLBACK( internoSeleccionarTipo ), nullptr );

    // Nuevo para ticket publico
    interfaz.conectarSenal( "EntradaNumeroPlacasPublico", "insert-text", G_CALLBACK( convertirMayusculas ), nullptr );

    // Vista de registro de peso
    interfaz.conectarSenal( "BotonCancelarLectura", "clicked", G_CALLBACK( lectorBasculaCerrar ), nullptr );
    interfaz.conectarSenal( "VentanaLectorPeso", "destroy", G_CALLBACK( lectorBasculaCerrar ), nullptr );
    
    // Vista que solicita la contrasena
    interfaz.conectarSenal( "BotonPermitirCambios", "clicked", G_CALLBACK( actualizarDatosUsuario ), nullptr );

    // Vista de configuración
    interfaz.conectarSenal( "EnlaceConfiguracionRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( "BotonGuardarConfiguracion", "clicked", G_CALLBACK( guardarConfiguracion ), nullptr );
}

//
void conectarSenalesAdministrador()
{
    // Vista de registro de empresa (primer inicio)
    interfaz.conectarSenal( "BotonRegistrarEmpresaPropia", "clicked", G_CALLBACK( registrarEmpresa ), nullptr );

    // Vista de registros
    interfaz.conectarSenal( "BotonRegistros", "clicked", G_CALLBACK( irHacia ), (void *)"Registros" );
    interfaz.conectarSenal( "BotonUsuarios", "clicked", G_CALLBACK( vistaConsultarUsuarios ), nullptr );

    // Vista Registros
    interfaz.conectarSenal( "EnlaceRegresarRegistros", "activate-link", G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( "BotonRegistrosEmpresas", "clicked", G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
    interfaz.conectarSenal( "BotonRegistrosProductos", "clicked", G_CALLBACK( vistaRegistrosProductos ), nullptr );
    interfaz.conectarSenal( "BotonRegistrosPesajesInternos", "clicked", G_CALLBACK( vistaConsultarPesajesInternos ), nullptr );
    interfaz.conectarSenal( "BotonRegistrosBasculaPublica", "clicked", G_CALLBACK( vistaConsultarPesajesPublicos ), nullptr );

    // Vista de consulta de registros
    interfaz.conectarSenal( "BotonEditarRegistro", "clicked", G_CALLBACK( vistaRegistroEditar ), nullptr );
    interfaz.conectarSenal( "BotonCancelarEdicionRegistro", "clicked", G_CALLBACK( registroCancelarEdicion ), nullptr );
    interfaz.conectarSenal( "EnlaceConsultarRegistrosRegresar", "activate-link", G_CALLBACK( irHacia ), (void *)"Registros" );
    interfaz.conectarSenal( "BotonNo", "clicked", G_CALLBACK( cancelarOperacion ), nullptr );
    interfaz.conectarSenal( "BotonEliminarRegistro", "clicked", G_CALLBACK( alertaEliminarRegistro ), nullptr );

    // Vista de ticket interno
    interfaz.conectarSenal( "RegresarInterno", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );
    interfaz.conectarSenal( "EliminarRegistroInterno", "clicked", G_CALLBACK( internoAlertaEliminar ), nullptr );

    // Vista de ticket interno
    interfaz.conectarSenal( "RegresarPublico", "activate-link", G_CALLBACK( irHacia ), (void *)"Pesajes" );
    interfaz.conectarSenal( "EliminarRegistroPublico", "clicked", G_CALLBACK( internoAlertaEliminar ), nullptr );
}

// Muestra una alerta con el mensaje indicado
void alerta( GtkWidget *widget, gpointer data )
{
    interfaz.establecerTextoEtiqueta( "MensajeAlerta", (char *) data );
    interfaz.mostrarElemento( "VentanaSiNo" );
}

// Abre la ventana de mensaje con el mensaje indicado
void mostrarMensaje( string mensaje )
{
    interfaz.establecerTextoEtiqueta( "DialogoMensaje", mensaje );
    interfaz.mostrarElemento( "VentanaMensaje" );
}

// Cierra la ventana de mensaje
void aceptar( GtkWidget *widget, gpointer ptr )
{
	interfaz.ocultarElemento( "VentanaMensaje" );
}

// Actualiza el nombre de la empresa
void actualizarNombreEmpresa()
{
    // Establece los nombres de la empresa
    interfaz.establecerTextoEtiqueta( "NombreEmpresa", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa1", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa2", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa3", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa4", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa5", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa6", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa7", nombreEmpresa );
    interfaz.establecerTextoEtiqueta( "NombreEmpresa8", nombreEmpresa );
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

string obtenerFecha( unsigned int dia, unsigned int mes, unsigned int anio )
{
    // Variable
    stringstream fecha;
    
    // Convierte la fecha
    fecha << setfill( '0' ) << setw( 4 ) << anio << "-" << setw( 2 ) << mes << "-" << setw( 2 ) << dia << setfill( ' ' );

    // Retorna la fecha creada
    return fecha.str();
}

void primerInicio()
{
    ifstream archivo;

    // Intenta abrir el archivo
    archivo.open( nombreArchivo );
    if( archivo.is_open() ){
        archivo.close();
        esInicio = false;
        return; // Regresa no hay que establecer opciones adicionales
    }

    // Es primer inicio
    esInicio = true;

    // Creamos el archivo
    database.open( nombreArchivo );

    // Creamos las tablas
    database.query( desencriptar( databaseTables, numeroMagico ) );

    // Cerramos la base de datos
    database.close();

    // Redirigimos a la página de registro
    irHacia( nullptr, (void *)"RegistrarUsuario" );
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


