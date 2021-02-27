#include "Aplicacion.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "Widget.h"
#include "Funciones.h"
#include "Senales.h"
#include "Vistas.h"
#include "LectorBascula.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Database.h"
#include "Senales.h"
#include "Sesion.h"
using namespace std;

// Interfaz principal 
Widget interfaz;
string vistaActual;

// Aplicacion activa
bool aplicacionActiva;

void irHacia( GtkWidget *widget, gpointer ptr )
{
	mostrarVista( (char *)ptr );
}

// Inicializa la aplicacion
void iniciar()
{
    // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
    interfaz.cargarWidget( "../resources/interfaces/Principal.glade" );
    interfaz.conectarSenal( "VentanaPrincipal", "destroy", G_CALLBACK( gtk_main_quit ), nullptr );

    // Establece la hora
    actualizarTiempo( nullptr, nullptr );
    g_timeout_add( 1000, G_SOURCE_FUNC( actualizarTiempo ), nullptr );

    // Conecta la señales base
    conectarSenalesBase();

    // Redirige hacia la interfaz de inicio de sesión
    irHacia( nullptr, (void *)"IniciarSesion" );

    // Si no existen usuarios registrados, se dirige a la sección para registrar uno
    if( !existenUsuariosRegistrados() ){
        irHacia( nullptr, (void *)"RegistrarUsuario" );
    }
}

// Carga la información necesaria
void cargarInformacion()
{
    // Carga el nombre de la empresa
    cargarNombreEmpresa();

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

    // Obtine los registros de completado de conductor y numero de placas
    actualizarElementosLista( &listaNombresConductor, &completadorNombresConductor, "nombre_conductor" );
    actualizarElementosLista( &listaNumerosPlaca, &completadorNumerosPlaca, "numero_placas" ); 
    interfaz.establecerCompletadorEntrada( "EntradaNombreConductorInterno", completadorNombresConductor );
    interfaz.establecerCompletadorEntrada( "EntradaNumeroPlacasInterno", completadorNumerosPlaca );

    // Carga la configuración de la bascula
    lectorBascula.cargarConfiguracion();
    cargarOpcionesImpresion();    
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
    interfaz.conectarSenal( enlaceRegistrarNuevoUsuario, G_CALLBACK( irHacia ), (void *)"RegistrarUsuario" );
    interfaz.conectarSenal( enlaceRecuperarContrasena, G_CALLBACK( irHacia ), (void *)"RecuperarContrasena" );
    interfaz.conectarSenal( botonIniciarSesion, iniciarSesion, nullptr );
    interfaz.conectarSenal( entradaContrasena, iniciarSesion, nullptr );
    
    // Señales de la vista de registro de usuario
    interfaz.conectarSenal( botonRegistrarUsuario, registrarUsuario, nullptr );
    interfaz.conectarSenal( enlaceRegistrarUsuarioRegresar, G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    
    // Señales de recuperacion de contraseña
    interfaz.conectarSenal( enlaceRecuperarContrasenaRegresar, G_CALLBACK( irHacia ), (void *)"IniciarSesion" );
    interfaz.conectarSenal( botonReemplazarContrasena, G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( entradaRecuperacionCodigoSeguridad, G_CALLBACK( validarCodigoRecuperacion ), nullptr );

    // Ventana que contiene un mensaje
    interfaz.conectarSenal( botonAceptar, G_CALLBACK( aceptar ), nullptr );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{   
    // Vista Inicio
    interfaz.conectarSenal( botonBascula, G_CALLBACK( irHacia ), (void *)"Bascula" );
    interfaz.conectarSenal( botonConfiguracion, G_CALLBACK( vistaConfiguracion ), nullptr );

    // Enlace hacia los créditos
    interfaz.conectarSenal( enlaceCreditos, G_CALLBACK( irHacia ), (void *)"Creditos" );
    interfaz.conectarSenal( enlaceCreditosRegresar, G_CALLBACK( irHacia ), (void *)"Inicio" );
    
    // Barra de usuario
    interfaz.conectarSenal( enlaceCuenta, G_CALLBACK( vistaCuenta ), nullptr );
    interfaz.conectarSenal( enlaceCuentaRegresar, G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( botonActualizarCuenta, G_CALLBACK( autorizarCambios ), nullptr );

    // Vista seleccion servicio
    interfaz.conectarSenal( enlaceTipoBasculaRegresar, G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( botonBasculaPublica, G_CALLBACK( vistaBasculaPublica ), nullptr );
    interfaz.conectarSenal( botonBasculaInterna, G_CALLBACK( vistaBasculaInterna ), nullptr );
    
    // Uso bascula de uso interno
    interfaz.conectarSenal( entradaSeguimiento, G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( enlaceBasculaRegresar, G_CALLBACK( irHacia ), (void*)"Bascula" );
    
    // Nuevo para ticket interno
    interfaz.conectarSenal( enlaceRegresarInterno, G_CALLBACK( internoCancelar ), nullptr );
    interfaz.conectarSenal( botonLeerPesoBrutoInterno, G_CALLBACK( vistaLeerPesoBruto ), nullptr );
    interfaz.conectarSenal( botonLeerPesoTaraInterno, G_CALLBACK( vistaLeerPesoTara ), nullptr);
    interfaz.conectarSenal( botonRegistrarPendienteInterno, G_CALLBACK( internoRegistrarPendiente ), nullptr );
    interfaz.conectarSenal( botonFinalizarPendienteInterno, G_CALLBACK( internoFinalizarPendiente ), nullptr );
    interfaz.conectarSenal( entradaNumeroPlacasInterno, G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( botonCalcularDescuento, G_CALLBACK( internoActualizarPesoNeto ), nullptr );
    interfaz.conectarSenal( opcionDescuentoInterno, G_CALLBACK( internoHabilitarDescuento ), nullptr );
    interfaz.conectarSenal( opcionRegistraEntrada, G_CALLBACK( internoSeleccionarTipo ), nullptr );

    // Nuevo para ticket publico
    interfaz.conectarSenal( entradaNumeroPlacasPublico, G_CALLBACK( convertirMayusculas ), nullptr );
    interfaz.conectarSenal( botonLeerPesoBrutoPublico, G_CALLBACK( vistaLeerPesoBrutoPublico ), nullptr );
    interfaz.conectarSenal( botonLeerPesoTaraPublico, G_CALLBACK( vistaLeerPesoTaraPublico ), nullptr );
    interfaz.conectarSenal( botonRegistrarPendientePublico, G_CALLBACK( publicoRegistrarPendiente ), nullptr );
    interfaz.conectarSenal( botonFinalizarPendientePublico, G_CALLBACK( publicoFinalizarPendiente ), nullptr );

    // Vista de registro de peso
    interfaz.conectarSenal( botonCancelarLectura, G_CALLBACK( lectorBasculaCerrar ), nullptr );
    interfaz.conectarSenal( ventanaLectorPeso, G_CALLBACK( lectorBasculaCerrar ), nullptr );
    
    // Vista que solicita la contrasena
    interfaz.conectarSenal( botonPermitirCambios, G_CALLBACK( actualizarDatosUsuario ), nullptr );

    // Vista de configuración
    interfaz.conectarSenal( enlaceConfiguracionRegresar, G_CALLBACK( irHacia ), (void *)"Inicio" );
    interfaz.conectarSenal( botonGuardarConfiguracion, G_CALLBACK( guardarConfiguracion ), nullptr );

    // Consultar registro interno
    interfaz.conectarSenal( imprimirRegistroInterno, G_CALLBACK( internoImprimirSeleccionado ), nullptr );
}

//
void conectarSenalesAdministrador()
{
    // Habilita los botones de consulta de registros
    interfaz.mostrarElemento( "BotonRegistros" );
	interfaz.mostrarElemento( "BotonUsuarios" );

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

    // Vista de administración de usuarios
    interfaz.conectarSenal( enlaceConsultarUsuariosRegresar, G_CALLBACK( regresarInicio ), nullptr );
    interfaz.conectarSenal( enlaceConsultarUsuarioRegresar, G_CALLBACK( regresarUsuarios ), nullptr );
	interfaz.conectarSenal( entradaConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
	interfaz.conectarSenal( botonConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
    interfaz.conectarSenal( botonObtenerCodigoRecuperacion, G_CALLBACK( generarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( usuarioAdministrador, G_CALLBACK( actualizarEstadoAdministrador ), nullptr );
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


