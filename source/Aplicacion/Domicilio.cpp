

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