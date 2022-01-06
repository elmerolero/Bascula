#include "Producto.h"
#include "Aplicacion.h"
#include "GestorRegistros.h"
#include "Senales.h"
#include "Funciones.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <string>
#include <random>
#include <regex>
using namespace std;

guint senal_producto_guardar_nuevo = 0;
guint senal_producto_nuevo = 0;
guint senal_producto_cancelar_nuevo = 0;
guint senal_producto_editar = 0;
guint senal_producto_eliminar = 0;
guint senal_producto_confirmar_eliminar = 0;
guint senal_producto_seleccionar = 0;



guint enlace_salir = 0;

void producto_validar_nombre( string nombre ){
    // Revisa el formato
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ.\\s]*" );
	if( nombre.empty() ){
		throw  invalid_argument( "Error, no puede establecerse un nombre vacío a un registro." );
	}
	
	if( nombre.size() > 50 || !regex_match( nombre, formato ) ){
		throw invalid_argument( "Error, no el nombre no puede contener más de cuarenta caracteres ni símbolos especiales." );
	}
}

void producto_listar_registros( GtkWidget *widget, gpointer info ){
    cout << "producto_listar_registros" << endl;
    // Actualiza la lista de registros
    producto_actualizar_registros();

    // Conecta las señales
    producto_conectar_senales();

    // Establece la vista para consultar registros y establece el nombre de los registros que se estan consultando
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "Titulo" ) ), "productos" );
	irA( "ConsultarRegistros", false );
}

void producto_nuevo( GtkWidget *widget, gpointer info ){
    // Obtiene el nombre del registro
    string producto_nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreNuevoRegistro" ) ) );

    try{
        // Valida el nombre del producto
        producto_validar_nombre( producto_nombre );

        // Verifica que no exista un registro con ese nombre
		stringstream consulta;
		consulta << "select * from Producto where nombre like '%" << producto_nombre << "'";
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( results.size() > 0 ){
			cout << "llegue a qui" << endl;
			app_mostrar_error( "El registro que desea agregar ya existe." );
			return;
		}

        consulta.str( "" );
        consulta << "insert into Producto values( null, '" << producto_nombre << "', null, null )";

        // Efectua la consulta
        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Actualiza los registros
        producto_actualizar_registros();

        // Redirige a la vista
        regresarVista();
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void producto_editar( GtkWidget *widget, gpointer info ){
	try{
        GdkPixbuf *imagen = nullptr;
        GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) );
        if( itemSeleccionado == NULL ){
            app_mostrar_error( "No se ha seleccionado ningún registro." );
            return;
        }

		stringstream consulta;
		consulta << "select * from Producto where id_producto = "
				 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );
		
		// Realiza la consultas
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );

			// Estabece los datos del formulario
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "RegistroClaveEditar" ) ), (* resultado )[ "id_producto" ].c_str() );
			gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ), (* resultado )[ "nombre" ].c_str() );
			if( (* resultado)[ "descripcion" ].compare( "null" ) != 0 ){
				gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ), (* resultado )[ "descripcion" ].c_str() );
			}

            if( (*resultado)[ "imagen" ].compare( "null" ) != 0 ){
                imagen = imagen_cargar_escalar( "../recursos/imagenes/productos/" + (*resultado)[ "imagen" ], 128, 128 );
                if( imagen != nullptr ){
                    gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenProductoEditar" ) ), imagen );
                }
            }
            else{
                gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenProductoEditar" ) ), "../recursos/imagenes/iconos/Producto64.png" );
            }
		}

		irA( "ProductoEditar", false );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void producto_eliminar( GtkWidget *widget, gpointer data ){
    cout << "producto_eliminar" << endl;

    // Cierra el mensaje de alerta
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

	// Obtiene el renglon seleccionado
    GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) );
    if( itemSeleccionado == NULL ){
        app_mostrar_error( "No se ha seleccionado ningún registro." );
        return;
    }

    // Obtiene el id del registro seleccionado
	unsigned int id = stoi( gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) ) );

	// Comando de consulta
	stringstream consulta;

	// Elimina las dependencias del producto
	/*consulta << "delete * from where id_producto = " 
			 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) ) ) ) ) )
			 << */
    // Elimina el producto
	consulta << "delete from Producto where id_producto = " << id;
			
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	// Actualiza la vista de registros
	producto_actualizar_registros();

    // Indica que se eliminó correctamente
    app_mostrar_error( "Producto eliminado correctamente." );
}

void producto_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
    cout << "producto_seleccionar" << endl;
	// Obtiene el id del folio
	unsigned int folio = obtenerFolioSelector( row );
	
	// Obtiene el folio
	string registro = (char *)data;
	database.open( databaseFile );
	database.query( "select * from " + registro + " where id_producto = " + to_string( folio ) );
	database.close();

	if( results.size() > 0 ){
		// Obtiene el renglon
		unordered_map< string, string > *resultados = results.at( 0 );

		// Establece la clave del registro
		stringstream clave;
		clave << setfill( '0' ) << setw( 7 ) << folio << setfill( ' ' );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaClaveRegistro" ) ), clave.str().c_str() );
	
		// Establece el nombre del registro
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaNombreRegistro" ) ), (* resultados)[ "nombre" ].c_str() );

		// Establece la descripción
		if( (* resultados)[ "descripcion" ].compare( "null" ) == 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaDescripcionRegistro" ) ), "Ninguno." );
		}
		else{
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaDescripcionRegistro" ) ), (* resultados)[ "descripcion" ].c_str() );
		}

		// Establece imagen
		if( (* resultados)[ "imagen" ].compare( "null" ) == 0 ){
			gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistroConsulta" ) ), "../recursos/imagenes/iconos/Producto.png" );
		}
		else{
			string ruta = "../recursos/imagenes/";
			ruta = ruta + (char *)data + "s/" + (* resultados)[ "imagen" ];
			GdkPixbuf *imagen = imagen_cargar_escalar( ruta.c_str(), 180, 180 );
			gtk_image_set_from_pixbuf( GTK_IMAGE(  buscar_objeto( "ImagenRegistroConsulta" ) ), imagen );
		}

		// Establece la vista de registro
		irA( "Registro", false );
	}
}

void producto_escribir_imagen( GtkWidget *widget, gpointer info ){
    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio;

    // Crea el nombre del archivo
    stringstream s;
    s << gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "RegistroClaveEditar" ) ) ) 
      << gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ) ) << intAleatorio( motor );
    string producto_imagen = "../recursos/imagenes/productos/" + crearHash( s.str() ) + ".png";

    // Guarda la selección temporal
    imagen_temporal_guardar( producto_imagen );

    // Carga la nueva imagen
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenProductoEditar" ) ), imagen_temporal );
}

void producto_guardar_cambios( GtkWidget *widget, gpointer info ){
    // Obtiene la información del formulario
    string clave = gtk_label_get_text( GTK_LABEL( buscar_objeto( "RegistroClaveEditar" ) ) );
    string nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ) );
    string descripcion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ) );

    // Realiza la insercion
    stringstream consulta;
    consulta << "update Producto set nombre = '" << nombre << "', "
             << "descripcion = '" << descripcion << "', "
             << "imagen = '" << ( imagen_temporal != nullptr && !nombreTemporal.empty() ? nombreTemporal : "null" ) << "' "
             << "where id_producto = " << clave;
            
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Cancela el resto
    producto_cancelar_edicion();
}

void producto_descartar_cambios( GtkWidget *widget, gpointer info ){
    // 
    if( !nombreTemporal.empty() /*|| producto_existen_cambios()*/ ){
        app_alerta( nullptr, (void *)"¿Desea descartar los cambios realizados?" );
        return;
    }

    producto_cancelar_edicion();
}

void producto_cancelar_nuevo( GtkWidget *widget, gpointer info ){
    regresarVista();
}

void producto_cancelar_edicion(){
    // Cancela las señales
    g_signal_handler_disconnect( buscar_objeto( "EnlaceProductoCambiarFoto" ), senal_imagen_seleccionar );
    g_signal_handler_disconnect( buscar_objeto( "BotonGuardarEdicionImagen" ), senal_imagen_guardar );
    g_signal_handler_disconnect( buscar_objeto( "BotonCancelarEdicionImagen" ), senal_imagen_cancelar );
    g_signal_handler_disconnect( buscar_objeto( "BotonSi" ), senal_imagen_descartar );
    g_signal_handler_disconnect( buscar_objeto( "ProductoEdicionGuardar" ), senal_imagen_continuar );
    g_signal_handler_disconnect( buscar_objeto( "ProductoEdicionCancelar" ), senal_imagen_omitir );

    senal_imagen_seleccionar = 0;
    senal_imagen_guardar = 0;
    senal_movimiento = 0;

    // 
    imagen_cancelar();
}

void producto_actualizar_registros( void ){
    // Limpia el contenedor
    limpiar_contenedor( "ContenedorRegistros" );
    
    // Itera a través de la lista de registros y los añade a la interfaz
    database.open( databaseFile );
    database.query( "select * from Producto" );
    database.close();

    if( results.size() > 0 ){
        for( unordered_map< string, string > *producto : results ){
            GError *error = nullptr;
            GtkBuilder *builder_chido = gtk_builder_new();
            GdkPixbuf *imagen = nullptr;
            
            // Clave del registro
            try{
                if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ItemRegistro.glade", &error ) != 0 ){
                    if( (*producto)[ "imagen" ].compare( "null" ) != 0 ){
                        imagen = imagen_cargar_escalar( "../recursos/imagenes/productos/" + (*producto)[ "imagen" ], 64, 64 );
                        if( imagen != nullptr ){
                            gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenRegistro" ) ), imagen );
                        }
                    }
                    else{
                        gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistro" ) ), "../recursos/imagenes/iconos/Producto64.png" );
                    }
                    
                    gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "ItemRegistro" ) ), (*producto)[ "id_producto" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaClave" ) ), (*producto)[ "id_producto" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaNombre" ) ), (*producto)[ "nombre" ].c_str() );
                    
                    gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ), GTK_WIDGET( buscar_objeto( "ItemRegistro" ) ), stoi( (*producto)[ "id_producto" ] ) );
                }
            }
            catch( runtime_error &re ){
                cerr << re.what() << endl;
            }
        }
    }
}

void producto_conectar_senales( void ){
    // Vista de listado de productos
    senal_producto_guardar_nuevo = g_signal_connect( buscar_objeto( "BotonRegistroGuardarNuevo" ), "clicked", G_CALLBACK( producto_nuevo ), nullptr );
    senal_producto_nuevo = g_signal_connect( buscar_objeto( "BotonRegistroNuevo" ), "clicked", G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );
    senal_producto_cancelar_nuevo = g_signal_connect( buscar_objeto( "BotonRegistroCancelarNuevo" ), "clicked", G_CALLBACK( producto_cancelar_nuevo ), nullptr );
    senal_producto_editar = g_signal_connect( buscar_objeto( "BotonRegistroEditar" ), "clicked", G_CALLBACK( producto_editar ), nullptr );
    senal_producto_eliminar = g_signal_connect( buscar_objeto( "BotonRegistroEliminar" ), "clicked", G_CALLBACK( app_alerta ), (void *)"ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este registro, también serán eliminados.\n¿Estás seguro que deseas hacerlo?" );
    senal_producto_seleccionar = g_signal_connect( buscar_objeto( "ContenedorRegistros" ), "row-activated", G_CALLBACK( producto_seleccionar ), nullptr );
    senal_producto_confirmar_eliminar = g_signal_connect( buscar_objeto( "BotonSi"), "clicked", G_CALLBACK( producto_eliminar ), nullptr );

    // Señales
    senal_imagen_seleccionar = g_signal_connect( buscar_objeto( "EnlaceCambiarFoto" ), "activate-link", G_CALLBACK( seleccionar_archivo ), nullptr );
    senal_imagen_guardar = g_signal_connect( buscar_objeto( "BotonGuardarEdicionImagen" ), "clicked", G_CALLBACK( producto_escribir_imagen ), nullptr );
    senal_imagen_cancelar = g_signal_connect( buscar_objeto( "BotonCancelarEdicionImagen" ), "clicked", G_CALLBACK( imagen_cancelar ), nullptr );
    senal_imagen_descartar = g_signal_connect( buscar_objeto( "BotonSi" ), "clicked", G_CALLBACK( producto_cancelar_edicion ), nullptr );
    senal_imagen_continuar = g_signal_connect( buscar_objeto( "ProductoEdicionGuardar" ), "clicked", G_CALLBACK( producto_guardar_cambios ), nullptr );
    senal_imagen_omitir = g_signal_connect( buscar_objeto( "ProductoEdicionCancelar" ), "clicked", G_CALLBACK( producto_descartar_cambios ), nullptr );
    enlace_salir = g_signal_connect( buscar_objeto( "EnlaceRegresar" ), "activate-link", G_CALLBACK( producto_desconectar_senales ), nullptr );
}
 
void producto_desconectar_senales( GtkWidget *widget, gpointer info ){
    // Cancela las señales
    g_signal_handler_disconnect( buscar_objeto( "EnlaceProductoCambiarFoto" ), senal_imagen_seleccionar );
    g_signal_handler_disconnect( buscar_objeto( "BotonGuardarEdicionImagen" ), senal_imagen_guardar );
    g_signal_handler_disconnect( buscar_objeto( "BotonCancelarEdicionImagen" ), senal_imagen_cancelar );
    g_signal_handler_disconnect( buscar_objeto( "BotonSi" ), senal_imagen_descartar );
    g_signal_handler_disconnect( buscar_objeto( "ProductoEdicionGuardar" ), senal_imagen_continuar );
    g_signal_handler_disconnect( buscar_objeto( "ProductoEdicionCancelar" ), senal_imagen_omitir );
    g_signal_handler_disconnect( buscar_objeto( "EnlaceRegresar" ), enlace_salir );
    senal_imagen_seleccionar = 0;
    senal_imagen_guardar = 0;
    senal_movimiento = 0;
}