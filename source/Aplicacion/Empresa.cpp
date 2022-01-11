#include "Empresa.h"
#include "Aplicacion.h"
#include "Imagen.h"
#include "GestorRegistros.h"
#include "Funciones.h"
#include "Senales.h"
#include <iostream>
#include <sstream>
#include <random>
using namespace std;

Signal botonInicioRazonSocial{ "BotonRegistrarEmpresaPropia", "clicked", 0 };

Signal senal_empresa_inicio_imagen_seleccionar = { "InicioSeleccionarFotoEmpresa", "clicked", 0 };
Signal senal_empresa_inicio_imagen_guardar = { "InicioAgregarFotoEmpresa", "clicked", 0 };
Signal senal_empresa_inicio_imagen_cancelar = { "InicioOmitirFotoEmpresa", "clicked", 0 };
Signal senal_empresa_domicilio_guardar = { "DomicilioAgregar", "clicked", 0 };
Signal senal_empresa_domicilio_cancelar = { "DomicilioCancelar", "clicked", 0 };

void empresa_inicio_senales_conectar(){
    // Razón social
    conectar_senal( botonInicioRazonSocial, G_CALLBACK( empresa_inicio_registrar ), nullptr );
    
    // 
    conectar_senal( senal_empresa_inicio_imagen_guardar, G_CALLBACK( empresa_inicio_imagen ), nullptr );
    conectar_senal( senal_empresa_inicio_imagen_cancelar, G_CALLBACK( empresa_inicio_imagen_omitir ), nullptr );
    conectar_senal( botonSi, G_CALLBACK( empresa_inicio_imagen_omitir_confirmacion ), nullptr );
    conectar_senal( senal_empresa_domicilio_guardar, G_CALLBACK( empresa_domicilio_agregar ), nullptr );
    conectar_senal( senal_empresa_domicilio_cancelar, G_CALLBACK( empresa_domicilio_cancelar ), nullptr );

    // Selección y edición de imagen
    conectar_senal( senal_empresa_inicio_imagen_seleccionar, G_CALLBACK( seleccionar_archivo ), nullptr );
    conectar_senal( senal_imagen_guardar_edicion, G_CALLBACK( empresa_inicio_imagen_escribir ), nullptr );
    conectar_senal( senal_imagen_cancelar_edicion, G_CALLBACK( imagen_cancelar ), nullptr );

    // 
    gtk_button_set_label( GTK_BUTTON( buscar_objeto( "DomicilioCancelar" ) ), "Omitir" );
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
    cout << "empresa_inicio_imagen" << endl;
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

    // Cierra el mensaje
    gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

    // Redirige a la siguiente vista
    irA( "AgregarDomicilio", false );
}

void empresa_inicio_imagen_escribir( GtkWidget *widget, gpointer info ){
    cout << "empresa_inicio_imagen_escribir" << endl;

    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio;

    // Crea el nombre del archivo
    stringstream s;
    s << empresa_razon_social << intAleatorio( motor );
    string empresa_imagen = crearHash( s.str() ) + ".png";

    // Guarda la selección temporal
    imagen_temporal_guardar( empresa_imagen );

    // Carga la nueva imagen
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenEmpresaInicio" ) ), imagen_temporal );
}

void empresa_senales_desconectar(){
    /*g_signal_handler_disconnect( buscar_objeto( "InicioAgregarFotoEmpresa" ), senal_empresa_imagen_seleccionar );
    g_signal_handler_disconnect( buscar_objeto( "BotonGuardarEdicionImagen" ), senal_emimagen_guardar );
    g_signal_handler_disconnect( buscar_objeto( "BotonCancelarEdicionImagen" ), senal_imagen_cancelar );
    g_signal_handler_disconnect( buscar_objeto( "BotonSi" ), senal_imagen_descartar );
    g_signal_handler_disconnect( buscar_objeto( "InicioContinuarConfiguracion" ), senal_imagen_continuar );
    g_signal_handler_disconnect( buscar_objeto( "InicioOmitirFotoEmpresa" ), senal_imagen_omitir );
    g_signal_handler_disconnect( buscar_objeto( "DomicilioAgregar" ), senal_domicilio_aceptar );
    g_signal_handler_disconnect( buscar_objeto( "DomicilioCancelar" ), senal_domicilio_cancelar );
    senal_imagen_seleccionar = 0;
    senal_imagen_guardar = 0;
    senal_movimiento = 0;*/
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
