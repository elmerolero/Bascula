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

Signal senal_producto_imagen_seleccionar = { "EnlaceProductoCambiarFoto", "activate-link", 0 };

// Lista de productos registrados
GtkListStore *listaProductos = nullptr;

Signal senal_producto_nuevo = { "BotonRegistroNuevo", "clicked", 0 };
Signal senal_producto_guardar_nuevo = { "BotonRegistroGuardarNuevo", "clicked", 0 };
Signal senal_producto_cancelar_nuevo = { "BotonRegistroCancelarNuevo", "clicked", 0 };

Signal senal_producto_editar = { "BotonRegistroEditar", "clicked", 0 };
Signal senal_producto_guardar_edicion = { "ProductoEdicionGuardar", "clicked", 0 };
Signal senal_producto_cancelar_edicion = { "ProductoEdicionCancelar", "clicked", 0 };
Signal senal_producto_eliminar = { "BotonRegistroEliminar", "clicked", 0 };
Signal senal_producto_seleccionar = { "ContenedorRegistros", "row-activated", 0 };

Signal senal_producto_buscar = { "BuscadorRegistros", "search-changed", 0 };

void producto_conectar_senales( void ){
    const char *mensaje = "ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este producto también serán eliminados.\n¿Estás seguro que deseas hacerlo?";

    // Vista de listado de productos
    conectar_senal( senal_producto_nuevo, G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );                 // Crea nuevo producto
    conectar_senal( senal_producto_guardar_nuevo, G_CALLBACK( producto_nuevo ), nullptr );                  // Guarda el nuevo producto
    conectar_senal( senal_producto_cancelar_nuevo, G_CALLBACK( regresarVista ), nullptr );                  // Cancela la creación del nuevo producot
    conectar_senal( senal_producto_editar, G_CALLBACK( producto_editar ), nullptr );                        // Edita el producto seleccionado
    conectar_senal( senal_producto_cancelar_edicion, G_CALLBACK( producto_descartar_cambios ), nullptr );   // Cancela la edicion del producto seleccionado
    conectar_senal( senal_producto_guardar_edicion, G_CALLBACK( producto_guardar_edicion ), nullptr );      // Guarda los cambios de la edición del producto seleccionado
    conectar_senal( senal_producto_eliminar, G_CALLBACK( app_alerta ), (void *)mensaje );                   // Lanza una alerta de eliminación del producto seleccionado
    conectar_senal( senal_producto_seleccionar, G_CALLBACK( producto_seleccionar ), nullptr );              // Selecciona un producto
    conectar_senal( botonSi, G_CALLBACK( producto_eliminar ), nullptr );
    conectar_senal( senal_producto_buscar, G_CALLBACK( producto_buscar ), nullptr );

    // Señales
    conectar_senal( senal_producto_imagen_seleccionar, G_CALLBACK( seleccionar_archivo ), nullptr );
    conectar_senal( senal_imagen_guardar_edicion, G_CALLBACK( producto_escribir_imagen ), nullptr );
    conectar_senal( senal_imagen_cancelar_edicion, G_CALLBACK( imagen_cancelar ), nullptr );
}

// Obtiene una lista con el nombre de los productos para un autocompletador
void producto_obtener_registros( void ){
    // Crea una nueva lista
    listaProductos = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todos los productos
    database.open( databaseFile );
    database.query( "select nombre from Producto" );
    database.close();

    // ¿Hay resultados?
    if( results.size() > 0 ){
        for( auto producto : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaProductos, &iterador );
            gtk_list_store_set( listaProductos, &iterador, 0, ( (* producto)[ "nombre" ] ).c_str(), -1 );
        }
    }
}
 
void producto_desconectar_senales( GtkWidget *widget, gpointer info ){
    // Cancela las señales
    desconectar_senal( senal_producto_nuevo );
    desconectar_senal( senal_producto_guardar_nuevo );
    desconectar_senal( senal_producto_cancelar_nuevo );
    desconectar_senal( senal_producto_editar );
    desconectar_senal( senal_producto_cancelar_edicion );
    desconectar_senal( senal_producto_guardar_edicion );
    desconectar_senal( senal_producto_eliminar );
    desconectar_senal( senal_producto_seleccionar );
}

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
    conectar_senal( botonSi, G_CALLBACK( producto_eliminar ), nullptr );
    
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

        // Vuelve a buscar el producto
        consulta.str( "" );
        string producto_id;
        consulta << "select id_producto from Producto where nombre like '%" << producto_nombre << "'";
        database.open( databaseFile );
		database.query( consulta.str() );
        if( results.size() > 0 ){
            // Registra el nuevo producto
            producto_id = (* results.at( 0 ))[ "id_producto" ];
        }
        
        // Agrega el nuevo producto
        if( !producto_id.empty() ){
            consulta.str( "" );
            consulta << "insert into Existencia values( 1, " << producto_id << ", 0, 0, 0 )";

            database.query( consulta.str() );
            database.close();
		}

        // Actualiza los registros
        producto_actualizar_registros();

        app_mostrar_exito( "Producto registrado correctamente." );

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
            gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ), "" );
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

        // Conecta la señal de confirmación descartar cambios
        conectar_senal( enlaceRegresar, G_CALLBACK( producto_descartar_cambios ), nullptr );
        conectar_senal( botonSi, G_CALLBACK( producto_cancelar_edicion ), nullptr );

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

    // Elimina la imagen del producto
    
    consulta << "select * from Producto where id_producto = " << id;
    database.open( databaseFile );
    database.query( consulta.str() );
    if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
        remove( ("../recursos/imagenes/productos/" + (* results.at( 0 ))[ "imagen" ]).c_str() );
    }

	// Elimina las dependencias del producto
    consulta.str( "" );
	consulta << "delete from PesajesPublicos where id_producto = " << id << "; ";
    database.query( consulta.str() );
    database.close();

    // Elimina el producto
    consulta.str( "" );
	consulta << "delete from Producto where id_producto = " << id;
			
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	// Actualiza la vista de registros
	producto_actualizar_registros();

    // Indica que se eliminó correctamente
    app_mostrar_exito( "Producto eliminado correctamente." );
}

void producto_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
    cout << "producto_seleccionar" << endl;
	// Obtiene el id del folio
	unsigned int folio = obtenerFolioSelector( row );
	
	// Obtiene el producto
	database.open( databaseFile );
	database.query( "select Producto.id_producto, nombre, descripcion, imagen, entrante, saliente, total from Producto join Existencia on Existencia.id_producto = Producto.id_producto where Producto.id_producto = " + to_string( folio ) );
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
			string ruta = "../recursos/imagenes/productos/";
			GdkPixbuf *imagen = imagen_cargar_escalar( ( ruta + (* resultados)[ "imagen" ] ).c_str(), 180, 180 );
			gtk_image_set_from_pixbuf( GTK_IMAGE(  buscar_objeto( "ImagenRegistroConsulta" ) ), imagen );
		}

        // Establece la existencia
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaProductoEntrante" ) ), (* resultados)[ "entrante" ].c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaProductoSaliente" ) ), (* resultados)[ "saliente" ].c_str() );
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EtiquetaProductoTotal" ) ), (* resultados)[ "total" ].c_str() );

		// Establece la vista de registro
		irA( "Producto", false );
	}
}

void producto_escribir_imagen( GtkWidget *widget, gpointer info ){
    cout << "producto_escribir_imagen" << endl;

    // Motor para generar numeros aleatorios
    default_random_engine motor( static_cast< unsigned int >( time( 0 ) ) );
    uniform_int_distribution< unsigned int > intAleatorio;

    // Crea el nombre del archivo
    stringstream s;
    s << gtk_label_get_text( GTK_LABEL( buscar_objeto( "RegistroClaveEditar" ) ) ) 
      << gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ) ) << intAleatorio( motor );
    string producto_imagen = crearHash( s.str() ) + ".png";

    // Guarda la selección temporal
    imagen_temporal_guardar( producto_imagen );

    // Carga la nueva imagen
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenProductoEditar" ) ), imagen_temporal );
}

void producto_guardar_edicion( GtkWidget *widget, gpointer info ){
    cout << "producto_guardar_edicion" << endl;

    // Obtiene la información del formulario
    string clave = gtk_label_get_text( GTK_LABEL( buscar_objeto( "RegistroClaveEditar" ) ) );
    string nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ) );
    string descripcion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ) );

    // Realiza la insercion
    stringstream consulta;
    consulta << "update Producto set nombre = '" << nombre << "', "
             << "descripcion = '" << descripcion << "' " 
             << "where id_producto = " << clave;
            
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Guarda el archivo del nuevo producto
    if( imagen_temporal != nullptr && !nombreTemporal.empty() ){
        // Variables de entrada
        string ruta_nuevo = "../recursos/imagenes/productos/" + nombreTemporal;
        string ruta_anterior;

        // Obtiene el nombre anterior de la imagen
        consulta.str( "" );
        consulta << "select imagen from producto where id_producto = " << clave;
        database.open( databaseFile );
        database.query( consulta.str() );

        if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
            cout << "llego aquí" << endl;
            // Construye la ruta en la que se guardará el archivo nuevo y en la que se guardaba el archivo anterior
            ruta_anterior = "../recursos/imagenes/productos/" + (* results.at( 0 ))[ "imagen" ];

            // Si la ruta anterior es distinta de la ruta nueva se elimina el archivo anterior
            if( ruta_nuevo.compare( ruta_anterior ) != 0 ){
                cout << "Eliminando " << ruta_anterior << endl;
                remove( ruta_anterior.c_str() );
            }
        }

        // Guarda la imagen editada
        imagen_guardar( ruta_nuevo );

        // Registra el nuevo nombre de la imagen
        consulta.str( "" );
        consulta << "update Producto set imagen = '" << nombreTemporal << "' where id_producto = " << clave;
        cout << clave << endl;
        database.query( consulta.str() );
        database.close();

        // Cancela la imagen
        imagen_cancelar();
    }

    //
    producto_actualizar_registros();

    // Muestra el mensaje de que se actualizó la información del registro
    app_mostrar_error( "Registro editado correctamente." );

    // Cancela el resto
    producto_cancelar_edicion();
}

/************************************************************************************************
 * Producto Descartar Cambios                                                                   *
 * Punto intermedio entre la cancelación de la edición y la lista de registros                  *
 * Permite revisar si se realizó algún cambio.                                                  *
 * **********************************************************************************************/
void producto_descartar_cambios( GtkWidget *widget, gpointer info ){
    cout << "producto_descartar_cambios" << endl;
    // ¿Se estableció o cambió una imagen o se realizó un cambio en la información del registro?
    if( !nombreTemporal.empty() || producto_existen_cambios() ){
        // Lanza una alerta y regresa
        app_alerta( nullptr, (void *)"¿Desea descartar los cambios realizados?" );
        return;
    }

    // Cancela la edición
    producto_cancelar_edicion();
}

/***********************************************************************************************
 * Producto Cancelar Edición                                                                   *
 * Cancela la edición de un producto seleccionado.                                             *
 * *********************************************************************************************/
void producto_cancelar_edicion(){
    cout << "producto_cancelar_edicion" << endl;
    // Establece la señal para eliminar registros de la vista anterior
    conectar_senal( botonSi, G_CALLBACK( producto_eliminar ), nullptr ); 

    // Cancela la edición de imagen
    imagen_cancelar();

    // Oculta la alerta que lanzó
    conectar_senal( enlaceRegresar, G_CALLBACK( regresarVista ), nullptr );
    gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

    // Regresa a la vista anterior
    regresarVista();
}

bool producto_existen_cambios( void ){
    stringstream consulta;
    consulta << "select * from Producto where id_producto = " << gtk_label_get_text( GTK_LABEL( buscar_objeto( "RegistroClaveEditar" ) ) );

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    if( results.size() > 0 ){
        unordered_map< string, string > *resultados = results.at( 0 );
        if( (* resultados)[ "nombre" ].compare( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoNombreEditar" ) ) ) ) != 0 ){
            return true;
        }

        if( (* resultados)[ "descripcion" ].compare( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "ProductoDescripcionEditar" ) ) ) ) != 0 ){
            return true;
        }
    }

    return false;
}

/***********************************************************************************************
 * Producto Actualizar Registros                                                               *
 * Obtiene todos los productos registrados y los coloca en el contenedor de registros,         *
 * formando una lista de elementos seleccionables que se pueden seleccionar editar y eliminar  *
 * El contenedor de registros tiene el nombre "ContenedorRegistros" en el archivo de interfaz. *
 * *********************************************************************************************/
void producto_actualizar_registros( void ){
    cout << "producto_actualizar_registros" << endl;
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

void producto_buscar( GtkSearchEntry* entry, GdkEvent* event ){
    cout << "producto_buscar" << endl;
    // Obtiene el contenido de la entrada
    string busqueda = gtk_entry_get_text( GTK_ENTRY( entry ) );

    cout << "Busqueda: " << busqueda << endl;

    // Limpia el contenedor
    limpiar_contenedor( "ContenedorRegistros" );

    // Realiza la busqueda
    database.open( databaseFile );
    database.query( "select * from Producto where nombre like '%" + busqueda + "%'" );
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

string producto_buscar_por_id( string id ){
    cout << "producto_buscar_por_id" << endl;
    // Obtiene la base de datos
    database.open( databaseFile );
    database.query( "select nombre from Producto where id_producto = "  + id );
    database.close();

    if( results.size() > 0 ){
        return (* results.at( 0 ))[ "nombre" ];
    }

    return "";
}

string producto_buscar_existente( string producto_nombre ){
    cout << "producto_buscar_existente" << endl;
    try{
        // Valida el nombre del producto
        producto_validar_nombre( producto_nombre );

        // Verifica que no exista un registro con ese nombre
		stringstream consulta;
		consulta << "select id_producto from Producto where nombre like '%" << producto_nombre << "'";
		database.open( databaseFile );
		database.query( consulta.str() );
		if( results.size() > 0 ){
			return (* results.at( 0 ))[ "id_producto" ];
		}

        consulta.str( "" );
        consulta << "insert into Producto values( null, '" << producto_nombre << "', null, null )";

        // Efectua la consulta
        database.query( consulta.str() );

        // Vuelve a buscar el producto
        consulta.str( "" );
        string producto_id;
        consulta << "select id_producto from Producto where nombre like '%" << producto_nombre << "'";
		database.query( consulta.str() );
        if( results.size() > 0 ){
            // Registra el nuevo producto
            producto_id = (* results.at( 0 ))[ "id_producto" ];
        }
        
        // Agrega el nuevo producto
        if( !producto_id.empty() ){
            consulta.str( "" );
            consulta << "insert into Existencia values( 1, " << producto_id << ", 0, 0, 0 )";

            database.query( consulta.str() );
		}

        // Cierra la conexión con la base de datos
        database.close();

        return producto_id;
    }
    catch( invalid_argument &ia ){
        throw ia;
    }
}
