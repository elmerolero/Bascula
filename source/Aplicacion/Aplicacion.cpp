#include <stdexcept>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include "Widget.h"
#include "Funciones.h"
#include "Senales.h"
#include "Vistas.h"
#include "GestorBasculas.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Database.h"
#include "Senales.h"
#include "Sesion.h"
#include "Windows.h"
using namespace std;

// Interfaz principal 
Widget interfaz;

// Aplicacion activa
bool aplicacionActiva;

// Pila de vistas
vector< string > pilaVistas;

Signal senalDibujo{ "AreaImpresion", "draw", 0 };

gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  guint width, height;
  GdkRGBA color;
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (widget);

  width = gtk_widget_get_allocated_width (widget);
  height = gtk_widget_get_allocated_height (widget);

  gtk_render_background (context, cr, 0, 0, width, height);

  //cairo_arc (cr,
    //         width / 2.0, height / 2.0,
      //       MIN (width, height) / 2.0,
        //     0, 2 * G_PI);

  cairo_move_to( cr, width / 2.0, height /  2.0 );
  cairo_line_to( cr, 50, 50 );
  cairo_text_path( cr, "Hola" );

  gtk_style_context_get_color (context,
                               gtk_style_context_get_state (context),
                               &color);
  gdk_cairo_set_source_rgba (cr, &color);

  cairo_fill (cr);

 return FALSE;
}


void iniciar( GtkApplication *aplicacion, gpointer informacion )
{
    // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
    interfaz.cargarWidget( "../resources/interfaces/Principal.glade" );
    gtk_application_add_window( aplicacion, GTK_WINDOW( interfaz.obtenerObjeto( "VentanaPrincipal" ) ) );
    interfaz.mostrarElemento( "VentanaPrincipal" );
    interfaz.conectarSenal( ventanaPrincipal, G_CALLBACK( gtk_widget_destroy ), nullptr );

    // Establece la hora
    actualizarTiempo( nullptr, nullptr );
    g_timeout_add( 1000, G_SOURCE_FUNC( actualizarTiempo ), nullptr );

    // Conecta la señales base
    conectarSenalesBase();

    // Redirige hacia la interfaz de inicio de sesión
    irA( "IniciarSesion", false );

    // Si no existen usuarios registrados, se dirige a la sección para registrar uno
    if( !existenUsuariosRegistrados() ){
        irA( "RegistrarUsuario", false );
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
    cargarOpcionesImpresion();
    basculaObtenerPuertosDisponibles();
    basculaObtenerRegistros();
}

void mostrarMensajeError( string mensajeError )
{
    interfaz.establecerTextoEtiqueta( "MensajeError", mensajeError );
    interfaz.mostrarElemento( "MensajeError" );
}

// Son necesarias para que funcione el inicio de sesión
void conectarSenalesBase()
{
    interfaz.conectarSenal( senalDibujo, G_CALLBACK( draw_callback ), nullptr );
    // Enlace regresar
    interfaz.conectarSenal( enlaceRegresar, G_CALLBACK( regresarVista ), nullptr );

    // Señales de la vista de inicio de sesion
    interfaz.conectarSenal( enlaceRegistrarNuevoUsuario, G_CALLBACK( irHacia ), (void *)"RegistrarUsuario" );
    interfaz.conectarSenal( enlaceRecuperarContrasena, G_CALLBACK( irHacia ), (void *)"RecuperarContrasena" );
    interfaz.conectarSenal( botonIniciarSesion, iniciarSesion, nullptr );
    interfaz.conectarSenal( entradaContrasena, iniciarSesion, nullptr );
    
    // Señales de la vista de registro de usuario
    interfaz.conectarSenal( botonRegistrarUsuario, registrarUsuario, nullptr );
    
    // Señales de recuperacion de contraseña
    interfaz.conectarSenal( botonReemplazarContrasena, G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( entradaRecuperacionCodigoSeguridad, G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( botonCambiarContrasena, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );
	interfaz.conectarSenal( entradaReemplazarConfirmacion, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

    // Ventana que contiene un mensaje
    interfaz.conectarSenal( botonAceptar, G_CALLBACK( aceptar ), nullptr );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{
    interfaz.conectarSenal( senalDibujo, G_CALLBACK( draw_callback ), nullptr );

    // Vista Inicio
    interfaz.conectarSenal( botonBascula, G_CALLBACK( irHacia ), (void *)"Bascula" );
    interfaz.conectarSenal( enlaceCreditos, G_CALLBACK( irHacia ), (void *)"Creditos" );
    interfaz.conectarSenal( enlaceCuenta, G_CALLBACK( vistaCuenta ), nullptr );
    interfaz.conectarSenal( botonConfiguracion, G_CALLBACK( vistaConfiguracion ), nullptr );
    
    // Barra de usuario
    interfaz.conectarSenal( botonActualizarCuenta, G_CALLBACK( autorizarCambios ), nullptr );

    // Vista seleccion servicio
    interfaz.conectarSenal( botonBasculaPublica, G_CALLBACK( vistaBasculaPublica ), nullptr );
    interfaz.conectarSenal( botonBasculaInterna, G_CALLBACK( vistaBasculaInterna ), nullptr );
    
    // Nuevo para ticket interno
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
    
    // Vista que solicita la contrasena
    interfaz.conectarSenal( botonPermitirCambios, G_CALLBACK( actualizarDatosUsuario ), nullptr );

    // Vista de configuración
    interfaz.conectarSenal( botonGuardarConfiguracion, G_CALLBACK( guardarConfiguracion ), nullptr );

    // Consultar registro interno
    interfaz.conectarSenal( imprimirRegistroInterno, G_CALLBACK( internoImprimirSeleccionado ), nullptr );

    // Vista de configuración de báscula
    interfaz.conectarSenal( botonGuardarBascula, G_CALLBACK( basculaGuardar ), nullptr );
    interfaz.conectarSenal( basculaSeleccionada, G_CALLBACK( vistaBascula ), nullptr );
    interfaz.conectarSenal( botonBasculaAgregar, G_CALLBACK( vistaBasculaEdicion ), nullptr );
    interfaz.conectarSenal( botonBasculaEliminar, G_CALLBACK( vistaBasculaEliminar ), nullptr );

    // Ventana lectora de peso
    interfaz.conectarSenal( ventanaLectorPeso, G_CALLBACK( basculaCerrarLector ), nullptr );
    interfaz.conectarSenal( botonCancelarPeso, G_CALLBACK( basculaCerrarLector ), nullptr );
}

//
void conectarSenalesAdministrador()
{
    // Habilita los botones de consulta de registros
    interfaz.mostrarElemento( "BotonRegistros" );
	interfaz.mostrarElemento( "BotonUsuarios" );

    // Vista de registro de empresa (primer inicio)
    interfaz.conectarSenal( botonRegistrarEmpresaPropia, G_CALLBACK( registrarEmpresa ), nullptr );

    // Vista de registros
    interfaz.conectarSenal( botonRegistros, G_CALLBACK( irHacia ), (void *)"Registros" );
    interfaz.conectarSenal( botonUsuarios, G_CALLBACK( vistaConsultarUsuarios ), nullptr );

    // Vista Registros
    interfaz.conectarSenal( botonRegistrosEmpresas, G_CALLBACK( vistaRegistrosEmpresas ), nullptr );
    interfaz.conectarSenal( botonRegistrosProductos, G_CALLBACK( vistaRegistrosProductos), nullptr );
    interfaz.conectarSenal( botonRegistrosPesajesInternos, G_CALLBACK( vistaConsultarPesajesInternos ), nullptr );
    interfaz.conectarSenal( botonRegistrosBasculaPublica, G_CALLBACK( vistaConsultarPesajesPublicos ), nullptr );

    // Vista de listado de registros
    interfaz.conectarSenal( botonRegistroCancelarEdicion, G_CALLBACK( regresarVista ), nullptr );
    interfaz.conectarSenal( botonRegistroCancelarNuevo, G_CALLBACK( regresarVista ), nullptr );
    interfaz.conectarSenal( botonNo, G_CALLBACK( cancelarAccion ), nullptr );

    // Vista de ticket interno
    interfaz.conectarSenal( eliminarRegistroInterno, G_CALLBACK( internoAlertaEliminar ), nullptr );

    // Vista de ticket interno
    interfaz.conectarSenal( eliminarRegistroPublico, G_CALLBACK( internoAlertaEliminar ), nullptr );

    // Vista de administración de usuarios
	interfaz.conectarSenal( entradaConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
	interfaz.conectarSenal( botonConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
    interfaz.conectarSenal( botonObtenerCodigoRecuperacion, G_CALLBACK( generarCodigoRecuperacion ), nullptr );
    interfaz.conectarSenal( usuarioAdministrador, G_CALLBACK( actualizarEstadoAdministrador ), nullptr );
}

// Muestra una alerta con el mensaje indicado
void alerta( GtkWidget *widget, gpointer data )
{
    interfaz.establecerTextoEtiqueta( "MensajeAlerta", (char *)data );
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

void irHacia( GtkWidget *widget, gpointer vista )
{
    // Oculta cualquier mensaje de error creado
    interfaz.ocultarElemento( "MensajeError" );

    // Redirige hacia la vista dada
	mostrarVista( (char *)vista );
}

void irA( string vista, bool reiniciarPila )
{
    // Oculta cualquier mensaje de error dado
    interfaz.ocultarElemento( "MensajeError" );

    // Reinicia la pila si así se indica
    if( reiniciarPila ){
        reiniciarPilaVistas();
    }

    // Redirige hacia la vista dada
    mostrarVista( vista );
}

// Limpiar pila vistas
void reiniciarPilaVistas( void )
{
    // Oculta cada una de las vistas registradas en la pila
    for( string vista : pilaVistas ){
        interfaz.ocultarElemento( vista );
    }

    // Limpia la pila de vistas
    pilaVistas.clear();

    // Oculta el enlace para regresar a la vista anterior
    interfaz.ocultarElemento( "EnlaceRegresar" );
}

// Regresa hacia la vista anterior
void regresarVista( void )
{
    // ¿Hay elementos en la pila de vistas?
    if( pilaVistas.size() > 1 ){
        // Oculta el elemento actual
        interfaz.ocultarElemento( pilaVistas.back() );

        // Lo retira de la pila
        pilaVistas.pop_back();
    }

    // Se dirige hacia la vista anterior
    string vista = pilaVistas.back();
    pilaVistas.pop_back();
    mostrarVista( vista );
}

// 
void mostrarVista( string idVista )
{
    if( !pilaVistas.empty() ){
        interfaz.ocultarElemento( pilaVistas.back() );
    }

    pilaVistas.push_back( idVista );

    interfaz.mostrarElemento( idVista );
    interfaz.establecerTextoEtiqueta( "Titulo", interfaz.obtenerNombreWidget( idVista ) );
    
    if( pilaVistas.size() <= 1 ){
        interfaz.ocultarElemento( "EnlaceRegresar" );
    }
    else{
        interfaz.mostrarElemento( "EnlaceRegresar" );
    }
}
