#include "PesajesInternos.h"
#include "PesajesPublicos.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include "Empresa.h"
#include "Producto.h"
#include "Basculas.h"
#include "Imagen.h"
#include "Vistas.h"
#include "Senales.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <regex>
#include <stdexcept>
#include <Windows.h>
using namespace std;

bool bruto_manual;
bool tara_manual;

// Completador
GtkEntryCompletion *completadorInterno = nullptr;
GtkEntryCompletion *completadorInternoProducto = nullptr;
GtkEntryCompletion *completadorInternoConductor = nullptr;
GtkEntryCompletion *completadorInternoPlacas = nullptr;

GtkListStore *listaInternoConductores = nullptr;
GtkListStore *listaInternoPlacas = nullptr;

Signal senal_interno_opcion = { "BotonRegistrosPesajesInternos", "clicked", 0 };
Signal senal_interno_numero_placas = { "EntradaNumeroPlacasInterno", "insert-text", 0 };

Signal senal_interno_pendientes_listar = { "BotonBasculaAdministrativo", "clicked", 0 };
Signal senal_interno_editar_pendiente = { "BotonRegistrarPendienteInterno", "clicked", 0 };
Signal senal_interno_pendiente_finalizar = { "BotonFinalizarPendienteInterno", "clicked", 0 };;

Signal senal_interno_leer_bruto = { "BotonLeerPesoBrutoInterno", "clicked", 0 };
Signal senal_interno_leer_tara = { "BotonLeerPesoTaraInterno", "clicked", 0 };
Signal senal_interno_calcular_descuento = { "BotonCalcularDescuento", "clicked", 0 };

Signal senal_interno_editar = { "ContenedorPesajesPendientes", "row-activated", 0 };

Signal senal_interno_eliminar = { "EliminarInterno", "clicked", 0 };
Signal senal_interno_imprimir = { "ImprimirInterno", "clicked", 0 };

void interno_iniciar( void ){
	// Inicializa el completador
	completadorInterno = gtk_entry_completion_new();

	// Se se creó una lista de empresas y un completador interno se asocian a la entrada de registros internos
	if( completadorInterno != nullptr && listaEmpresas != nullptr ){
		gtk_entry_completion_set_model( completadorInterno, GTK_TREE_MODEL( listaEmpresas ) );
		gtk_entry_completion_set_text_column( completadorInterno, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ), completadorInterno );
	}

	// Inicializa el completador de productos
	completadorInternoProducto = gtk_entry_completion_new();

	// Se se creó una lista de empresas y un completador interno se asocian a la entrada de registros internos
	if( completadorInternoProducto != nullptr && listaProductos != nullptr ){
		gtk_entry_completion_set_model( completadorInternoProducto, GTK_TREE_MODEL( listaProductos ) );
		gtk_entry_completion_set_text_column( completadorInternoProducto, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ), completadorInternoProducto );
	}

	// Crea una nueva lista para los conductores
    listaInternoConductores = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todos los productos
    database.open( databaseFile );
    database.query( "select DISTINCT nombre_conductor from PesajesInternos" );
    database.close();
    if( results.size() > 0 ){
        for( auto conductor : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaInternoConductores, &iterador );
            gtk_list_store_set( listaInternoConductores, &iterador, 0, ( (* conductor)[ "nombre_conductor" ] ).c_str(), -1 );
        }
    }

	// Inicializa el completador de conductores
	completadorInternoConductor = gtk_entry_completion_new();
	if( completadorInternoConductor != nullptr && listaInternoConductores != nullptr ){
		gtk_entry_completion_set_model( completadorInternoConductor, GTK_TREE_MODEL( listaInternoConductores ) );
		gtk_entry_completion_set_text_column( completadorInternoConductor, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), completadorInternoConductor );
	}

	// Crea una nueva lista para los números de placa
    listaInternoPlacas = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todos los número de placa
    database.open( databaseFile );
    database.query( "select DISTINCT numero_placas from PesajesInternos" );
    database.close();
    if( results.size() > 0 ){
        for( auto placa : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaInternoPlacas, &iterador );
            gtk_list_store_set( listaInternoPlacas, &iterador, 0, ( (* placa)[ "numero_placas" ] ).c_str(), -1 );
        }
    }

	// Inicializa el completador de las placas
	completadorInternoPlacas = gtk_entry_completion_new();
	if( completadorInternoPlacas != nullptr && listaInternoPlacas != nullptr ){
		gtk_entry_completion_set_model( completadorInternoPlacas, GTK_TREE_MODEL( listaInternoPlacas ) );
		gtk_entry_completion_set_text_column( completadorInternoPlacas, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), completadorInternoPlacas);
	}
}

void interno_registros_listar_pendientes( GtkWidget *widget, gpointer info ){
	cout << "internos_registros_listar_pendientes" << endl;
	
	// Actualiza la lista de registros pendientes
	interno_registros_actualizar_pendientes();

	// Conecta las señales
    conectar_senal( senal_interno_numero_placas, G_CALLBACK( convertirMayusculas ), nullptr );
    conectar_senal( senal_interno_leer_bruto, G_CALLBACK( interno_leer_bruto ), nullptr );
    conectar_senal( senal_interno_leer_tara, G_CALLBACK( interno_leer_tara ), nullptr );
	conectar_senal( senal_interno_calcular_descuento, G_CALLBACK( interno_calcular_neto ), nullptr );
	conectar_senal( senal_interno_pendiente_finalizar, G_CALLBACK( interno_pendiente_finalizar ), nullptr );
	conectar_senal( senal_pesajes_nuevo_pendiente, G_CALLBACK( interno_pendiente_nuevo ), nullptr );
	conectar_senal( senal_publico_editar, G_CALLBACK( interno_pendiente_seleccionar ), nullptr );

	// Redirige a la vista de pesajes pendientes
	irA( "PesajesPendientes", false );
}

void interno_pendiente_nuevo( GtkWidget *widget, gpointer info ){
	// Folio
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), "No asignado" );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ), tiempo_leer_fecha_corta().c_str() );
	
	// Empresa
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ), "" );

	// Producto
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ), "" );
	
	// Nombre del conductor
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), "" );
	
	// Numero de placas
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), "" );
	
	// Hora entrada y Peso Bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ), "No establecida" );
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoBrutoManualInterno" ) ) );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ), "No establecido" );
	
	// Hora salida y Peso Tara
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ), "No establecida" );
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoTaraManualInterno" ) ) );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ), "No establecido" );
	
	// Peso neto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );
	
	// Tipo de registros
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( buscar_objeto( "TipoRegistroEntrada" ) ), TRUE );

	// Motivos y observaciones
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ), "" );

	// Conecta la señal
	conectar_senal( senal_interno_editar_pendiente, G_CALLBACK( interno_pendiente_guardar_nuevo ), nullptr );
	
	// Establece la vista de nuevo ticket
	irA( "PesajeInternoEditable", false );
}

void interno_pendiente_guardar_nuevo( GtkWidget *widget, gpointer info ){
	try{
		// Obtiene el producto introducido
		string id_empresa = empresa_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ) ) );
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ) ) );
    	string fecha = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ) );
		string conductor = interno_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ) ) );
		string numero_placas = interno_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ) ) );
		string id_basculista = to_string( usuario.obtenerClave() );
		string observaciones = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ) );

		// Registra el peso bruto
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ) );
		double peso_tara = 0.f;
		string hora_salida;
		double descuento = 0.f;
		double peso_neto = 0.f;
		int tipo_registro = 0;
		
		try{
			// Registra el peso tara
			peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
			hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ) );
		}
		catch( invalid_argument &ia ){
			// registra el peso tara en cero
			peso_tara = 0.f;
			hora_salida = "";
		}
		
		try{
			descuento = stod( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
		}
		catch( invalid_argument &ia ){
			descuento = 0.f;
		}
		
		// Registra el peso neto
		peso_neto = abs( peso_bruto - peso_tara ) - descuento;

		// Establece si es de entrada o salida
		tipo_registro = TIPO_REGISTRO_SALIDA;
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "TipoRegistroEntrada" ) ) ) ){
			tipo_registro = TIPO_REGISTRO_ENTRADA;
		}

		stringstream consulta;
		consulta << "insert into PesajesInternos values( null, '" << fecha << "', " << tipo_registro << ", " 
				 << id_empresa << ", " << id_producto << ", '" << numero_placas << "', '" << conductor << "', '" 
				 << hora_entrada << "', " << ( hora_salida.empty() ? "null" : "'" + hora_salida + "'" ) << ", " << peso_bruto << ", " << bruto_manual << ", " 
				 << peso_tara << ", " << tara_manual << ", " << descuento << ", " << peso_neto << ", '" << observaciones << "', " 
				 << usuario.obtenerClave() << ", 1 )";
		
		cout << consulta.str() << endl;

		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		// Obtiene los tickets pendientes
		interno_registros_actualizar_pendientes();

		//
		regresarVista();
		
		// Crea o registra el registro según corresponda
		app_mostrar_exito( "Registro creado correctamente." );
	}
	catch( invalid_argument &ia ){
		string error = ia.what();
		app_mostrar_error( ia.what() );
		if( error.compare( "stod" ) == 0 ){
			app_mostrar_error( "Es necesario registrar el peso bruto." );
		}
	}
}

void interno_pendiente_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data ){
	cout << "interno_pendiente_seleccionar" << endl;

	// Obtiene el folio del registro de pesaje
	unsigned int folio = obtenerFolioSelector( row );

	stringstream consulta;
	consulta << "select id_pesaje, fecha, Empresas.razon_social as empresa, Producto.nombre as producto, numero_placas, "
			 << "nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_bruto_manual, peso_tara, peso_tara_manual, "
			 << "peso_neto, descuento, observaciones from PesajesInternos "
			 << "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa "
			 << "join Producto on PesajesInternos.id_producto = Producto.id_producto "
			 << "where id_pesaje = " << folio;

	// Obtiene el registro de pesaje
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();
	if( results.size() > 0 ){
		cout << "Llegue aqui" << endl;
		// Obtiene el pesaje
		unordered_map< string, string > *pesaje = results.at( 0 );

		// Folio
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), (* pesaje)[ "id_pesaje" ].c_str() );
		
		// Fecha
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ), (* pesaje)[ "fecha" ].c_str() );
		
		// Nombre empresa interno
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ), (* pesaje)[ "empresa" ].c_str() );

		// Producto
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ), (* pesaje)[ "producto" ].c_str() );
		
		// Nombre del conductor
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ), (* pesaje)[ "nombre_conductor" ].c_str() );
		
		// Numero de placas
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ), (* pesaje)[ "numero_placas" ].c_str() );
		
		// Hora entrada y Peso Bruto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ), (* pesaje)[ "hora_entrada" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ), ((* pesaje)[ "peso_bruto" ] + " Kg.").c_str() );
		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoBrutoManualInterno" ) ) );
		bruto_manual = stoi( (* pesaje)[ "peso_bruto_manual" ] );
		if( bruto_manual ){
			gtk_widget_show( GTK_WIDGET( buscar_objeto( "EntradaPesoBrutoManualInterno" ) ) );
		}
		
		// Hora salida y Peso Tara
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ), "No establecida" );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ), "No establecido" );
		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoTaraManualInterno" ) ) );
		if( (* pesaje)[ "peso_tara" ].compare( "null" ) != 0 && (* pesaje)[ "hora_salida" ].compare( "null" ) != 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ), (* pesaje)[ "hora_salida" ].c_str() );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ), ((* pesaje)[ "peso_tara" ] + " Kg." ).c_str() );
			tara_manual = stoi( (* pesaje)[ "peso_tara_manual" ] );
			if( tara_manual ){
				gtk_widget_show( GTK_WIDGET( buscar_objeto( "EntradaPesoTaraManualInterno" ) ) );
			}
		}
		
		// Peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );
		if( (* pesaje)[ "peso_neto" ].compare( "null" ) != 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), ((* pesaje)[ "peso_neto" ] + "Kg.").c_str()  );
		}

		// Motivos y observaciones
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ), "" );
		if( (* pesaje)[ "observaciones" ].compare( "null" ) != 0 ){
			gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ), ( (* pesaje)[ "observaciones" ] ).c_str() );
		}

		// Conecta las señales
		conectar_senal( senal_interno_editar_pendiente, G_CALLBACK( interno_pendiente_guardar_edicion ), nullptr );

		irA( "PesajeInternoEditable", false );
	}
}

void interno_pendiente_guardar_edicion( GtkWidget *widget, gpointer info ){
	cout << "interno_pendiente_guardar_edicion" << endl;
	try{
		// Obtiene el folio de pesaje
		unsigned long folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) );
		
		// Recolecta los demás datos
		string id_empresa = empresa_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ) ) );
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ) ) );
		string conductor = interno_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ) ) );
		string numero_placas = interno_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ) ) );
		string observaciones = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ) );

		// Registra el peso bruto
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ) );
		double peso_tara = 0.f;
		string hora_salida;
		double descuento = 0.f;
		double peso_neto = 0.f;
		int tipo_registro = 0;
		
		try{
			// Registra el peso tara
			peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
			hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ) );
		}
		catch( invalid_argument &ia ){
			// registra el peso tara en cero
			peso_tara = 0.f;
			hora_salida = "";
		}
		
		try{
			descuento = stod( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
		}
		catch( invalid_argument &ia ){
			descuento = 0.f;
		}
		
		// Registra el peso neto
		peso_neto = abs( peso_bruto - peso_tara ) - descuento;

		// Establece si es de entrada o salida
		tipo_registro = TIPO_REGISTRO_SALIDA;
		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "TipoRegistroEntrada" ) ) ) ){
			tipo_registro = TIPO_REGISTRO_ENTRADA;
		}

		stringstream consulta;
		consulta << "update PesajesInternos set id_empresa = " << id_empresa << ", id_producto = " << id_producto << ", "
				 << "numero_placas = '" << numero_placas << "', " << " nombre_conductor = '" << conductor << "', " 
				 << "hora_entrada = '" << hora_entrada << "', hora_salida = " << ( hora_salida.empty() ? "null" : "'" + hora_salida + "'" ) << ", "
				 << "peso_bruto = " << peso_bruto << ", peso_bruto_manual = " << bruto_manual << ", "
				 << "peso_tara = " << peso_tara << ", peso_tara_manual = " << tara_manual << ", "
				 << "descuento = " << descuento << ", peso_neto = " << peso_neto << ", observaciones = '" << observaciones << "', "
				 << "tipo_registro = " << tipo_registro << " where id_pesaje = " << folio;
		
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		// Obtiene los tickets pendientes
		interno_registros_actualizar_pendientes();

		//
		regresarVista();
		
		// Crea o registra el registro según corresponda
		app_mostrar_exito( "Registro actualizado correctamente." );
	}
	catch( invalid_argument &ia ){
		string error = ia.what();
		app_mostrar_error( ia.what() );
		if( error.compare( "stod" ) == 0 ){
			app_mostrar_error( "Es necesario registrar el peso bruto." );
		}
	}
}

void interno_pendiente_finalizar( GtkWidget *widget, gpointer info ){
	try{
		// Obtiene los datos de pesaje
		string fecha = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaInterno" ) ) );
		string id_empresa = empresa_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreEmpresaInterno" ) ) ) );
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoInterno" ) ) ) );
		string conductor = interno_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorInterno" ) ) ) );
		string numero_placas = interno_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasInterno" ) ) ) );
		string observaciones = gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaObservacionesInterno" ) ) );

		// Registra el peso bruto, el tara y revisa el peso neto.
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaInterno" ) ) );
		double peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
		string hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaInterno" ) ) );
		double descuento = 0.d;

		try{
			descuento = stod( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
		}catch( invalid_argument &ia ){
			descuento = 0.d;
		}
		double peso_neto = abs( peso_bruto - peso_tara ) - descuento;
		bool tipo_registro = !gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( buscar_objeto( "TipoRegistroEntrada" ) ) );// Establece si es de entrada o salida

		bool nuevo = false;

		stringstream consulta;

		try{
			unsigned long folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) );
			consulta << "update PesajesInternos set id_empresa = " << id_empresa << ", id_producto = " << id_producto << ", "
				 << "numero_placas = '" << numero_placas << "', " << " nombre_conductor = '" << conductor << "', " 
				 << "hora_entrada = '" << hora_entrada << "', hora_salida = " << ( hora_salida.empty() ? "null" : "'" + hora_salida + "'" ) << ", "
				 << "peso_bruto = " << peso_bruto << ", peso_bruto_manual = " << bruto_manual << ", "
				 << "peso_tara = " << peso_tara << ", peso_tara_manual = " << tara_manual << ", "
				 << "descuento = " << descuento << ", peso_neto = " << peso_neto << ", observaciones = '" << observaciones << "', "
				 << "tipo_registro = " << tipo_registro << ", pendiente = 0 where id_pesaje = " << folio;
		}
		catch( invalid_argument &ia ){
			consulta << "insert into PesajesInternos values( null, '" << fecha << "', " << tipo_registro << ", " 
				 << id_empresa << ", " << id_producto << ", '" << numero_placas << "', '" << conductor << "', '" 
				 << hora_entrada << "', '" << hora_salida << "', " << peso_bruto << ", " << bruto_manual << ", " 
				 << peso_tara << ", " << tara_manual << ", " << descuento << ", " << peso_neto << ", '" << observaciones << "', " 
				 << usuario.obtenerClave() << ", 0 ); select last_insert_rowid() as Pesaje from PesajesInternos";

				 nuevo = true;
		}
		
		database.open( databaseFile );
		database.query( consulta.str() );
		if( nuevo && results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ), (*resultado)[ "Pesaje" ].c_str() );
		}

		// Actualiza la cantidad de producto disponible
		consulta.str( "" );
		consulta << "select * from Existencia where id_producto = " << id_producto;
		database.query( consulta.str() );
		if( results.size() < 1 ){
			string error = "interno_pendiente_finalizar: Ha ocurrido al intentar identificar el renglon de existencias para el producto ";
			throw runtime_error( error );
		}

		// Obtiene la existencia actual referente al producto que se registro
		unordered_map< string, string > *existencia = results.at( 0 );

		double entrante = stod( (* existencia)[ "entrante" ] );
		double saliente = stod( (* existencia)[ "saliente" ] );
		double total = 0.d;

		// Se actualiza la existencia dependiendo el tipo de registro
		if( tipo_registro == TIPO_REGISTRO_ENTRADA ){
			entrante = entrante + peso_neto;
		}
		else{
			saliente = saliente + peso_neto;
		}

		// Obtiene el total
		total = entrante - saliente;

		consulta.str( "" );
		consulta << "update Existencia set entrante = " << entrante << ", "
				 << "saliente = " << saliente << ", total = " << total << " "
				 << "where id_producto = " << id_producto;
		database.query( consulta.str() );
		database.close();

		// Actualiza los tickets pendientes
		interno_registros_actualizar_pendientes();

		interno_hacer_impresion( nullptr, nullptr );

		//
		regresarVista();

		// Crea o registra el registro según corresponda
		app_mostrar_exito( "Registro finalizado correctamente." );
	}
	catch( invalid_argument &ia ){
		string error = ia.what();
		app_mostrar_error( ia.what() );
		if( error.compare( "stod" ) == 0 ){
			app_mostrar_error( "Es necesario registrar el peso bruto y el peso tara." );
		}
	}
}

// Abre el cuadro de dialogo para leer el peso bruto
void interno_leer_bruto( GtkWidget *widget, gpointer info ){
	// Abre el lector
	bascula_lector_abrir();

	// COnecta la señal
	conectar_senal( senal_bascula_registrar_pesaje, G_CALLBACK( interno_registrar_bruto ), nullptr );
}

// Registra el peso bruto
void interno_registrar_bruto( GtkWidget *widget, gpointer info ){
	// Registra el peso bruto
	bascula_registrar_pesaje( "EntradaPesoBrutoInterno", "EntradaHoraEntradaInterno" );
	
	// Establece si el peso bruto fue registrado de forma manual
	bruto_manual = bascula_entrada_manual_habilitado;
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoBrutoManualInterno" ) ) );
	if( bruto_manual ){
		gtk_widget_show( GTK_WIDGET( buscar_objeto( "EntradaPesoBrutoManualInterno" ) ) );
	}

    // Intenta calcular el peso neto
    interno_calcular_neto();
}

// Abre el cuadro de dialogo para leer el peso bruto
void interno_leer_tara( GtkWidget *widget, gpointer info ){
	// Abre el lector
	bascula_lector_abrir();

	// Conecta la señal
	conectar_senal( senal_bascula_registrar_pesaje, G_CALLBACK( interno_registrar_tara ), nullptr );
}

// Registra el peso tara
void interno_registrar_tara( GtkWidget *widget, gpointer info ){
	// Registra el peso tara
	bascula_registrar_pesaje( "EntradaPesoTaraInterno", "EntradaHoraSalidaInterno" );
    
	// Registra si el peso fue registrado de manera manual
	tara_manual = bascula_entrada_manual_habilitado;
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaPesoTaraManualInterno" ) ) );
	if( tara_manual ){
		gtk_widget_show( GTK_WIDGET( buscar_objeto( "EntradaPesoTaraManualInterno" ) ) );
	}

    // Calcula el peso neto
    interno_calcular_neto();
}

// Actualiza el peso neto
void interno_calcular_neto( void ){
	try{
		// Obtiene el peso bruto y tara registrados
		double bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoInterno" ) ) ) );
		double tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraInterno" ) ) ) );
		double descuento = 0.f;

		// Obtiene el descuento
		try{
			descuento = stod( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaDescuentoInterno" ) ) ) );
		}
		catch( invalid_argument &ia ){
			descuento = 0.f;
		}

		// Calcula el peso neto
		double neto = abs( bruto - tara ) - descuento;

		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeError" ) ) );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), pesoString( neto, 2 ).c_str() );
	}
	catch( invalid_argument &ia ){
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoInterno" ) ), "No establecido" );
	}
}

void interno_registros_listar( GtkWidget *widget, gpointer info ){
	cout << "interno_registros_listar" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorRegistrosPesaje" );

	// Consulta los registros a actualizar
	stringstream consulta;
	consulta << "select id_pesaje, numero_placas, fecha, Producto.nombre as producto, Empresas.razon_social as empresa from PesajesInternos "
 			 << "join Producto on PesajesInternos.id_producto = Producto.id_producto "
			 << "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa "
			 << "where pendiente = 0 and fecha = '" << tiempo_leer_fecha_corta() << "'";
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	// Si no hay resultados regresa
	if( results.size() > 0 ){
		for( unordered_map< string, string > *pesaje : results ){
			GError *error = nullptr;
			GtkBuilder *builder = gtk_builder_new();
			GdkPixbuf *imagen = nullptr;
			
			// Clave del registro
			if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoPesajeInterno.glade", &error ) == 0 ){
				throw runtime_error( "Ha ocurrido un error al caragar ElementoPesajeInterno.glade" );
			}

			string id_pesaje = (*pesaje)[ "id_pesaje" ];

			gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeEmpresa" ) ), (*pesaje)[ "empresa" ].c_str() );
				
			gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistrosPesaje" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), stoi( id_pesaje.c_str() ) );
		}
	}

	database.open( databaseFile );
	consulta.str( "" );
	consulta << "select count( id_pesaje ) as cuenta from PesajesInternos where fecha = '" 
	 	     << tiempo_leer_fecha_corta() << "'";
	database.query( consulta.str() );
	database.close();
	if( results.size() > 0 ){
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), (* results.at( 0 ))[ "cuenta" ].c_str() );
	}

	conectar_senal( senal_pesajes_periodo_seleccionar, G_CALLBACK( irHacia ), (void *)"SeleccionarPeriodo" );
	conectar_senal( senal_pesajes_obtener_seleccionados, G_CALLBACK( interno_registros_listar_rango ), nullptr );
	conectar_senal( senal_pesajes_exportar_seleccionados, G_CALLBACK( interno_registros_exportar ), nullptr );
	conectar_senal( senal_pesajes_seleccionar, G_CALLBACK( interno_seleccionar ), nullptr );
	
	irA( "Pesajes", false );
}

void interno_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data ){
	cout << "interno_seleccionar" << endl;
	// Obtiene el folio del registro de pesaje
	unsigned int folio = obtenerFolioSelector( row );

	stringstream consulta;
	consulta << "select id_pesaje, fecha, Empresas.razon_social as empresa, Producto.nombre as producto, numero_placas, "
			 << "nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_bruto_manual, peso_tara, peso_tara_manual, "
			 << "peso_neto, descuento, observaciones, Usuario.nombre || ' ' || Usuario.apellidos as basculista, "
			 << "tipo_registro from PesajesInternos "
			 << "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa "
			 << "join Producto on PesajesInternos.id_producto = Producto.id_producto "
			 << "join Usuario on PesajesInternos.id_basculista = Usuario.id_usuario "
			 << "where id_pesaje = " << folio << endl;

	// Obtiene el registro de pesaje
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();
	if( results.size() > 0 ){
		// Obtiene el pesaje
		unordered_map< string, string > *pesaje = results.at( 0 );

		// Folio
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "FolioInterno" ) ), (* pesaje)[ "id_pesaje" ].c_str() );
		
		// Fecha
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "FechaInterno" ) ), (* pesaje)[ "fecha" ].c_str() );
		
		// Empresa
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreEmpresaInterno" ) ), (* pesaje)[ "empresa" ].c_str() );

		// Producto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreProductoInterno" ) ), (* pesaje)[ "producto" ].c_str() );
		
		// Nombre del conductor
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreConductorInterno" ) ), (* pesaje)[ "nombre_conductor" ].c_str() );
		
		// Numero de placas
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NumeroPlacasInterno" ) ), (* pesaje)[ "numero_placas" ].c_str() );
		
		// Hora entrada y Peso Bruto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraEntradaInterno" ) ), (* pesaje)[ "hora_entrada" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoBrutoInterno" ) ), ((* pesaje)[ "peso_bruto" ] + " Kg.").c_str() );
		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "PesoBrutoManualInterno" ) ) );
		if( stoi( (* pesaje)[ "peso_bruto_manual" ] ) ){
			cout << "peso_bruto_manual" << endl;
			gtk_widget_show( GTK_WIDGET( buscar_objeto( "PesoBrutoManualInterno" ) ) );
		}
		
		// Hora salida y Peso Tara
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraSalidaInterno" ) ), (* pesaje)[ "hora_salida" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoTaraInterno" ) ), ((* pesaje)[ "peso_tara" ] + " Kg." ).c_str() );
		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "PesoTaraManualInterno" ) ) );
		if( stoi( (* pesaje)[ "peso_tara_manual" ] ) ){
			gtk_widget_show( GTK_WIDGET( buscar_objeto( "PesoTaraManualInterno" ) ) );
		}

		// Descuento
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "DescuentoInterno" ) ), ((* pesaje)[ "descuento" ] + " Kg.").c_str()  );

		// Peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoNetoInterno" ) ), ((* pesaje)[ "peso_neto" ] + "Kg.").c_str()  );

		// 
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreBasculistaInterno" ) ), (* pesaje)[ "basculista" ].c_str() );

		gtk_label_set_text( GTK_LABEL( buscar_objeto( "TipoRegistro" ) ), "Salida" );
		if( stoi( (* pesaje)[ "tipo_registro" ] ) == TIPO_REGISTRO_ENTRADA ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "TipoRegistro" ) ), "Entrada" );
		}

		// Conecta las señales
		conectar_senal( senal_interno_eliminar, G_CALLBACK( app_alerta ), (void *)"¿Deseas eliminar este registro?" );
		conectar_senal( senal_interno_imprimir, G_CALLBACK( interno_hacer_impresion ), nullptr );
		conectar_senal( botonSi, G_CALLBACK( interno_eliminar ), nullptr );

		irA( "PesajeInterno", false );
	}
}

void interno_eliminar( GtkWidget *widget, gpointer info ){
	// Obtiene el folio del elemento a eliminar
	unsigned long folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioInterno" ) ) ) );

	// Actualiza la cantidad de producto disponible
	stringstream consulta;
	consulta << "select PesajesInternos.id_producto, entrante, saliente, total, tipo_registro, peso_neto from Existencia "
			 << "join PesajesInternos on PesajesInternos.id_producto = Existencia.id_producto "
			 << "where PesajesInternos.id_pesaje = " << folio;
	
	database.open( databaseFile );
	database.query( consulta.str() );
	if( results.size() < 1 ){
		string error = "interno_pendiente_finalizar: Ha ocurrido al intentar identificar el renglon de existencias para el producto especificado.";
		throw runtime_error( error );
	}

	// Obtiene la existencia actual referente al producto que se registro
	unordered_map< string, string > *existencia = results.at( 0 );

	unsigned long id_producto = stoul( (* existencia)[ "id_producto" ] );
	bool tipo_registro = stoi( (* existencia)[ "tipo_registro" ] );
	double peso_neto = stod( (* existencia)[ "peso_neto" ] );
	double entrante = stod( (* existencia)[ "entrante" ] );
	double saliente = stod( (* existencia)[ "saliente" ] );
	double total = 0.d;

	// Se actualiza la existencia dependiendo el tipo de registro
	if( tipo_registro == TIPO_REGISTRO_ENTRADA ){
		entrante = entrante - peso_neto;
	}
	else{
		saliente = saliente - peso_neto;
	}

	// Obtiene el total
	total = entrante - saliente;

	consulta.str( "" );
	consulta << "update Existencia set entrante = " << entrante << ", "
				<< "saliente = " << saliente << ", total = " << total << " "
				<< "where id_producto = " << id_producto;
	database.query( consulta.str() );
	
	// Elimina el registro de pesaje
	database.query( "delete from PesajesInternos where id_pesaje = " + to_string( folio ) );
	database.close();

	// Cierra la ventana
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

	//
	interno_registros_listar_rango( nullptr, nullptr );

	// Muestra el mensaje de éxito
	app_mostrar_exito( "Pesaje eliminado correctamente." );
}

void interno_registros_actualizar_pendientes( void ){
	cout << "interno_registros_actualizar_pendientes" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorPesajesPendientes" );

	// Consulta los registros a actualizar
	database.open( databaseFile );
	database.query( "select id_pesaje, numero_placas, fecha, Producto.nombre as producto, Empresas.razon_social as empresa from PesajesInternos "
 					"join Producto on PesajesInternos.id_producto = Producto.id_producto "
					 "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa where pendiente = 1" );
	database.close();

	// Si no hay resultados regresa
	if( results.size() < 1 ){
		return;
	}

	for( unordered_map< string, string > *pesaje : results ){
		GError *error = nullptr;
		GtkBuilder *builder = gtk_builder_new();
		GdkPixbuf *imagen = nullptr;
		
		// Clave del registro
		if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoPesajeInterno.glade", &error ) == 0 ){
			throw runtime_error( "Ha ocurrido un error al caragar ElementoPesajeInterno.glade" );
		}

		string id_pesaje = (*pesaje)[ "id_pesaje" ];

		gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeEmpresa" ) ), (*pesaje)[ "empresa" ].c_str() );

			
		gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorPesajesPendientes" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), stoi( id_pesaje ) );
	}
}

void interno_registros_listar_rango(GtkWidget *widget, gpointer info ){
	cout << "interno_registros_listar_rango" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorRegistrosPesaje" );

	// Variables de la fecha de los días seleccionado
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene la fecha
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInicio" ) ), &diaInicio, &mesInicio, &anioInicio );
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaFin" ) ), &diaFin, &mesFin, &anioFin );

	stringstream consulta;
	consulta << "select id_pesaje, numero_placas, fecha, Producto.nombre as producto, Empresas.razon_social as empresa "
			 << "from PesajesInternos join Producto on PesajesInternos.id_producto = Producto.id_producto "
			 << "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa "
			 << "where fecha between '" << tiempo_construir_fecha( diaInicio, mesInicio + 1, anioInicio ) << "' and '" << tiempo_construir_fecha( diaFin, mesFin + 1, anioFin ) << "' "
			 << "order by fecha";

			 cout << consulta.str() << endl;

	// Consulta los registros a actualizar
	database.open( databaseFile );
	database.query( consulta.str() );
	database.close();

	regresarVista();

	// Si no hay resultados regresa
	if( results.size() > 0 ){
		for( unordered_map< string, string > *pesaje : results ){
			GError *error = nullptr;
			GtkBuilder *builder = gtk_builder_new();
			GdkPixbuf *imagen = nullptr;
			
			// Clave del registro
			if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoPesajeInterno.glade", &error ) == 0 ){
				throw runtime_error( "Ha ocurrido un error al caragar ElementoPesajeInterno.glade" );
			}

			string id_pesaje = (*pesaje)[ "id_pesaje" ];

			gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeEmpresa" ) ), (*pesaje)[ "empresa" ].c_str() );
				
			gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistrosPesaje" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "PesajeInterno" ) ), stoi( id_pesaje.c_str() ) );
		}

		app_mostrar_exito( "Registros encontrados." );
	}
	else{
		app_mostrar_error( "No se encontraron registros en el perido seleccionado." );
	}
}

void interno_registros_exportar( GtkWidget *widget, gpointer info ){
	cout << "interno_registros_exportar" << endl;
	// Muestra un mensaje que indica que se está generando el informe
	app_mostrar_mensaje( "Obteniendo registros.\nPor favor espere.");

	// Variables para el rango de seleccion de fechas
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene las fechas
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInicio" ) ), &diaInicio, &mesInicio, &anioInicio  );
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaFin" ) ), &diaFin, &mesFin, &anioFin );

	// Consulta a realizar
    string consulta = "select id_pesaje, fecha, Producto.nombre, numero_placas, nombre_conductor, hora_entrada, "
    				  "hora_salida, peso_bruto, peso_tara, peso_neto, entrada_manual, Usuario.nombre "
    				  "from PesajesInternos join Producto on PesajesInternos.id_producto = Producto.id_producto "
					  "join Usuario on Usuario.id_usuario = PesajesInternos.id_basculista "
    				  "where pendiente = 0 and fecha between '" + tiempo_construir_fecha( diaInicio, mesInicio + 1, anioInicio ) + 
    				  "' and '" + tiempo_construir_fecha( diaFin, mesFin + 1, anioFin ) + "' " + "order by fecha";
    
	// Realiza la consulta y envía los resultados al archivos
	database.open( databaseFile );
	database.query( consulta, "../recursos/datos/Registros.csv" );
    database.close();

    ShellExecute(NULL, "open", "../recursos/datos/Registros.csv", NULL, NULL, SW_HIDE );
	app_mostrar_exito( "Datos exportados correctamente." );
	regresarVista();
}

static void interno_hacer_impresion( GtkWidget* widget, GtkWidget* info ){
    GtkPrintOperation *print = nullptr;
    GtkPrintOperationResult res;
    GError *error;

    print = gtk_print_operation_new();
    if( print == nullptr ){
        throw runtime_error( "Ah ocurrido un error al crear la configuración de la impresora." );
    }

	GtkPrintSettings *settings = gtk_print_settings_new();
    gtk_print_operation_set_n_pages(print, 1);

    guint senal = g_signal_connect(print, "draw_page", G_CALLBACK( interno_imprimir ), info );

    res = gtk_print_operation_run( print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) ), NULL);

    if (res == GTK_PRINT_OPERATION_RESULT_APPLY){
        if (settings != NULL){
            g_object_unref (settings);
        }

        settings = GTK_PRINT_SETTINGS(g_object_ref (gtk_print_operation_get_print_settings (print)));
    }

    g_object_unref( print );
}

static void interno_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data){
    cout << "interno_imprimir" << endl;
    unsigned long folio;
	try{
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioInterno" ) ) ) );
	}
	catch( invalid_argument &ia ){
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioInterno" ) ) ) );
	}
	
	unordered_map< string, string > pesaje;
	unordered_map< string, string > empresa;
	string domicilio = "\n";

	stringstream consulta;
	consulta << "select id_pesaje, fecha, Empresas.razon_social as empresa, Producto.nombre as producto, numero_placas, "
			 << "nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_bruto_manual, peso_tara, peso_tara_manual, "
			 << "peso_neto, descuento, observaciones, Usuario.nombre || ' ' || Usuario.apellidos as basculista, "
			 << "tipo_registro from PesajesInternos "
			 << "join Empresas on PesajesInternos.id_empresa = Empresas.id_empresa "
			 << "join Producto on PesajesInternos.id_producto = Producto.id_producto "
			 << "join Usuario on PesajesInternos.id_basculista = Usuario.id_usuario "
			 << "where id_pesaje = " << folio << endl;

	// Obtiene el registro
	database.open( databaseFile );
	database.query( consulta.str() );
	if( results.size() > 0 ){
		pesaje = (* results.at( 0 ) );
	}
	database.query( "select * from EmpresaPropia where id_empresa = 1" );
	if( results.size() > 0 ){
		empresa = (* results.at( 0 ) );
	}
	database.query( "select * from DomicilioEmpresaPropia" );
	if( results.size() > 0 ){
		unordered_map< string, string > resultado = (* results.at( 0 ));
		domicilio = resultado[ "calle" ] + " " + resultado[ "numero" ] + ", " + resultado[ "colonia" ] + " " +
					resultado[ "codigo_postal" ] + ", " + resultado[ "municipio" ] + ", " + resultado[ "estado" ] + ", " +
					resultado[ "pais" ] + "\n";
	}
	database.close();

	// Ancho y alto de la página
	gdouble ancho, alto;
	GdkRGBA color;
	int a, b, c;

	// Obtiene el contexto de impresión
	cairo_t *cr = gtk_print_context_get_cairo_context( context );
	
	// Obtiene las dimensiones de la página
	ancho = gtk_print_context_get_width( context );
	alto = gtk_print_context_get_height( context );

	cairo_move_to (cr, 0, 0);
	cairo_set_source_rgb( cr, 0, 0, 0 );

	// Crea el formato pango y establece las opciones generales
	PangoLayout* layout = gtk_print_context_create_pango_layout(context);
	PangoFontDescription *fuente = pango_font_description_from_string( "sans 8" );
	pango_layout_set_font_description( layout, fuente );
	pango_font_description_free( fuente );
	pango_layout_set_single_paragraph_mode( layout, FALSE );
	pango_layout_set_width( layout, ancho * PANGO_SCALE );
	pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
	
	string texto = empresa[ "razon_social" ] + "\n" + domicilio;

	// 
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_layout_get_pixel_size( layout, &a, &b );

	pango_cairo_layout_path( cr, layout );
	cairo_move_to (cr, 0, b );

	texto = "Folio: " + pesaje[ "id_pesaje" ] + "\n";
	pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	pango_layout_get_pixel_size( layout, &a, &c );
	cairo_move_to( cr, 0, b );

	texto = "Fecha: " + pesaje[ "fecha" ] + "\n";
	pango_layout_set_alignment( layout, PANGO_ALIGN_LEFT );
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	pango_layout_get_pixel_size( layout, &a, &c );
	b = b + c;
	cairo_move_to( cr, 0, b );

	// Izquierda
	texto = "Empresa: " + pesaje[ "empresa" ] + "\n"
			"Producto: " + pesaje[ "producto" ] + "\n\n"
			"Conductor: " + pesaje[ "nombre_conductor" ] + "\n";
	
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	pango_layout_get_pixel_size( layout, &a, &c );
	b = b + c;

	// Derecha
	string tipo = ( stoi( pesaje[ "tipo_registro" ] ) ? "\nSalida\n" : "\nEntrada\n" ); 
	texto =  "Registra: " + tipo + "Placas: " + pesaje[ "numero_placas" ] + "\n";

	pango_layout_set_alignment( layout, PANGO_ALIGN_RIGHT );
	pango_layout_set_text( layout, texto.c_str(), -1 );
	pango_cairo_layout_path( cr, layout );
	cairo_move_to( cr, 0, b );

	texto = "Peso bruto: \n"
			"Peso tara: \n\n"
			"Peso neto: \n";

	pango_layout_set_alignment( layout, PANGO_ALIGN_LEFT );
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	cairo_move_to( cr, 0, b );

	texto = pesaje[ "peso_bruto" ] + " Kg\n" +
			pesaje[ "peso_tara" ]  +" Kg\n\n" +
			pesaje[ "peso_neto" ] + " Kg\n";

	pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	b = b + c;
	cairo_move_to( cr, 0, b );

	texto = "\n\n\n_________________________\n"
			"Autoriza\n\n"
			"Realizado con Sistema de Control de Báscula\n";
	pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	b = b + c;
	cairo_move_to( cr, 0, b );

    //
    cairo_fill (cr);
}

// VALIDACION DE DATOS
string interno_conductor_validar( std::string nombre ){
	// Formato para validar conductor
	regex formato( "[a-zA-ZÑñáéíóúÁÉÍÓÚ\\s]*" );
	
	if( nombre.empty() ){
		throw invalid_argument( "Debe introducir el nombre del conductor." );
	}

	if( nombre.size() > 35 || !regex_match( nombre, formato ) ){
		throw invalid_argument( "El nombre del conductor introducido no es válido." );
	}

	return nombre;
}

string interno_placas_validar( std::string placas ){
	// Regex para validar el número económico
	regex formato( "[A-Z0-9Ñ]*" );

	// Verifica que el número de placas no exceda el formato máximo
	if( placas.size() > 7 ){
		throw invalid_argument( "El número de placa del vehículo no puede exceder los 7 caracteres." );
	}
	
	// Verifica el formato de lo introducido
	if( !regex_match( placas, formato ) ){
		throw invalid_argument( "No se pudo establecer el número de placas.\nSolo admite letras y números." );
	}
	
	return placas;
}
