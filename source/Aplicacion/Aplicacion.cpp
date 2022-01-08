#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include "Widget.h"
#include "Database.h"
#include "Senales.h"
#include "Sesion.h"
#include "Imagen.h"
#include "Vistas.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "GestorRegistros.h"
#include "GestorBasculas.h"
#include "Producto.h"
#include "Empresa.h"
#include <random>
using namespace std;

// Interfaz principal 
Widget interfaz;

// Aplicacion activa
bool aplicacionActiva;

// Pila de vistas
vector< string > pilaVistas;

struct tm tiempo;

void iniciar( GtkApplication *aplicacion, gpointer informacion ){
    try{
        // Elemento para obtener los errores que ocurran al cargar el widget
        GError *error = nullptr;

        // Crea el builder
        builder = gtk_builder_new();
        if( gtk_builder_add_from_file( builder, "../recursos/interfaces/Principal.glade", &error ) == 0 ){
            // Lanza una excepcion
            throw runtime_error( "<- Aplicacion::iniciar() Error al cargar el archivo." ); 
        }

        interfaz.establecerBuildable( builder );

        // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
        GObject *objeto = gtk_builder_get_object( builder, "VentanaPrincipal" );
        gtk_application_add_window( aplicacion, GTK_WINDOW( objeto ) );
        gtk_widget_show( GTK_WIDGET( objeto ) );
        conectar_senal( ventanaPrincipal, G_CALLBACK( gtk_widget_destroy ), nullptr );

        // Agrega los eventos a área edición
        objeto = gtk_builder_get_object( builder, ( "AreaEdicion" ) );
        gtk_widget_add_events( GTK_WIDGET( objeto ), GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK );
        senal_presionado = g_signal_connect( GTK_WIDGET( objeto), "button-press-event", G_CALLBACK( leer_mouse ), nullptr );
        senal_movimiento = g_signal_connect( GTK_WIDGET( objeto ), "motion-notify-event", G_CALLBACK( imagen_mover ), nullptr );

        // Agraga un GtkAdjustment a la barra de escalado
        objeto = buscar_objeto( "BarraEscaladoImagen" );
        GtkAdjustment *ajuste = gtk_adjustment_new( 100, 50, 150, 0, 1, 0 );
        gtk_range_set_adjustment( GTK_RANGE( objeto ), ajuste );
        g_signal_connect( GTK_RANGE( objeto ), "value-changed", G_CALLBACK( cambiar_tamanio ), nullptr );

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
    catch( runtime_error &re ){
        string error = "<- Aplicacion::iniciar() ";
        throw runtime_error( error + re.what() );
    }
}

// Carga la información necesaria
void cargarInformacion()
{
    try{
        // Carga el nombre de la empresa
        empresa_leer_informacion();

        // Obtiene las empresas y los productos registrados en la base de datos
        /*productos.establecerNombrePlural( "productos" );
        productos.establecerNombreSingular( "Producto" );
        productos.obtenerRegistros();*/

        /*empresas.establecerNombrePlural( "empresas" );
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
        gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), completadorNombresConductor ); 
        gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), completadorNumerosPlaca  );

        // Carga la configuración de la bascula
        cargarOpcionesImpresion();
        basculaObtenerPuertosDisponibles();
        basculaObtenerRegistros();*/
    }
    catch( runtime_error re ){
        throw re;
    }
}

// Son necesarias para que funcione el inicio de sesión
void conectarSenalesBase()
{
    // Enlace regresar
    conectar_senal( enlaceRegresar, G_CALLBACK( regresarVista ), nullptr );

    // Señales de la vista de inicio de sesion
    conectar_senal( enlaceRegistrarNuevoUsuario, G_CALLBACK( irHacia ), (void *)"RegistrarUsuario" );
    conectar_senal( enlaceRecuperarContrasena, G_CALLBACK( irHacia ), (void *)"RecuperarContrasena" );
    conectar_senal( botonIniciarSesion, G_CALLBACK( iniciarSesion ), nullptr );
    conectar_senal( entradaContrasena, iniciarSesion, nullptr );
    
    // Señales de la vista de registro de usuario
    conectar_senal( botonRegistrarUsuario, registrarUsuario, nullptr );
    
    // Señales de recuperacion de contraseña
    conectar_senal( botonReemplazarContrasena, G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    conectar_senal( entradaRecuperacionCodigoSeguridad, G_CALLBACK( validarCodigoRecuperacion ), nullptr );
    conectar_senal( botonCambiarContrasena, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );
	conectar_senal( entradaReemplazarConfirmacion, G_CALLBACK( cambiarContrasenaUsuario ), nullptr );

    // Ventana que contiene un mensaje
    conectar_senal( botonAceptar, G_CALLBACK( app_aceptar_mensaje ), nullptr );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{
    // Vista Inicio
    conectar_senal( botonBascula, G_CALLBACK( irHacia ), (void *)"Bascula" );
    conectar_senal( enlaceCreditos, G_CALLBACK( irHacia ), (void *)"Creditos" );
    conectar_senal( enlaceCuenta, G_CALLBACK( vistaCuenta ), nullptr );
    conectar_senal( botonConfiguracion, G_CALLBACK( vistaConfiguracion ), nullptr );
    
    // Barra de usuario
    conectar_senal( botonActualizarCuenta, G_CALLBACK( autorizarCambios ), nullptr );

    // Vista seleccion servicio
    conectar_senal( botonBasculaPublica, G_CALLBACK( vistaBasculaPublica ), nullptr );
    conectar_senal( botonBasculaInterna, G_CALLBACK( vistaBasculaInterna ), nullptr );
    
    // Nuevo para ticket interno
    conectar_senal( botonLeerPesoBrutoInterno, G_CALLBACK( vistaLeerPesoBruto ), nullptr );
    conectar_senal( botonLeerPesoTaraInterno, G_CALLBACK( vistaLeerPesoTara ), nullptr);
    conectar_senal( botonRegistrarPendienteInterno, G_CALLBACK( internoRegistrarPendiente ), nullptr );
    conectar_senal( botonFinalizarPendienteInterno, G_CALLBACK( internoFinalizarPendiente ), nullptr );
    conectar_senal( entradaNumeroPlacasInterno, G_CALLBACK( convertirMayusculas ), nullptr );
    conectar_senal( botonCalcularDescuento, G_CALLBACK( internoActualizarPesoNeto ), nullptr );
    conectar_senal( opcionDescuentoInterno, G_CALLBACK( internoHabilitarDescuento ), nullptr );
    conectar_senal( opcionRegistraEntrada, G_CALLBACK( internoSeleccionarTipo ), nullptr );

    // Nuevo para ticket publico
    conectar_senal( entradaNumeroPlacasPublico, G_CALLBACK( convertirMayusculas ), nullptr );
    conectar_senal( botonLeerPesoBrutoPublico, G_CALLBACK( vistaLeerPesoBrutoPublico ), nullptr );
    conectar_senal( botonLeerPesoTaraPublico, G_CALLBACK( vistaLeerPesoTaraPublico ), nullptr );
    conectar_senal( botonRegistrarPendientePublico, G_CALLBACK( publicoRegistrarPendiente ), nullptr );
    conectar_senal( botonFinalizarPendientePublico, G_CALLBACK( publicoFinalizarPendiente ), nullptr );
     
    // Vista que solicita la contrasena
    conectar_senal( botonPermitirCambios, G_CALLBACK( actualizarDatosUsuario ), nullptr );

    // Vista de configuración
    conectar_senal( botonGuardarConfiguracion, G_CALLBACK( guardarConfiguracion ), nullptr );

    // Consultar registro interno
    conectar_senal( imprimirRegistroInterno, G_CALLBACK( internoImprimirSeleccionado ), nullptr );

    // Vista de configuración de báscula
    //conectar_senal( botonGuardarBascula, nullptr );
    conectar_senal( basculaSeleccionada, G_CALLBACK( vistaBascula ), nullptr );
    conectar_senal( botonBasculaAgregar, G_CALLBACK( vistaBasculaEdicion ), nullptr );
    conectar_senal( botonBasculaEliminar, G_CALLBACK( vistaBasculaEliminar ), nullptr );

    // Ventana lectora de peso
    conectar_senal( ventanaLectorPeso, G_CALLBACK( basculaCerrarLector ), nullptr );
    conectar_senal( botonCancelarPeso, G_CALLBACK( basculaCerrarLector ), nullptr );
}

//
void conectarSenalesAdministrador(){
    // Vista de registro de empresa (primer inicio)

    // Vista de registros
    conectar_senal( botonRegistros, G_CALLBACK( irHacia ), (void *)"Registros" );
    conectar_senal( botonUsuarios, G_CALLBACK( vistaConsultarUsuarios ), nullptr );

    // Vista Registros
    conectar_senal( botonRegistrosEmpresas, G_CALLBACK( vista_registros ), (void *)(&empresas) );
    conectar_senal( botonRegistrosProductos, G_CALLBACK( producto_listar_registros ), (void *)(&productos) );
    conectar_senal( botonRegistrosPesajesInternos, G_CALLBACK( vistaConsultarPesajesInternos ), nullptr );
    conectar_senal( botonRegistrosBasculaPublica, G_CALLBACK( vistaConsultarPesajesPublicos ), nullptr );

    // Vista de listado de registros
    //conectar_senal( botonRegistroCancelarEdicion, nullptr );
    conectar_senal( botonNo, G_CALLBACK( cancelarAccion ), nullptr );

    // Vista de ticket interno
    //conectar_senal( eliminarRegistroInterno, nullptr );

    // Vista de ticket interno
    //conectar_senal( eliminarRegistroPublico, nullptr );

    // Vista de administración de usuarios
	conectar_senal( entradaConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
	conectar_senal( botonConsultarUsuario, G_CALLBACK( vistaConsultarUsuario ), nullptr );
    conectar_senal( botonObtenerCodigoRecuperacion, G_CALLBACK( generarCodigoRecuperacion ), nullptr );
    conectar_senal( usuarioAdministrador, G_CALLBACK( actualizarEstadoAdministrador ), nullptr );
}

// Muestra una alerta con el mensaje indicado
void app_alerta( GtkWidget *widget, gpointer data ){
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeAlerta" ) ), (char *)data );
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );
}

// Indica un error
void app_mostrar_error( string error ){
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "MensajeError" ) ), error.c_str() );
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "MensajeError" ) ) );
}

// Abre la ventana de mensaje con el mensaje indicado
void app_mostrar_mensaje( string mensaje ){
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "DialogoMensaje" ) ), mensaje.c_str() );
    gtk_widget_show( GTK_WIDGET( buscar_objeto( "VentanaMensaje" ) ) );
}

// Cierra la ventana de mensaje
void app_aceptar_mensaje( GtkWidget *widget, gpointer ptr ){
    gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaMensaje" ) ) );
}

// Redirige hacia la vista dada
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
    ocultar_elemento( "MensajeError" );

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

    mostrar_elemento( idVista );
    establecer_texto_etiqueta( "Titulo", obtener_nombre_elemento( idVista ) );
    
    if( pilaVistas.size() <= 1 ){
        ocultar_elemento( "EnlaceRegresar" );
    }
    else{
        mostrar_elemento( "EnlaceRegresar" );
    }
}

void tiempo_actualizar(){
    time_t t = time( NULL );
    tiempo = *localtime( &t );
}

// Construye la hora
string tiempo_leer_hora( bool HORA_CORTA ){ 
    // Actualiza la hora
    tiempo_actualizar();

    // Donde se registra el string con la hora
    stringstream hora;

    hora << setfill( '0' ) << setw( 2 ) << ( HORA_CORTA ? tiempo.tm_hour : ( ( tiempo.tm_hour == 0 || tiempo.tm_hour == 12 ) ? 12 : tiempo.tm_hour % 12 ) ) << ":"
                 << setw( 2 ) << tiempo.tm_min << ":" << setw( 2 ) << tiempo.tm_sec << " " << ( HORA_CORTA ? "" : ( tiempo.tm_hour < 12 ) ? "am" : "pm" ) << setfill( ' ' );

    // Devuelve la hora
    return hora.str();
}

string tiempo_leer_fecha_larga(){
    // Actualiza la hora
    tiempo_actualizar();

    // Guarda la fecha
    stringstream fecha;

    // Construye la fecha
    fecha << setfill( '0' ) << setw( 2 ) << tiempo.tm_mday << setfill( ' ' ) << " de " << meses.at( tiempo.tm_mon ) << " de " << tiempo.tm_year + 1900;

    // Devuelve la fecha
    return fecha.str();
}

// Construye la fecha en el formato de la base de datos
string tiempo_leer_fecha_corta(){
    // Actualiza la hora
    tiempo_actualizar();

    stringstream fecha;
    fecha << setfill( '0' ) << setw( 4 ) << (tiempo.tm_year + 1900) << "-" << setw( 2 ) << (tiempo.tm_mon + 1) << "-" << setw( 2 ) << tiempo.tm_mday << setfill( ' ' );
    
    // Devuelve la fecha
    return fecha.str();
}

// Construye una fecha a partir de la información dada
string tiempo_construir_fecha( unsigned int dia, unsigned int mes, unsigned int anio ){
    // Variable
    stringstream fecha;
    
    // Convierte la fecha
    fecha << setfill( '0' ) << setw( 4 ) << anio << "-" << setw( 2 ) << mes << "-" << setw( 2 ) << dia << setfill( ' ' );

    // Retorna la fecha creada
    return fecha.str();
}

// Actualiza el tiempo cada segundo
void actualizarTiempo( GtkWidget *widget, gpointer ptr ){
    // Saludo
    string saludo = "";

    GObject *item = gtk_builder_get_object( builder, "Hora" );
    gtk_label_set_text( GTK_LABEL( item ), ("Hora: " + tiempo_leer_hora( 0 )).c_str() );
	
    // Establece la fecha
    item = gtk_builder_get_object( builder, "Fecha" );
    gtk_label_set_text( GTK_LABEL( item ), ("Fecha: " + tiempo_leer_fecha_larga() ).c_str() );
    
    //¿La hora oscila entre las 12 y 11 de la mañana?
    if( tiempo.tm_hour < 12 ){
        // Establece el saludo a los buenos días
        saludo = "¡Buenos días! ¿Qué desea hacer hoy?";
    }
    // ¿La hora oscila entre las 12 y 8 de la tarde?
    else if( tiempo.tm_hour < 20 ){
        saludo = "¡Buenas tardes! ¿Qué desea hacer hoy?";
    }
    // La hora oscila entre las 9 de la noche y 4 de la mañana
    else{
        saludo = "¡Buenas noches! ¿Qué desea hacer hoy?";
    }
    
    // Obtiene el item solicitado
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "Saludo" ) ), saludo.c_str() );
}
