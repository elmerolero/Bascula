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
#include "GestorRegistros.h"
#include "GestorBasculas.h"
#include "Producto.h"
#include <random>
using namespace std;

// Interfaz principal 
Widget interfaz;

// Aplicacion activa
bool aplicacionActiva;

// Pila de vistas
vector< string > pilaVistas;

struct tm tiempo;

guint senal_domicilio_aceptar = 0;
guint senal_domicilio_cancelar = 0;

void iniciar( GtkApplication *aplicacion, gpointer informacion )
{
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
        conectar_senal( ventanaPrincipal, nullptr );

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

void empresa_leer_informacion( void ){
    cout << "empresa_leer_informacion" << endl;
    // Obtiene los datos de la empresa
    database.open( databaseFile );
    database.query( "select * from EmpresaPropia" );
    database.close();

    // ¿Hay empresa registrada?
    if( results.size() > 0 ){
        // Establece el nombre de la empresa
        empresa_razon_social = (* results.at( 0 ))[ "razon_social" ];
        empresa_rfc = ( (* results.at( 0 ))[ "RFC" ].compare( "null" ) != 0 ? (* results.at( 0 ))[ "RFC" ] : "" );
        empresa_imagen = ( (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ? (* results.at( 0 ))[ "imagen" ] : "" );

        // Actualiza el nombre de la empresa
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreEmpresa" ) ), empresa_razon_social.c_str() );
        gtk_widget_show( GTK_WIDGET( buscar_objeto( "NombreEmpresa" ) ) );
    }
    else{
        irA( "RegistrarEmpresa", true );
        empresa_inicio_senales_conectar();
        //irHacia( nullptr, (void *)"RegistrarEmpresa" );
    }
}

void empresa_inicio_senales_conectar(){
    // Razón social
    conectar_senal( botonInicioRazonSocial, nullptr );

    // Selección y edición de imagen
    senal_imagen_seleccionar = g_signal_connect ( buscar_objeto( "InicioAgregarFotoEmpresa" ), "clicked", G_CALLBACK( seleccionar_archivo ), nullptr );
    senal_imagen_guardar = g_signal_connect( buscar_objeto( "BotonGuardarEdicionImagen" ), "clicked", G_CALLBACK( empresa_escribir_imagen ), nullptr );
    senal_imagen_cancelar = g_signal_connect( buscar_objeto( "BotonCancelarEdicionImagen" ), "clicked", G_CALLBACK( imagen_cancelar ), nullptr );
    senal_imagen_descartar = g_signal_connect( buscar_objeto( "BotonSi" ), "clicked", G_CALLBACK( empresa_inicio_imagen_omitir_confirmacion ), nullptr );
    senal_imagen_continuar = g_signal_connect( buscar_objeto( "InicioContinuarConfiguracion" ), "clicked", G_CALLBACK( empresa_inicio_imagen ), nullptr );
    senal_imagen_omitir = g_signal_connect( buscar_objeto( "InicioOmitirFotoEmpresa" ), "clicked", G_CALLBACK( empresa_inicio_imagen_omitir ), nullptr );
    senal_domicilio_aceptar = g_signal_connect( buscar_objeto( "DomicilioAgregar"), "clicked", G_CALLBACK( empresa_domicilio_agregar ), nullptr );
    senal_domicilio_cancelar = g_signal_connect( buscar_objeto( "DomicilioCancelar" ), "clicked", G_CALLBACK( empresa_domicilio_cancelar ), nullptr );

    gtk_button_set_label( GTK_BUTTON( buscar_objeto( "DomicilioCancelar" ) ), "Omitir" );
}

void empresa_senales_desconectar(){
    g_signal_handler_disconnect( buscar_objeto( "InicioAgregarFotoEmpresa" ), senal_imagen_seleccionar );
    g_signal_handler_disconnect( buscar_objeto( "BotonGuardarEdicionImagen" ), senal_imagen_guardar );
    g_signal_handler_disconnect( buscar_objeto( "BotonCancelarEdicionImagen" ), senal_imagen_cancelar );
    g_signal_handler_disconnect( buscar_objeto( "BotonSi" ), senal_imagen_descartar );
    g_signal_handler_disconnect( buscar_objeto( "InicioContinuarConfiguracion" ), senal_imagen_continuar );
    g_signal_handler_disconnect( buscar_objeto( "InicioOmitirFotoEmpresa" ), senal_imagen_omitir );
    g_signal_handler_disconnect( buscar_objeto( "DomicilioAgregar" ), senal_domicilio_aceptar );
    g_signal_handler_disconnect( buscar_objeto( "DomicilioCancelar" ), senal_domicilio_cancelar );
    senal_imagen_seleccionar = 0;
    senal_imagen_guardar = 0;
    senal_movimiento = 0;
}

string empresa_validar_razon_social( string razon_social ){
    // Formato para validar la información
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ.\\s]*" );

    // Verifica que cumpla con las características dadas
    if( razon_social.empty() ){
        throw invalid_argument( "Es necesario registrar un nombre para su negocio." );
    }

    if( razon_social.size() > 100 ){
        throw invalid_argument( "El nombre excede la cantidad de caracteres permitidos." );
    }

    if( !regex_match( razon_social, formato ) ){
        throw invalid_argument( "El nombre introducido no es válido." );
    }

    return razon_social;
}

void empresa_inicio_registrar( GtkWidget *widget, gpointer info ){
    cout << "empresa_inicio_registrar" << endl;
    try{
        // Establece la razón social
        empresa_razon_social = empresa_validar_razon_social( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaInicioRazonSocial" ) ) ) );
    
        // 
        database.open( databaseFile );
        database.query( "select * from EmpresaPropia where id_empresa = 1" );
        if( results.empty() ){
            database.query( "insert into EmpresaPropia values( 1, '" + empresa_razon_social + "', null, null )" );
        }
        else{
            database.query( "update EmpresaPropia set razon_social = '" + empresa_razon_social + "' where id_empresa = 1" );
        }
        database.close();

        // Dirige hacia la siguiente vista
        irA( "AgregarImagenEmpresaPropia", false );         
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void empresa_inicio_imagen( GtkWidget *widget, gpointer info ){
    try{
        // Verifica que se estableció un archivo
        if( nombreTemporal.empty() ){
            app_mostrar_error( "No se ha establecido una imagen." );
            return;
        }

        // Guarda la ubicación del archivo
        database.open( databaseFile );
        database.query( "update EmpresaPropia set imagen = '" + nombreTemporal + "' where id_empresa = 1" );
        database.close();

        // Guarda la imagen
        cairo_surface_write_to_png( imagen_temporal, nombreTemporal.c_str() );
        if( imagen_temporal != nullptr ){
            cairo_surface_destroy( imagen_temporal );
            imagen_temporal = nullptr;
        }

        // Redirige a la siguiente vista
        irA( "AgregarDomicilio", false );
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void empresa_inicio_imagen_omitir( GtkWidget *widget, gpointer info ){
    // Si hay imagen seleccionada, lanza una alerta para descartar la imagen seleccionada
    if( !nombreTemporal.empty() ){
        app_alerta( nullptr, (char *)"¿Desea descartar la imagen que\nseleccionó anteriormente?" );
        return;
    }
    
    // Redirige a la siguiente vista
    irA( "AgregarDomicilio", false );
}

void empresa_inicio_imagen_omitir_confirmacion( GtkWidget *widget, gpointer info ){
    // Elimina el nombre del archivo
    empresa_razon_social.clear();

    // Si se estableció imagen
    if( !nombreTemporal.empty() ){
        // Elimina el archivo de imagen
        remove( nombreTemporal.c_str() );

        // Limpia el string
        nombreTemporal.clear();
    }

    // Destruye la imagen temporal
    if( imagen_temporal != nullptr ){
        cairo_surface_destroy( imagen_temporal );
        imagen_temporal = nullptr;
    }

    // Redirige a la siguiente vista
    irA( "AgregarDomicilio", false );
}

string domicilio_validar_lugar( std::string objeto, bool obligatorio ){
    // Formato que permite el nombre de un lugar
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );

    // Obtiene la información del campo solicitado
    string informacion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( objeto ) ) );
    string nombre_campo = gtk_widget_get_name( GTK_WIDGET( buscar_objeto( objeto ) ) );

    // Verifica si es obligario y no esta vacío
    if( obligatorio && informacion.empty() ){
        throw invalid_argument( "Es necesario registrar " + nombre_campo + "." );
    }

    // Se asegura que esté dentro del tamaño establecido
    if( informacion.size() > 100 ){
        throw invalid_argument( "El tamaño del " + nombre_campo + " excede el número de caracteres permitidos." );
    }

    // Verifica que solo tiene caracteres permitidos
    if( !regex_match( informacion, formato ) ){
        throw invalid_argument( "Se está registrando un " + nombre_campo + " inválido." );
    }

    return informacion;
}

string domicilio_validar_numero( std::string numero, bool obligatorio ){
    // Formato que permite el nombre de un lugar
    regex formato( "[0-9a-zA-ZÑñáéíóúÁÉÍÓÚ\\s-]*" );

    // Obtiene la información del campo solicitado
    string informacion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( numero ) ) );
    string nombre_campo = gtk_widget_get_name( GTK_WIDGET( buscar_objeto( numero ) ) );

    // Verifica si es obligario y no esta vacío
    if( obligatorio && informacion.empty() ){
        throw invalid_argument( "Es necesario registrar " + nombre_campo + "." );
    }

    // Se asegura que esté dentro del tamaño establecido
    if( informacion.size() > 10 ){
        throw invalid_argument( "El tamaño del " + nombre_campo + " excede el número de caracteres permitidos." );
    }

    // Verifica que solo tiene caracteres permitidos
    if( !regex_match( informacion, formato ) ){
        throw invalid_argument( "Se está registrando un " + nombre_campo + " inválido." );
    }

    return informacion;
}

std::string domicilio_validar_descripcion( std::string descripcion ){
    // Formato que permite el nombre de un lugar
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );

    // Obtiene la información del campo solicitado
    string informacion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( descripcion ) ) );

    // Se asegura que esté dentro del tamaño establecido
    if( informacion.size() > 300 ){
        throw invalid_argument( "El tamaño de la descripción excede el número de caracteres permitidos." );
    }

    // Verifica que solo tiene caracteres permitidos
    if( !regex_match( informacion, formato ) ){
        throw invalid_argument( "Se está registrando una descripción no inválida." );
    }

    return informacion;
}

void empresa_domicilio_agregar( GtkWidget *widget, gpointer info ){
    try{
        // Consulta 
        stringstream consulta;

        // Establece la consulta a la base de datos
        consulta << "insert into DomicilioEmpresaPropia values( null, 1, '"
                 << domicilio_validar_lugar( "DomicilioCalle", true ) << "', '"
                 << domicilio_validar_numero( "DomicilioNumero", true ) << "', '"
                 << domicilio_validar_numero( "DomicilioNumeroInterior", false ) << "', '"
                 << domicilio_validar_lugar( "DomicilioColonia", true ) << "', "
                 << stoi( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "DomicilioCodigoPostal" ) ) ) ) << ", '"
                 << domicilio_validar_lugar( "DomicilioMunicipio", true ) << "', '"
                 << domicilio_validar_lugar( "DomicilioEstado" , true ) << "', '"
                 << domicilio_validar_lugar( "DomicilioPais", true ) << "', '"
                 << domicilio_validar_descripcion( "DomicilioDescripcion" )
                 << "' )";
        
        cout << consulta.str() << endl;

        // Realiza la consulta
        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        app_mostrar_mensaje( "Datos de la empresa registrados exitosamente." );
        irA( "Inicio", true ); 

        empresa_senales_desconectar();
    }
    catch( invalid_argument &ia ){
        // Obtiene el mensaje de error
        string what = ia.what();

        // Verifica que sea error stoi
        if( what.compare( "stoi" ) == 0 ){
            app_mostrar_error( "Código Postal no válido." );
        }
        else{
            app_mostrar_error( what );
        }
    }
}

void empresa_domicilio_cancelar( GtkWidget *widget, gpointer info ){
    //
    empresa_senales_desconectar();

    app_mostrar_mensaje( "Puede agregar o actualizar los datos de\nla empresa mas adelante en configuración." );
    irA( "Inicio", true );
}

void empresa_escribir_imagen( GtkWidget *widget, gpointer info ){
    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio;

    // Crea el nombre del archivo
    stringstream s;
    s << empresa_razon_social << intAleatorio( motor );
    string empresa_imagen = "../recursos/imagenes/empresas/" + crearHash( s.str() ) + ".png";

    // Guarda la selección temporal
    imagen_temporal_guardar( empresa_imagen );

    // Carga la nueva imagen
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenEmpresaInicio" ) ), imagen_temporal );
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
    conectar_senal( enlaceRegresar, nullptr );

    // Señales de la vista de inicio de sesion
    conectar_senal( enlaceRegistrarNuevoUsuario, (void *)"RegistrarUsuario" );
    conectar_senal( enlaceRecuperarContrasena, (void *)"RecuperarContrasena" );
    conectar_senal( botonIniciarSesion, nullptr );
    conectar_senal( entradaContrasena, nullptr );
    
    // Señales de la vista de registro de usuario
    conectar_senal( botonRegistrarUsuario, nullptr );
    
    // Señales de recuperacion de contraseña
    conectar_senal( botonReemplazarContrasena, nullptr );
    conectar_senal( entradaRecuperacionCodigoSeguridad, nullptr );
    conectar_senal( botonCambiarContrasena, nullptr );
	conectar_senal( entradaReemplazarConfirmacion, nullptr );

    // Ventana que contiene un mensaje
    conectar_senal( botonAceptar, nullptr );
}

// Conecta las señales de cada una de las vistas
void conectarSenales()
{
    // Vista Inicio
    conectar_senal( botonBascula, (void *)"Bascula" );
    conectar_senal( enlaceCreditos, (void *)"Creditos" );
    conectar_senal( enlaceCuenta, nullptr );
    conectar_senal( botonConfiguracion, nullptr );
    
    // Barra de usuario
    conectar_senal( botonActualizarCuenta, nullptr );

    // Vista seleccion servicio
    conectar_senal( botonBasculaPublica, nullptr );
    conectar_senal( botonBasculaInterna, nullptr );
    
    // Nuevo para ticket interno
    conectar_senal( botonLeerPesoBrutoInterno, nullptr );
    conectar_senal( botonLeerPesoTaraInterno, nullptr);
    conectar_senal( botonRegistrarPendienteInterno, nullptr );
    conectar_senal( botonFinalizarPendienteInterno, nullptr );
    conectar_senal( entradaNumeroPlacasInterno, nullptr );
    conectar_senal( botonCalcularDescuento, nullptr );
    conectar_senal( opcionDescuentoInterno, nullptr );
    conectar_senal( opcionRegistraEntrada, nullptr );

    // Nuevo para ticket publico
    conectar_senal( entradaNumeroPlacasPublico, nullptr );
    conectar_senal( botonLeerPesoBrutoPublico, nullptr );
    conectar_senal( botonLeerPesoTaraPublico, nullptr );
    conectar_senal( botonRegistrarPendientePublico, nullptr );
    conectar_senal( botonFinalizarPendientePublico, nullptr );
     
    // Vista que solicita la contrasena
    conectar_senal( botonPermitirCambios, nullptr );

    // Vista de configuración
    conectar_senal( botonGuardarConfiguracion, nullptr );

    // Consultar registro interno
    conectar_senal( imprimirRegistroInterno, nullptr );

    // Vista de configuración de báscula
    conectar_senal( botonGuardarBascula, nullptr );
    conectar_senal( basculaSeleccionada, nullptr );
    conectar_senal( botonBasculaAgregar, nullptr );
    conectar_senal( botonBasculaEliminar, nullptr );

    // Ventana lectora de peso
    conectar_senal( ventanaLectorPeso, nullptr );
    conectar_senal( botonCancelarPeso, nullptr );
}

//
void conectarSenalesAdministrador(){
    // Vista de registro de empresa (primer inicio)

    // Vista de registros
    conectar_senal( botonRegistros, (void *)"Registros" );
    conectar_senal( botonUsuarios, nullptr );

    // Vista Registros
    conectar_senal( botonRegistrosEmpresas, (void *)(&empresas) );
    conectar_senal( botonRegistrosProductos, (void *)(&productos) );
    conectar_senal( botonRegistrosPesajesInternos, nullptr );
    conectar_senal( botonRegistrosBasculaPublica, nullptr );

    // Vista de listado de registros
    conectar_senal( botonRegistroCancelarEdicion, nullptr );
    conectar_senal( botonNo, nullptr );

    // Vista de ticket interno
    conectar_senal( eliminarRegistroInterno, nullptr );

    // Vista de ticket interno
    conectar_senal( eliminarRegistroPublico, nullptr );

    // Vista de administración de usuarios
	conectar_senal( entradaConsultarUsuario, nullptr );
	conectar_senal( botonConsultarUsuario, nullptr );
    conectar_senal( botonObtenerCodigoRecuperacion, nullptr );
    conectar_senal( usuarioAdministrador, nullptr );
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
