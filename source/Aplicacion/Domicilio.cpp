#include "Domicilio.h"
#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Senales.h"
#include "Widget.h"
#include <unordered_map>
#include <iostream>
#include <sstream>
using namespace std;

Signal senal_domicilio_guardar_nuevo = { "DomicilioAgregar", "clicked", 0 };
Signal senal_domicilio_cancelar_nuevo = { "DomicilioCancelar", "clicked", 0 };

Signal senal_domicilio_empresa_propia_agregar = { "DomicilioEmpresaPropiaAgregar", "clicked", 0 };
Signal senal_domicilio_empresa_propia_editar = { "DomicilioEmpresaPropiaEditar", "clicked", 0 };
Signal senal_domicilio_empresa_propia_eliminar = { "DomicilioEmpresaPropiaEliminar", "clicked", 0 };

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
    
    // Itera a través de la lista de registros y los añade a la interfaz
    database.open( databaseFile );
    database.query( "select * from " + tabla + " where id_empresa = " + to_string( id ) );
    database.close();

    if( results.size() > 0 ){
        for( unordered_map< string, string > *domicilio : results ){
            GError *error = nullptr;
            GtkBuilder *builder_chido = gtk_builder_new();
            
            // Clave del registro
            try{
                if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoDomicilio.glade", &error ) != 0 ){
                    gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "ElementoDomicilio" ) ), (*domicilio)[ "id_domicilio" ].c_str() );
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoDomicilioIdentificador" ) ), ((*domicilio)[ "calle" ] + " " + (*domicilio)[ "numero" ]).c_str() );
                    string domicilioStr = (*domicilio)[ "colonia" ] + " " + (*domicilio)[ "codigo_postal" ] + ", " + (*domicilio)[ "municipio" ] + ", " + (*domicilio)[ "estado" ] + ", " + (*domicilio)[ "pais" ] + ".";
                    gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ItemDescripcionDomicilio" ) ), domicilioStr.c_str() );
                    gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( contenedor ) ), GTK_WIDGET( buscar_objeto( "ElementoDomicilio" ) ), stoi( (*domicilio)[ "id_domicilio" ] ) );
                }
            }
            catch( runtime_error &re ){
                cerr << re.what() << endl;
            }
        }
    }
}

/***********************************************************************************************
 * Domicilio Empresa Propia Eliminar                                                           *
 * Elimina el domicilio seleccionado del contendedor de domicilios registrados para la empresa. *
 * *********************************************************************************************/
void domicilio_empresa_propia_eliminar( GtkWidget *widget, gpointer info ){
    cout << "domicilio_empresa_propia_eliminar" << endl;
    // Obtiene el domicilio seleccionado
    GtkListBoxRow *itemSeleccionado = gtk_list_box_get_selected_row( GTK_LIST_BOX( buscar_objeto( "ContenedorEmpresaPropiaDomicilios" ) ) );
    if( itemSeleccionado == NULL ){
        app_mostrar_error( "No se ha seleccionado ningún registro." );
        return;
    }

    // Elimina el domicilio seleccionado
    stringstream consulta;
	consulta << "delete from DomicilioEmpresaPropia where id_domicilio = "
			 << gtk_widget_get_name( GTK_WIDGET ( gtk_bin_get_child( GTK_BIN( itemSeleccionado ) ) ) );

    database.open( databaseFile );
    database.query( consulta.str() );
    database.close();

    // Actualiza la vista de registros
    domicilio_empresa_registros_actualizar( "DomicilioEmpresaPropia", "ContenedorEmpresaPropiaDomicilios", 1 );
}

void domicilio_limpiar_formulario( void ){
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioCalle" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioNumero" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioNumeroInterior" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioColonia" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioCodigoPostal" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioMunicipio" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioEstado" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioPais" ) ), "" );
    gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "DomicilioDescripcion" ) ), "" );
}

string domicilio_validar_numero( std::string numero, bool obligatorio ){
    // Formato que permite el nombre de un lugar
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ,.\\s]*" );

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
        throw invalid_argument( "Se está registrando " + nombre_campo + " no inválido." );
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