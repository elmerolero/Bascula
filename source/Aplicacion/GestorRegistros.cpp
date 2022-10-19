#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Imagen.h"
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

// usuarios registrados
list< Usuario * > registrosUsuarios;

// Nombre de la empresa
string empresa_razon_social;
string empresa_rfc;
string empresa_imagen;

bool esInicio;

// Completador para el nombre del conductor
GtkListStore *listaNombresConductor = nullptr;
GtkEntryCompletion *completadorNombresConductor = nullptr;

// Completador 
GtkListStore *listaNumerosPlaca = nullptr;
GtkEntryCompletion *completadorNumerosPlaca = nullptr;

// Obtiene los usuarios registrados
void obtenerUsuariosRegistrados(){
    // Conecta con la base de datos
    database.open( databaseFile );
    
    // Obtiene los tickets del día
    string consulta = "select * from Usuario";
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

    unsigned int opcion = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "UsuarioAdministrador" ) ) );

    string consulta = "update Usuario set administrador = " + to_string( opcion ) + " where nombre = '" + usuarioConsultado -> obtenerNombreUsuario() + "';";
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

            gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeAlerta" ) ) );

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
            usuario_ConsultarRegistros( nullptr, nullptr );
            regresarVista();
        }
        catch( invalid_argument &ia ){
            app_mostrar_mensaje( "Ha ocurrido un error al intentar\neliminar el usuario." );
        }
    }
}

// Actualiza los registros publicos pendientes
void usuario_ConsultarRegistros( GtkWidget *widget, gpointer info ){
    // Limpia el contendor
    limpiar_contenedor( "ContenedorUsuarios" );

    // Busca los usuarios registrados
    database.open( databaseFile );
    database.query( "select * from Usuario" );
    database.close();
    if( results.size() > 0 ){
        for( auto usuario : results ){
            // Crea un elemento que será añadido a la interfaz
            GtkBuilder *builder = gtk_builder_new();
            GError *error = NULL;

            if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoUsuario.glade", &error ) ){
                gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaClaveUsuario" ) ), (* usuario)[ "id_usuario" ].c_str() );
                gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaNombre" ) ), ( (* usuario)[ "nombre" ] + " " + (* usuario)[ "apellidos" ] ).c_str() );
                if( (* usuario)[ "imagen" ].compare( "null" ) != 0 ){
                    GdkPixbuf *imagen = imagen_cargar_escalar( "../recursos/imagenes/usuarios/" + (*usuario)[ "imagen" ], 64, 64 );
                    if( imagen != nullptr ){
                        gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenUsuario" ) ), imagen );
                    }
                }

                gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorUsuarios" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "ItemUsuario" ) ), stoi( (*usuario)[ "id_usuario" ] ) );
            }
        }
    }

    irA( "ConsultarUsuarios", false );
}

void generarCodigoRecuperacion()
{
    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio( 65, 90 );

    // Nombre de usuario
    string usuario = Usuario::validarNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "NombreUsuarioConsultado" ) ) ) );

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
        string entradaUsuario = Usuario::validarNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRecuperacionUsuario" ) ) ) );
        string codigoSeguridad = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRecuperacionCodigoSeguridad" ) ) );

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