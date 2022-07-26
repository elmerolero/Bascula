#include "Usuario.h"
#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include "Imagen.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include <cmath>
#include <regex>
using namespace std;

// Usuario
Usuario usuario;

Signal enlaceCuenta{ "EnlaceCuenta", "activate-link", 0 };
Signal senal_usuario_foto_seleccionar{ "UsuarioCambiarFoto", "clicked", 0 };
Signal senal_usuario_nombre_editar{ "EnlaceCambiarNombre", "activate-link", 0 };
Signal senal_usuario_nombre_cancelar_edicion { "BotonUsuarioNombreCancelar", "clicked", 0 };
Signal senal_usuario_nombre_guardar_edicion { "BotonUsuarioNombreGuardar", "clicked", 0 };

Signal senal_usuario_apellidos_editar { "EnlaceUsuarioCambiarApellidos", "activate-link", 0 };
Signal senal_usuario_apellidos_cancelar_edicion{ "BotonUsuarioApellidosCancelar", "clicked", 0 };
Signal senal_usuario_apellidos_guardar_edicion{ "BotonUsuarioApellidosGuardar", "clicked", 0 };

Signal senal_usuario_pseudonimo_editar { "EnlaceUsuarioCambiarPseudonimo", "activate-link", 0 };
Signal senal_usuario_pseudonimo_cancelar_edicion{ "BotonUsuarioPseudonimoCancelar", "clicked", 0 };
Signal senal_usuario_pseudonimo_guardar_edicion{ "BotonUsuarioPseudonimoGuardar", "clicked", 0 };

// Constructor
Usuario::Usuario():
    administrador( false )
{
    // Nada que hacer
}

Usuario::Usuario( unordered_map< string, string > *renglon ):
    administrador( false )
{
    // Se asegura que no se esté intentando establecer un renglón nulo
    if( renglon == nullptr ){
	    throw invalid_argument( "El renglón que se desea asignar está vacío." );
    }

    // Establece los datos del registro
    establecerClave( stoi( (* renglon)[ "id_usuario" ] ) );
    establecerNombreUsuario( (* renglon)[ "nombre_usuario" ] );
    establecerHash( (* renglon)[ "contrasena" ] );
    establecerNombre( (* renglon)[ "nombre" ] );
    establecerApellidos( (* renglon)[ "apellidos" ] );
    establecerAdministrador( stoi( (* renglon)[ "administrador" ] ) );
}

// Clave
void Usuario::establecerClave( unsigned int clave ){
	this -> clave = clave;
}

unsigned int Usuario::obtenerClave() const{
	return clave;
}

// Establece el nombre del usuario
void Usuario::establecerNombreUsuario( string nombreUsuario )
{
    regex formato( "[a-zA-Z0-9_-]*" );
    
    // Se asegura que no se introduzca un nombre de usuario vacío
    if( nombreUsuario.empty() ){
        throw invalid_argument( "Debe introducirse un nombre de usuario." );
    }
    
    // Se asegura de que el tamaño del nombre de usuario sea de almenos 6 caracteres y menor a 20
    if( nombreUsuario.size() < 4 || nombreUsuario.size() > 20 ){
        throw invalid_argument( "El numero de caracteres del usuario introducido se encuentra fuera del rango permitido." );
    }
    
    // Se asegura que no se introduzcan caracteres inválidos en el nombre de usuario
    if( !regex_match( nombreUsuario, formato ) ){
        throw invalid_argument( "El nombre introducido no es válido." );
    }
    
    this -> nombreUsuario = nombreUsuario;
}

// Obtiene el nombre del usuario
string Usuario::obtenerNombreUsuario() const
{
    return nombreUsuario;
}

// Valida el nombre de usuario
string  Usuario::validarNombreUsuario( string nombreUsuario )
{
    std::regex formato( "[a-zA-Z0-9_-]*" );
    
    // Se asegura que no se introduzca un nombre de usuario vacío
    if( nombreUsuario.empty() ){
        throw std::invalid_argument( "Debe introducirse un nombre de usuario." );
    }
    
    // Se asegura de que el tamaño del nombre de usuario sea de almenos 6 caracteres y menor a 20
    if( nombreUsuario.size() < 4 || nombreUsuario.size() > 20 ){
        throw std::invalid_argument( "El numero de caracteres del usuario introducido se encuentra fuera del rango permitido." );
    }

    // Se asegura que no se introduzcan caracteres inválidos en el nombre de usuario
    if( !std::regex_match( nombreUsuario, formato ) ){
        throw std::invalid_argument( "El nombre introducido no es válido." );
    }
    
    return nombreUsuario;
}

// Establece el nombre real del usuario
void Usuario::establecerNombre( string nombre )
{
    // Formato de validación del nombre introducido
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
    // Se asegura que no se introduzca un nombre vacío
    if( nombre.empty() ){
        throw invalid_argument( "Es nesesario introducir un nombre." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( nombre.size() > 35 || !regex_match( nombre, formato ) ){
        throw invalid_argument( "El nombre introducido no es válido." );
    } 
    
    this -> nombre = nombre;
}

// Obtiene el nombre real del usuario
string Usuario::obtenerNombre() const
{
    return nombre;
}

// Valida el nombre de la persona
string Usuario::validarNombre( std::string nombre ){
    // Formato de validación del nombre introducido
    std::regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
    // Se asegura que no se introduzca un nombre vacío
    if( nombre.empty() ){
        throw std::invalid_argument( "Es nesesario introducir un nombre." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( nombre.size() > 35 || !std::regex_match( nombre, formato ) ){
        throw std::invalid_argument( "El nombre introducido no es válido." );
    } 
    
    return nombre;
}

// Establece los apellidos del usuario
void Usuario::establecerApellidos( string apellidos )
{
    // Formato de validación del nombre introducido
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
    // Se asegura que no se introduzca un nombre vacío
    if( apellidos.empty() ){
        throw invalid_argument( "Es nesesario introducir un apellido." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( apellidos.size() > 35 || !regex_match( apellidos, formato ) ){
        throw invalid_argument( "El apellido introducido no es válido." );
    } 

    this -> apellidos = apellidos;
}

// Obtiene los apellidos del usuario
string Usuario::obtenerApellidos() const
{
    return apellidos;
}

// Valida los apellidos del usuario
string Usuario::validarApellidos( string apellidos )
{
    // Formato de validación del nombre introducido
    std::regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );

    // Se asegura que no se introduzca un nombre vacío
    if( apellidos.empty() ){
        throw std::invalid_argument( "Es nesesario introducir un apellido." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( apellidos.size() > 35 || !std::regex_match( apellidos, formato ) ){
        throw std::invalid_argument( "El apellido introducido no es válido." );
    } 
    
    return apellidos;
}

// Establece el hash del usuario a registrar
void Usuario::establecerHash( string hash )
{
    this -> hash = hash;
}

// Obtiene el hash del usuario a registrar
string Usuario::obtenerHash() const
{
    return hash;
}

// Compara las contraseñas
void Usuario::compararContrasenas( string contrasena, string confirmacion )
{
    // Realiza la comparativa de las contraseñas y lanza una excepción si no son iguales
	if( contrasena.compare( confirmacion ) != 0 ){
		throw invalid_argument( "Las contraseñas introducidas no coinciden." );
	}
}

// Establece los permisos de administrador
void Usuario::establecerAdministrador( bool permiso )
{
    this -> administrador = permiso;
}

// Revisa si el usuario tiene permisos de administrador
bool Usuario::esAdministrador() const
{
    return administrador;
}

// Valida la constraseña del usuario
string Usuario::validarContrasena( std::string contrasena ){
    // Formato de validación del nombre introducido
    std::regex formato( "[a-zA-Z0-9_\\-#@?*%$&]*" );
    if( contrasena.empty() ){
        throw std::invalid_argument( "Es necesario introducir una contraseña." );
    }
    
    if( contrasena.size() < 6 || !std::regex_match( contrasena, formato ) ){
        throw std::invalid_argument( "La contraseña introducida no es válida. Asegúrate que tenga al menos 6 caracteres." );
    }
    
    return contrasena;
}


void usuario_cuenta_leer( GtkWidget *widget, gpointer ptr ){
    cout << "usuario_cuenta_leer" << endl;
	// Establece las opciones para cuenta
    stringstream consulta;
    consulta << "select * from Usuario where id_usuario = " << usuario.obtenerClave();

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    if( results.size() > 0 ){
        cout << "llegue aqui" << endl;
        unordered_map< string, string > *cuenta = results.at( 0 );

        // Establece los datos base
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "UsuarioClave" ) ), (* cuenta)[ "id_usuario" ].c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaNombre" ) ), (* cuenta)[ "nombre" ].c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaApellidos" ) ), (* cuenta)[ "apellidos" ].c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaPseudonimo" ) ), (* cuenta)[ "pseudonimo" ].c_str() );

        // Establece los datos adicionales
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaFechaNacimiento" ) ), "No establecido." );
        if( (* cuenta)[ "fecha_nacimiento" ].compare( "null" ) != 0 ){
            gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaFechaNacimiento" ) ), (* cuenta)[ "fecha_nacimiento" ].c_str() );
        }

        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaRFC" ) ), "No establecido" );
        if( (* cuenta)[ "RFC" ].compare( "null" ) != 0 ){
            gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaRFC" ) ), (* cuenta)[ "RFC" ].c_str() );
        }

        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaCorreoElectronico" ) ), "No establecido" );
        if( (* cuenta)[ "correo" ].compare( "null" ) != 0 ){
            gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaCorreoElectronico" ) ), (* cuenta)[ "correo" ].c_str() );
        }

        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaFechaRegistro" ) ), ( "Registrado el " + (* cuenta)[ "fecha_registro" ] ).c_str() );

        gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenUsuarioPerfil" ) ), "../recursos/imagenes/iconos/Usuario.png" );
        if( (* cuenta)[ "imagen" ].compare( "null" ) != 0 ){
            gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenUsuarioPerfil" ) ), ("../recursos/imagenes/usuarios/" + (* cuenta)[ "imagen" ] ).c_str() );
        }
    }

    // Conecta las señales
    conectar_senal( senal_usuario_foto_seleccionar, G_CALLBACK( seleccionar_archivo ), nullptr );
    conectar_senal( senal_usuario_nombre_editar, G_CALLBACK( irHacia ), (void *)"UsuarioEscribirNombre" );
    conectar_senal( senal_usuario_nombre_guardar_edicion, G_CALLBACK( usuario_nombre_actualizar ), nullptr );
    conectar_senal( senal_usuario_nombre_cancelar_edicion, G_CALLBACK( regresarVista ), nullptr );
    conectar_senal( senal_imagen_guardar_edicion, G_CALLBACK( usuario_escribir_imagen ), nullptr );

    conectar_senal( senal_usuario_apellidos_editar, G_CALLBACK( irHacia ), (void *)"UsuarioEscribirApellidos" );
    conectar_senal( senal_usuario_apellidos_guardar_edicion, G_CALLBACK( usuario_apellidos_actualizar ), nullptr );
    conectar_senal( senal_usuario_apellidos_cancelar_edicion, G_CALLBACK( regresarVista ), nullptr );

    conectar_senal( senal_usuario_pseudonimo_editar, G_CALLBACK( irHacia ), (void *)"UsuarioEscribirPseudonimo" );
    conectar_senal( senal_usuario_pseudonimo_guardar_edicion, G_CALLBACK( usuario_pseudonimo_actualizar ), nullptr );
    conectar_senal( senal_usuario_pseudonimo_cancelar_edicion, G_CALLBACK( regresarVista ), nullptr );

	// Se dirige a cuenta
	irA( "Cuenta", false );
}

void usuario_escribir_imagen( GtkWidget *widget, gpointer info ){
    cout << "usuario_escribir_imagen" << endl;

    // Obtiene la clave de usuario
    string clave = gtk_label_get_text( GTK_LABEL( buscar_objeto( "UsuarioClave" ) ) );

    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio;

    // Crea el nombre del archivo
    stringstream s;
    s << gtk_label_get_text( GTK_LABEL( buscar_objeto( "UsuarioClave" ) ) ) 
      << gtk_label_get_text( GTK_LABEL( buscar_objeto( "CuentaPseudonimo" ) ) ) << intAleatorio( motor );
    string usuario_imagen = crearHash( s.str() ) + ".png";

    cairo_format_t formato = ( gdk_pixbuf_get_has_alpha( imagen.superficie ) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24 );
    imagen_temporal = cairo_image_surface_create( formato, 128, 128 );
    if( imagen_temporal == nullptr ){
        throw runtime_error( "Error de conversión de imagen." );
    }

    // Crea un contexto a partir de la superficie creada
    cairo_t *cr = cairo_create( imagen_temporal );
    if( cr == nullptr ){
        throw runtime_error( "Error de conversión de imagen." );
    }

    GdkPixbuf *corte = gdk_pixbuf_new_subpixbuf( imagen_escalada.superficie, abs( imagen_posicion_x ), abs( imagen_posicion_y ), TAMANIO_FOTO, TAMANIO_FOTO );
    GdkPixbuf *escala = gdk_pixbuf_scale_simple( corte, 128, 128, GDK_INTERP_BILINEAR );

    cairo_set_source_rgba(cr, 1, 1, 1, 1.f );
    cairo_rectangle(cr, 0, 0, 128, 128 );
    cairo_fill(cr);  

    gdk_cairo_set_source_pixbuf(cr, escala, 0, 0 );
    cairo_arc(cr, 64, 64, 64, 0, 2*M_PI);
    cairo_clip(cr);
    cairo_paint( cr );

    // Destruye el contexto
    cairo_destroy( cr );
    cr = nullptr;

    // Carga la nueva imagen
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenUsuarioPerfil" ) ), imagen_temporal );

    // Guarda el archivo del nuevo producto
    if( imagen_temporal != nullptr && !usuario_imagen.empty() ){
        // Variables de entrada
        string ruta_nuevo = "../recursos/imagenes/usuarios/" + usuario_imagen;
        string ruta_anterior;

        // Obtiene el nombre anterior de la imagen
        database.open( databaseFile );
        database.query( "select imagen from Usuario where id_usuario = " + clave );
        database.close();

        if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
            // Construye la ruta en la que se guardará el archivo nuevo y en la que se guardaba el archivo anterior
            ruta_anterior = "../recursos/imagenes/usuarios/" + (* results.at( 0 ))[ "imagen" ];

            // Si la ruta anterior es distinta de la ruta nueva se elimina el archivo anterior
            if( ruta_nuevo.compare( ruta_anterior ) != 0 ){
                cout << "Eliminando " << ruta_anterior << endl;
                remove( ruta_anterior.c_str() );
            }
        }

        // Guarda la imagen editada
        imagen_guardar( ruta_nuevo );

        // Registra el nuevo nombre de la imagen
        database.open( databaseFile );
        database.query( "update Usuario set imagen = '" + usuario_imagen + "' where id_usuario = " + clave );
        database.close();

        // Cancela la imagen
        imagen_cancelar();

        gtk_image_set_from_pixbuf( GTK_IMAGE( buscar_objeto( "ImagenUsuario" ) ), imagen_cargar_escalar( ruta_nuevo, 60, 60 ) );
        app_mostrar_exito( "Imagen cambiada exitosamente." );
    }
}

void usuario_nombre_actualizar( GtkWidget *widget, gpointer info ){
    cout << "usuario_nombre_actualizar" << endl;

    // Valida el nombre introducido
    string nombre = Usuario::validarNombre( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaUsuarioNombre" ) ) ) );

    // Registra al usuario en la base de datos
	database.open( databaseFile );
	database.query( "update Usuario set nombre = '" + nombre + "' where id_usuario = " + to_string( usuario.obtenerClave() ) );
	database.close();

    // Actualiza la información del usuario
    usuario.establecerNombre( nombre );
    
    // Actualiza la información de la interfaz
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaNombre" ) ), nombre.c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreUsuario" ) ), ( nombre + "\n" + usuario.obtenerApellidos() ).c_str() );

    //
    regresarVista();

    // Muestra un mensaje exitoso
    app_mostrar_exito( "Nombre actualizado correctamente." );
}

void usuario_apellidos_actualizar( GtkWidget *widget, gpointer info ){
    cout << "usuario_apellidos_actualizar" << endl;

    // Valida el nombre introducido
    string apellidos = Usuario::validarApellidos( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaUsuarioApellidos" ) ) ) );

    // Registra al usuario en la base de datos
	database.open( databaseFile );
	database.query( "update Usuario set apellidos = '" + apellidos + "' where id_usuario = " + to_string( usuario.obtenerClave() ) );
	database.close();

    // Actualiza la información del usuario
    usuario.establecerApellidos( apellidos );
    
    // Actualiza la información de la interfaz
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaApellidos" ) ), apellidos.c_str() );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreUsuario" ) ), ( usuario.obtenerNombre() + "\n" + apellidos ).c_str() );

    //
    regresarVista();

    // Muestra un mensaje exitoso
    app_mostrar_exito( "Apellidos actualizados correctamente." );
}

void usuario_PseudonimoOcupado( string pseudonimo ){
    // Abre la base de datos
	database.open( databaseFile );
	
	string consulta = "select * from Usuario where pseudonimo = '" + pseudonimo + "'";
	database.query( consulta );
	if( results.size() > 0 ){
		throw invalid_argument( "El pseudónimo que deseas utilizar ya está en uso." );
	}	
	
	// Cierra la conexión
	database.close();
}

void usuario_pseudonimo_actualizar( GtkWidget *widget, gpointer info ){
    cout << "usuario_pseudonimo_actualizar" << endl;

    try{
        // Valida el nombre introducido
        string pseudonimo = Usuario::validarNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaUsuarioPseudonimo" ) ) ) );

        // Revisa que el nombre de usuario no esté siendo usado por alguien más
        usuario_PseudonimoOcupado( pseudonimo );

        // Registra al usuario en la base de datos
        database.open( databaseFile );
        database.query( "update Usuario set pseudonimo = '" + pseudonimo + "' where id_usuario = " + to_string( usuario.obtenerClave() ) );
        database.close();

        // Actualiza la información del usuario
        usuario.establecerNombreUsuario( pseudonimo );

        // Actualiza la información de la interfaz
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "CuentaPseudonimo" ) ), pseudonimo.c_str() );

        //
        regresarVista();

        // Muestra un mensaje exitoso
        app_mostrar_exito( "Pseudónimo actualizado correctamente." );
    }
    catch( invalid_argument ia ){
        app_mostrar_error( ia.what() );
    }
}
