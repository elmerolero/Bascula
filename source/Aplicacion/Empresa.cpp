#include "Empresa.h"
#include "Aplicacion.h"
#include "Imagen.h"
#include "GestorRegistros.h"
#include "Funciones.h"
#include "Senales.h"
#include "Producto.h"
#include "Domicilio.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
using namespace std;

GtkListStore *listaEmpresas = nullptr;

Signal botonInicioRazonSocial{ "BotonRegistrarEmpresaPropia", "clicked", 0 };

Signal senal_empresa_propia_editar = { "OpcionEmpresa", "clicked", 0 };

// Señales para registrar la empresa propia
Signal senal_empresa_inicio_imagen_seleccionar = { "InicioSeleccionarFotoEmpresa", "clicked", 0 };
Signal senal_empresa_inicio_imagen_guardar = { "InicioAgregarFotoEmpresa", "clicked", 0 };
Signal senal_empresa_inicio_imagen_cancelar = { "InicioOmitirFotoEmpresa", "clicked", 0 };
Signal senal_empresa_domicilio_guardar = { "DomicilioAgregar", "clicked", 0 };
Signal senal_empresa_domicilio_cancelar = { "DomicilioCancelar", "clicked", 0 };

Signal senal_empresa_imagen_seleccionar = { "EnlaceEmpresaCambiarFoto", "clicked", 0 };
Signal senal_empresa_guardar_edicion = { "EmpresaEdicionGuardar", "clicked", 0 };
Signal senal_empresa_cancelar_edicion = { "EmpresaEdicionCancelar", "clicked", 0 };

void empresa_senales_conectar( void ){
    const char *mensaje = "ALERTA: Si el existen registros de pesaje que hagan\nreferencia a este empresa también serán eliminados.\n¿Estás seguro que deseas hacerlo?";

    conectar_senal( senal_producto_nuevo, G_CALLBACK( irHacia ), (void *)"NuevoRegistro" );    // Redirige a la vista nuevo registro
    conectar_senal( senal_producto_guardar_nuevo, G_CALLBACK( empresa_nuevo ), nullptr );      // Guarda la nueva empresa
    conectar_senal( senal_producto_cancelar_nuevo, G_CALLBACK( regresarVista ), nullptr );     // Cancela la nueva empresa
    conectar_senal( senal_producto_editar, G_CALLBACK( empresa_editar ), nullptr );            // Edita la empresa seleccionada
    conectar_senal( senal_producto_eliminar, G_CALLBACK( app_alerta ), (void *)mensaje );      // Lanza una alerta de eliminación del producto seleccionado
    conectar_senal( botonSi, G_CALLBACK( empresa_eliminar ), nullptr );                        // Confirma la eliminación de la empresa
    conectar_senal( senal_empresa_cancelar_edicion, G_CALLBACK( empresa_descartar_cambios ), nullptr );
    conectar_senal( senal_empresa_guardar_edicion, G_CALLBACK( empresa_guardar_edicion ), nullptr );
    conectar_senal( senal_empresa_imagen_seleccionar, G_CALLBACK( seleccionar_archivo ), nullptr );
    conectar_senal( senal_imagen_guardar_edicion, G_CALLBACK( empresa_imagen_escribir ), nullptr );
    conectar_senal( senal_imagen_cancelar_edicion, G_CALLBACK( imagen_cancelar ), nullptr );
    conectar_senal( senal_producto_seleccionar, G_CALLBACK( empresa_seleccionar ), nullptr );
}

void empresa_obtener_registros( void ){
    // Crea una nueva lista
    listaEmpresas = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todas las empresas
    database.open( databaseFile );
    database.query( "select razon_social from Empresas" );
    database.close();

    // ¿Hay resultados?
    if( results.size() > 0 ){
        cout << "results" << results.size() << endl;
        for( auto empresa : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaEmpresas, &iterador );
            gtk_list_store_set( listaEmpresas, &iterador, 0, ( (* empresa)[ "razon_social" ] ).c_str(), -1 );
        }
    }
}

void empresa_listar_registros( GtkWidget *widget, gpointer info ){
    cout << "empresa_listar_registros" << endl;
    // Actualiza la lista de registros
    empresa_actualizar_registros();

    // Conecta las señales
    conectar_senal( botonSi, G_CALLBACK( empresa_eliminar ), nullptr );
    
    // Conecta las señales
    empresa_senales_conectar();

    // Establece la vista para consultar registros y establece el nombre de los registros que se estan consultando
	irA( "ConsultarRegistros", false );
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "Titulo" ) ), "Empresas" );
}

void empresa_nuevo( GtkWidget *widget, gpointer info ){
    // Obtiene el nombre del registro
    string empresa_nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreNuevoRegistro" ) ) );

    try{
        // Valida el nombre del producto
        producto_validar_nombre( empresa_nombre );

        // Verifica que no exista un registro con ese nombre
		stringstream consulta;
		consulta << "select * from Empresas where razon_social like '%" << empresa_nombre << "'";
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();
		if( results.size() > 0 ){
			app_mostrar_error( "El registro que desea agregar ya existe." );
			return;
		}

        consulta.str( "" );
        consulta << "insert into Empresas values( null, '" << empresa_nombre << "', null, null )";

        // Efectua la consulta
        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Actualiza los registros
        empresa_actualizar_registros();

        // Redirige a la vista
        regresarVista();
    }
    catch( invalid_argument &ia ){
        app_mostrar_error( ia.what() );
    }
}

void empresa_editar( GtkWidget *widget, gpointer info ){
    cout << "empresa_editar" << endl;
	try{
        GdkPixbuf *imagen = nullptr;
        GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) );
        if( itemSeleccionado == NULL ){
            app_mostrar_error( "No se ha seleccionado ningún registro." );
            return;
        }

		stringstream consulta;
		consulta << "select * from Empresas where id_empresa = "
				 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );
		
		// Realiza la consultas
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );

			// Estabece los datos del formulario
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaClaveEditar" ) ), (* resultado )[ "id_empresa" ].c_str() );
			gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRazonSocialEditar" ) ), (* resultado )[ "razon_social" ].c_str() );
            gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ), "" );
			if( (* resultado)[ "RFC" ].compare( "null" ) != 0 ){
				gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ), (* resultado )[ "RFC" ].c_str() );
			}

            if( (*resultado)[ "imagen" ].compare( "null" ) != 0 ){
                imagen = imagen_cargar_escalar( "../recursos/imagenes/empresas/" + (*resultado)[ "imagen" ], 128, 128 );
                if( imagen != nullptr ){
                    gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenEmpresaEditar" ) ), imagen );
                }
            }
            else{
                gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEmpresaEditar" ) ), "../recursos/imagenes/iconos/Empresas.png" );
            }
		}

        // Conecta la señal de confirmación descartar cambios
        conectar_senal( enlaceRegresar, G_CALLBACK( empresa_descartar_cambios ), nullptr );
        conectar_senal( botonSi, G_CALLBACK( empresa_cancelar_edicion ), nullptr );

		irA( "EmpresaEditar", false );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void empresa_eliminar( GtkWidget *widget, gpointer info ){
    cout << "empresa_eliminar" << endl;
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

    // Elimina la imagen de la empresa
    consulta << "select * from Empresas where id_empresa = " << id;
    database.open( databaseFile );
    database.query( consulta.str() );
    if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
        remove( ("../recursos/imagenes/empresas/" + (* results.at( 0 ))[ "imagen" ]).c_str() );
    }

	// Elimina las dependencias de la empresa
	/*consulta << "delete from where id_producto = " 
			 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ) ) ) ) ) )
			 << */
    // Elimina la empresa
    consulta.str( "" );
	consulta << "delete from Empresas where id_empresa = " << id;
			
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	// Actualiza la vista de registros
	empresa_actualizar_registros();

    // Indica que se eliminó correctamente
    app_mostrar_exito( "Empresa eliminada correctamente." );
}

void empresa_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data ){
    cout << "empresa_seleccionar" << endl;
	// Obtiene el id del folio
	unsigned int folio = obtenerFolioSelector( row );
	
	// Obtiene el folio
	database.open( databaseFile );
	database.query( "select * from Empresas where id_empresa = " + to_string( folio ) );
	database.close();

	if( results.size() > 0 ){
		// Obtiene el renglon
		unordered_map< string, string > *resultados = results.at( 0 );

		// Establece la clave del registro
		stringstream clave;
		clave << setfill( '0' ) << setw( 7 ) << folio << setfill( ' ' );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaClave" ) ), clave.str().c_str() );
	
		// Establece el nombre del registro
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaRazonSocial" ) ), (* resultados)[ "razon_social" ].c_str() );

		// Establece la descripción
		if( (* resultados)[ "RFC" ].compare( "null" ) == 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaRFC" ) ), "Ninguno." );
		}
		else{
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaRFC" ) ), (* resultados)[ "RFC" ].c_str() );
		}

		// Establece imagen
		if( (* resultados)[ "imagen" ].compare( "null" ) == 0 ){
			gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEmpresa" ) ), "../recursos/imagenes/iconos/Empresas.png" );
		}
		else{
			string ruta = "../recursos/imagenes/empresas/";
			GdkPixbuf *imagen = imagen_cargar_escalar( ( ruta + (* resultados)[ "imagen" ] ).c_str(), 180, 180 );
			gtk_image_set_from_pixbuf( GTK_IMAGE(  buscar_objeto( "ImagenEmpresa" ) ), imagen );
		}

        // Obtiene los datos de la empresa
        domicilio_empresa_registros_actualizar( "DomiciliosEmpresas", "ContenedorEmpresaDomicilios", folio );

		// Establece la vista de registro
		irA( "Empresa", false );
	}
}

/************************************************************************************************
 * Empresa Descartar Cambios                                                                    *
 * Punto intermedio entre la cancelación de la edición y la lista de registros                  *
 * Permite revisar si se realizó algún cambio.                                                  *
 * **********************************************************************************************/
void empresa_descartar_cambios( GtkWidget *widget, gpointer info ){
    cout << "empresa_descartar_cambios" << endl;
    // ¿Se estableció o cambió una imagen o se realizó un cambio en la información del registro?
    if( !nombreTemporal.empty() || empresa_existen_cambios() ){
        // Lanza una alerta y regresa
        app_alerta( nullptr, (void *)"¿Desea descartar los cambios realizados?" );
        return;
    }

    // Cancela la edición
    empresa_cancelar_edicion();
}

/************************************************************************************************
 * Empresa Existen Cambios                                                                      *
 * Compara la información original del registro con la que se encuentra en el formulario        *
 * de edicion. Si hay alguna diferencia significa que existe un cambio.                         *
 * **********************************************************************************************/
bool empresa_existen_cambios( void ){
    cout << "empresa_existen_cambios";

    // Obtiene la información original de la empresa
    stringstream consulta;
    consulta << "select * from Empresas where id_empresa = " << gtk_label_get_text( GTK_LABEL( buscar_objeto( "EmpresaClaveEditar" ) ) );

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    if( results.size() > 0 ){
        unordered_map< string, string > *resultados = results.at( 0 );
        if( (* resultados)[ "razon_social" ].compare( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRazonSocialEditar" ) ) ) ) != 0 ){
            return true;
        }

        if( (* resultados)[ "RFC" ].compare( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ) ) ) != 0 ){
            return true;
        }
    }

    return false;
}

/***********************************************************************************************
 * Empresa Cancelar Edición                                                                   *
 * Cancela la edición de un producto seleccionado.                                             *
 * *********************************************************************************************/
void empresa_cancelar_edicion(){
    cout << "empresa_cancelar_edicion" << endl;
    // Establece la señal para eliminar registros de la vista anterior
    conectar_senal( botonSi, G_CALLBACK( empresa_eliminar ), nullptr );
    conectar_senal( enlaceRegresar, G_CALLBACK( regresarVista ), nullptr );

    // Cancela la edición de imagen
    imagen_cancelar();

    // Oculta la alerta que lanzó
    gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

    // Regresa a la vista anterior
    regresarVista();
}

void empresa_guardar_edicion( GtkWidget *widget, gpointer info ){
    cout << "empresa_guardar_edicion" << endl;
    // Obtiene la información del formulario
    string clave = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EmpresaClaveEditar" ) ) );
    string nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRazonSocialEditar" ) ) );
    string descripcion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ) );

    // Realiza la insercion
    stringstream consulta;
    consulta << "update Empresas set razon_social = '" << nombre << "', "
             << "RFC = '" << descripcion << "' " 
             << "where id_empresa = " << clave;

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Guarda el archivo del nuevo producto
    if( imagen_temporal != nullptr && !nombreTemporal.empty() ){
        // Variables de entrada
        string ruta_nuevo = "../recursos/imagenes/empresas/" + nombreTemporal;
        string ruta_anterior;

        // Obtiene el nombre anterior de la imagen
        consulta.str( "" );
        consulta << "select imagen from producto where id_producto = " << clave;
        database.open( databaseFile );
        database.query( consulta.str() );

        if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
            cout << "llego aquí" << endl;
            // Construye la ruta en la que se guardará el archivo nuevo y en la que se guardaba el archivo anterior
            ruta_anterior = "../recursos/imagenes/empresas/" + (* results.at( 0 ))[ "imagen" ];

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
        consulta << "update Empresas set imagen = '" << nombreTemporal << "' where id_empresa = " << clave;
        cout << clave << endl;
        database.query( consulta.str() );
        database.close();

        // Cancela la imagen
        imagen_cancelar();
    }

    //
    empresa_actualizar_registros();

    // Muestra el mensaje de que se actualizó la información del registro
    app_mostrar_exito( "Registro editado correctamente." );

    // Cancela el resto
    empresa_cancelar_edicion();
}

/***********************************************************************************************
 * Empresa Actualizar Registros                                                                *
 * Obtiene todas las empresas registradas y las coloca en el contenedor de registros,          *
 * formando una lista de elementos seleccionables que se pueden seleccionar editar y eliminar  *
 * El contenedor de registros tiene el nombre "ContenedorRegistros" en el archivo de interfaz. *
 * *********************************************************************************************/
void empresa_actualizar_registros( void ){
    cout << "empresa_actualizar_registros" << endl;
    // Limpia el contenedor
    limpiar_contenedor( "ContenedorRegistros" );
    
    // Itera a través de la lista de registros y los añade a la interfaz
    database.open( databaseFile );
    database.query( "select * from Empresas" );
    database.close();

    if( results.size() > 0 ){
        for( unordered_map< string, string > *empresa : results ){
            GError *error = nullptr;
            GtkBuilder *builder_chido = gtk_builder_new();
            GdkPixbuf *imagen = nullptr;
            
            // Clave del registro
            try{
                if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ItemRegistro.glade", &error ) != 0 ){
                    if( (*empresa)[ "imagen" ].compare( "null" ) != 0 ){
                        imagen = imagen_cargar_escalar( "../recursos/imagenes/empresas/" + (*empresa)[ "imagen" ], 64, 64 );
                        if( imagen != nullptr ){
                            gtk_image_set_from_pixbuf( GTK_IMAGE( gtk_builder_get_object( builder, "ImagenRegistro" ) ), imagen );
                        }
                    }
                    else{
                        gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenRegistro" ) ), "../recursos/imagenes/iconos/Empresa64.png" );
                    }
                    
                    gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "ItemRegistro" ) ), (*empresa)[ "id_empresa" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaClave" ) ), (*empresa)[ "id_empresa" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemEntradaNombre" ) ), (*empresa)[ "razon_social" ].c_str() );
                    
                    gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistros" ) ), GTK_WIDGET( buscar_objeto( "ItemRegistro" ) ), stoi( (*empresa)[ "id_empresa" ] ) );
                }
            }
            catch( runtime_error &re ){
                cerr << re.what() << endl;
            }
        }
    }
}

void empresa_inicio_senales_conectar(){
    // Razón social
    conectar_senal( botonInicioRazonSocial, G_CALLBACK( empresa_inicio_registrar ), nullptr );
    
    // 
    conectar_senal( senal_empresa_inicio_imagen_guardar, G_CALLBACK( empresa_inicio_imagen ), nullptr );
    conectar_senal( senal_empresa_inicio_imagen_cancelar, G_CALLBACK( empresa_inicio_imagen_omitir ), nullptr );
    conectar_senal( botonSi, G_CALLBACK( empresa_inicio_imagen_omitir_confirmacion ), nullptr );
    conectar_senal( senal_empresa_domicilio_guardar, G_CALLBACK( empresa_propia_domicilio_guardar_nuevo ), nullptr );
    conectar_senal( senal_empresa_domicilio_cancelar, G_CALLBACK( empresa_inicio_domicilio_cancelar ), nullptr );

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
        string ruta = "../recursos/imagenes/empresas/" + nombreTemporal;
        cairo_surface_write_to_png( imagen_temporal, ruta.c_str() );
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

void empresa_imagen_escribir( GtkWidget *widget, gpointer info ){
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
    gtk_image_set_from_surface( GTK_IMAGE( buscar_objeto( "ImagenEmpresaEditar" ) ), imagen_temporal );
}

void empresa_inicio_senales_desconectar(){
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

void empresa_propia_domicilio_guardar_nuevo( GtkWidget *widget, gpointer info ){
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

        // Actualiza la vista de domicilioss
        domicilio_empresa_registros_actualizar( "DomicilioEmpresaPropia", "ContenedorEmpresaPropiaDomicilios", 1 );

        // Regresa
        app_mostrar_exito( "Domicilio agregado correctamente." );
        regresarVista();
    }
    catch( invalid_argument &ia ){
        // Obtiene el mensaje de error
        string what = ia.what();

        // Verifica que sea error stoi
        app_mostrar_error( what );
        if( what.compare( "stoi" ) == 0 ){
            app_mostrar_error( "Código Postal no válido." );
        }
    }
}

void empresa_propia_editar( GtkWidget *widget, gpointer info ){
    // Obtiene la informacion de la empresa
    database.open( databaseFile );
    database.query( "select * from EmpresaPropia where id_empresa = 1" );
    database.close();

    if( results.size() > 0 ){
        // Lee el renglon resultante
        unordered_map< string, string > *empresa = results.at( 0 );

        // Completa los campos con la información dada
        gtk_label_set_text( GTK_LABEL( buscar_objeto( "EmpresaClaveEditar" ) ), (* empresa)[ "id_empresa" ].c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRazonSocialEditar" ) ), (* empresa)[ "razon_social" ].c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ), "" );
        if( (* empresa)[ "RFC" ].compare( "null" ) != 0 ){
            gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ), (* empresa)[ "RFC" ].c_str() );
        }

        gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEmpresaEditar" ) ), "../recursos/imagenes/iconos/Empresas.png" );
        if( (* empresa)[ "imagen" ].compare( "null" ) != 0 ){
            GdkPixbuf *imagen = imagen_cargar_escalar( "../recursos/imagenes/empresas/" + (*empresa)[ "imagen" ], 128, 128 );
            if( imagen != nullptr ){
                gtk_image_set_from_pixbuf( GTK_IMAGE( buscar_objeto( "ImagenEmpresaEditar" ) ), imagen );
            }
        }
    }

    // Conecta
    conectar_senal( senal_empresa_imagen_seleccionar, G_CALLBACK( seleccionar_archivo ), nullptr );
    conectar_senal( senal_imagen_guardar_edicion, G_CALLBACK( empresa_imagen_escribir ), nullptr );
    conectar_senal( senal_imagen_cancelar_edicion, G_CALLBACK( imagen_cancelar ), nullptr );
    conectar_senal( senal_empresa_guardar_edicion, G_CALLBACK( empresa_propia_guardar_edicion ), nullptr );
    conectar_senal( senal_empresa_cancelar_edicion, G_CALLBACK( regresarVista ), nullptr );

    irA( "EmpresaEditar", false );
}

void empresa_propia_guardar_edicion( GtkWidget *widget, gpointer info ){
    cout << "empresa_guardar_edicion" << endl;
    // Obtiene la información del formulario
    string clave = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EmpresaClaveEditar" ) ) );
    string nombre = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRazonSocialEditar" ) ) );
    string descripcion = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EmpresaRFCEditar" ) ) );

    // Realiza la insercion
    stringstream consulta;
    consulta << "update EmpresaPropia set razon_social = '" << nombre << "', "
             << "RFC = '" << descripcion << "' " 
             << "where id_empresa = " << clave;

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Guarda el archivo de la empresa
    if( imagen_temporal != nullptr && !nombreTemporal.empty() ){
        // Variables de entrada
        string ruta_nuevo = "../recursos/imagenes/empresas/" + nombreTemporal;
        string ruta_anterior;

        // Obtiene el nombre anterior de la imagen
        consulta.str( "" );
        consulta << "select imagen from EmpresaPropia where id_empresa = " << clave;
        database.open( databaseFile );
        database.query( consulta.str() );

        if( results.size() > 0 && (* results.at( 0 ))[ "imagen" ].compare( "null" ) != 0 ){
            cout << "llego aquí" << endl;
            // Construye la ruta en la que se guardará el archivo nuevo y en la que se guardaba el archivo anterior
            ruta_anterior = "../recursos/imagenes/empresas/" + (* results.at( 0 ))[ "imagen" ];

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
        consulta << "update EmpresaPropia set imagen = '" << nombreTemporal << "' where id_empresa = " << clave;
        cout << clave << endl;
        database.query( consulta.str() );
        database.close();

        //
        gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenEmpresaPropia" ) ), ( "../recursos/imagenes/empresas/" + nombreTemporal ).c_str() );

        // Cancela la imagen
        imagen_cancelar();
    }

    // Actualiza el nombre de la empresa
    gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreEmpresa" ) ), nombre.c_str() );

    // Muestra el mensaje de que se actualizó la información del registro
    app_mostrar_exito( "Nombre de la empresa editado correctamente." );

    // Cancela el resto
    empresa_cancelar_edicion();
}

void empresa_propia_domicilio_editar( GtkWidget *widget, gpointer info ){
    // Obtiene el item seleccionado
    GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorEmpresaPropiaDomicilios" ) ) );
    if( itemSeleccionado == NULL ){
        app_mostrar_error( "No ha seleccionado ningún registro." );
        return;
    }

    // Genera la consulta
    stringstream consulta;
    consulta << "select * from DomicilioEmpresaPropia where id_domicilio = " 
             << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );

    // Busca la información del domicilio seleccionado
    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();
    if( results.size() > 0 ){
        // Obtiene el mapa
        unordered_map< string, string > *domicilio = results.at( 0 );
    
        // Establece los campos correspondientes
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioCalle" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "calle" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioNumero" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "numero" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioNumeroInterior" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "numero_interior" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioColonia" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "colonia" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioCodigoPostal" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "codigo_postal" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioMunicipio" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "municipio" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioEstado" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "estado" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioPais" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "pais" ] : "" ).c_str() );
        gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioDescripcion" ) ), ( (* domicilio)[ "calle" ].compare( "null" ) != 0 ? (* domicilio)[ "descripcion" ] : "" ).c_str() );
    }

    // Establece la señal para guardar la edicion
    conectar_senal( enlaceRegresar, G_CALLBACK( empresa_propia_domicilio_cancelar_edicion ), nullptr );
    conectar_senal( senal_domicilio_cancelar_nuevo, G_CALLBACK( empresa_propia_domicilio_cancelar_edicion ), nullptr );
    conectar_senal( senal_domicilio_guardar_nuevo, G_CALLBACK( empresa_propia_domicilio_guardar_edicion ), nullptr );

    // Redirige hacia domicilio
    irA( "AgregarDomicilio", false );
}

void empresa_propia_domicilio_guardar_edicion( GtkWidget *widget, gpointer info ){
    try{
        GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorEmpresaPropiaDomicilios" ) ) );
        if( itemSeleccionado == NULL ){
            app_mostrar_error( "Ha ocurrido un error." );
            return;
        }

        // Consulta 
        stringstream consulta;

        // Establece la consulta a la base de datos
        consulta << "update DomicilioEmpresaPropia set "
                 << "calle = '" << domicilio_validar_lugar( "DomicilioCalle", true ) << "', "
                 << "numero = '" << domicilio_validar_numero( "DomicilioNumero", true ) << "', "
                 << "numero_interior = '" << domicilio_validar_numero( "DomicilioNumeroInterior", false ) << "', "
                 << "colonia = '" << domicilio_validar_lugar( "DomicilioColonia", true ) << "', "
                 << "codigo_postal = " << stoi( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "DomicilioCodigoPostal" ) ) ) ) << ", "
                 << "municipio = '" << domicilio_validar_lugar( "DomicilioMunicipio", true ) << "', "
                 << "estado = '" << domicilio_validar_lugar( "DomicilioEstado" , true ) << "', "
                 << "pais = '" << domicilio_validar_lugar( "DomicilioPais", true ) << "', "
                 << "descripcion = '" << domicilio_validar_descripcion( "DomicilioDescripcion" )
                 << "' where id_domicilio = " << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );
        
        cout << consulta.str() << endl;

        // Realiza la consulta
        database.open( databaseFile );
        database.query( consulta.str() );
        database.close();

        // Actualiza la vista de domicilioss
        domicilio_empresa_registros_actualizar( "DomicilioEmpresaPropia", "ContenedorEmpresaPropiaDomicilios", 1 );

        // Regresa
        empresa_propia_domicilio_cancelar_edicion( nullptr, nullptr );
        app_mostrar_exito( "Domicilio editado correctamente." );
    }
    catch( invalid_argument &ia ){
        // Obtiene el mensaje de error
        string what = ia.what();

        // Verifica que sea error stoi
        app_mostrar_error( what );
        if( what.compare( "stoi" ) == 0 ){
            app_mostrar_error( "Código Postal no válido." );
        }
    }
}

void empresa_propia_domicilio_cancelar_edicion( GtkWidget *widget, gpointer info ){
    // Reestablece la señal correspondiente
    conectar_senal( enlaceRegresar, G_CALLBACK( regresarVista ), nullptr );
    conectar_senal( senal_domicilio_guardar_nuevo, G_CALLBACK( empresa_propia_domicilio_guardar_nuevo ), nullptr );

    // Limpia el formulario
    domicilio_limpiar_formulario();

    // Regresa
    regresarVista();
}

void empresa_inicio_domicilio_agregar( GtkWidget *widget, gpointer info ){
    // Llama a agregar domicilio
    empresa_propia_domicilio_guardar_nuevo( nullptr, nullptr );

    // Desconecta las señales
    empresa_inicio_senales_desconectar();

    // Muestra el mensaje
    app_mostrar_mensaje( "Datos de la empresa registrados exitosamente." );
    irA( "Inicio", true ); 
}

void empresa_inicio_domicilio_cancelar( GtkWidget *widget, gpointer info ){
    // Desconecta las señales
    empresa_inicio_senales_desconectar();

    app_mostrar_mensaje( "Puede agregar o actualizar los datos de\nla empresa mas adelante en configuración." );
    irA( "Inicio", true );
}

string empresa_buscar_existente( string empresa_nombre ){
    cout << "producto_buscar_existente" << endl;
    try{
        // Valida el nombre del producto
        empresa_validar_razon_social( empresa_nombre );

        // Verifica que no exista un registro con ese nombre
		stringstream consulta;
		consulta << "select id_empresa from Empresas where razon_social like '%" << empresa_nombre << "'";
		database.open( databaseFile );
		database.query( consulta.str() );
		if( results.size() > 0 ){
			return (* results.at( 0 ))[ "id_empresa" ];
		}

        consulta.str( "" );
        consulta << "insert into Empresas values( null, '" << empresa_nombre << "', null, null )";

        // Efectua la consulta
        database.query( consulta.str() );

        // Vuelve a buscar el producto
        consulta.str( "" );
        string empresa_id;
        consulta << "select id_empresa from Empresas where razon_social like '%" << empresa_nombre << "'";
		database.query( consulta.str() );
        if( results.size() > 0 ){
            // Registra el nuevo producto
            empresa_id = (* results.at( 0 ))[ "id_empresa" ];
        }

        // Cierra la conexión con la base de datos
        database.close();

        return empresa_id;
    }
    catch( invalid_argument &ia ){
        throw ia;
    }
}
