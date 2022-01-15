#include "Domicilio.h"
using namespace std;

/***********************************************************************************************
 * Domicilio Actualizar Registros                                                              *
 * Obtiene todos los domicilios registradas y las coloca en el contenedor de registros         * 
 * indicado, formando una lista de elementos seleccionables que se pueden seleccionar          * 
 * editar y eliminar.                                                                          *
 * *********************************************************************************************/
void domicilio_empresa_registros_actualizar( string tabla, string contenedor, unsigned int id ){
    cout << "empresa_actualizar_registros" << endl;
    // Limpia el contenedor
    limpiar_contenedor( contenedor.c_str() );

    // Consulta a la base de datos
    stringstream consulta;
    consulta << "select * from " << tabla << " where id_empresa = " << id;
    
    // Itera a través de la lista de registros y los añade a la interfaz
    database.open( databaseFile );
    database.query( consulta );
    database.close();

    if( results.size() > 0 ){
        for( unordered_map< string, string > *domicilio : results ){
            GError *error = nullptr;
            GtkBuilder *builder_chido = gtk_builder_new();
            
            // Clave del registro
            try{
                if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoDomicilio.glade", &error ) != 0 ){
                    gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "ElementoDomicilio" ) ), (*domicilio)[ "id_domicilio" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoDomicilioIdentificador" ) ), ((*domicilio)[ "calle" ] + " #" + (*domicilio)[ "numero" ]).c_str() );
                    string domicilioStr = (*domicilio)[ "colonia" ] + " " + (*domicilio)[ "codigo_postal" ] + ", " + (*domicilio)[ "municipio" ] + ", " + (*domicilio)[ "estado" ] + ", " + (*domicilio)[ "pais" ] + ".";
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemDescripcionDomicilio" ) ), domicilioStr.c_str);
                    
                    gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( contenedor ) ), GTK_WIDGET( buscar_objeto( "ElementoDomicilio" ) ), stoi( (*empresa)[ "id_domicilio" ] ) );
                }
            }
            catch( runtime_error &re ){
                cerr << re.what() << endl;
            }
        }
    }
}