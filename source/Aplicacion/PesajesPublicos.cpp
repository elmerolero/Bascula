#include "PesajesPublicos.h"
#include "Aplicacion.h"
#include "Funciones.h"
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

Signal senal_publico_opcion = { "BotonRegistrosBasculaPublica", "clicked", 0 };
Signal senal_publico_numero_placas = { "EntradaNumeroPlacasPublico", "insert-text", 0 };

Signal senal_publico_pendientes_listar = { "BotonBasculaPublica", "clicked", 0 };
Signal senal_publico_editar_pendiente = { "BotonRegistrarPendientePublico", "clicked", 0 };
Signal senal_publico_pendiente_finalizar = { "BotonFinalizarPendientePublico", "clicked", 0 };;

Signal senal_publico_leer_bruto = { "BotonLeerPesoBrutoPublico", "clicked", 0 };
Signal senal_publico_leer_tara = { "BotonLeerPesoTaraPublico", "clicked", 0 };

Signal senal_publico_editar = { "ContenedorPesajesPendientes", "row-activated", 0 };

Signal senal_pesajes_nuevo_pendiente = { "BotonBasculaNuevo", "clicked", 0 };
Signal senal_pesajes_periodo_seleccionar = { "BotonRegistrosPesajeSeleccionarDia", "clicked", 0 };
Signal senal_pesajes_obtener_seleccionados = { "BotonObtenerRegistrosRango", "clicked", 0 };
Signal senal_pesajes_seleccionar = { "ContenedorRegistrosPesaje", "row-activated", 0 };
Signal senal_publico_eliminar = { "EliminarRegistroPublico", "clicked", 0 };
Signal senal_pesajes_exportar_seleccionados = { "BotonRegistrosPesajesExportar", "clicked", 0 };
Signal senal_publico_imprimir = { "ImprimirRegistroPublico", "clicked", 0 };

GtkListStore *listaPublicoConductores = nullptr;
GtkListStore *listaPublicoPlacas = nullptr;

GtkEntryCompletion *completadorPublicoProducto = nullptr;
GtkEntryCompletion *completadorPublicoConductor = nullptr;
GtkEntryCompletion *completadorPublicoPlacas = nullptr;

// Inicializa los completadores
void publico_iniciar(){
	// Inicializa el completador de productos
	completadorPublicoProducto = gtk_entry_completion_new();

	// Se se creó una lista de empresas y un completador interno se asocian a la entrada de registros internos
	if( completadorPublicoProducto != nullptr && listaProductos != nullptr ){
		gtk_entry_completion_set_model( completadorPublicoProducto, GTK_TREE_MODEL( listaProductos ) );
		gtk_entry_completion_set_text_column( completadorPublicoProducto, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), completadorPublicoProducto );
	}

	// Crea una nueva lista para los conductores
    listaPublicoConductores = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todos los productos
    database.open( databaseFile );
    database.query( "select DISTINCT nombre_conductor from PesajesPublicos" );
    database.close();
    if( results.size() > 0 ){
        for( auto conductor : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaPublicoConductores, &iterador );
            gtk_list_store_set( listaPublicoConductores, &iterador, 0, ( (* conductor)[ "nombre_conductor" ] ).c_str(), -1 );
        }
    }

	// Inicializa el completador de conductores
	completadorPublicoConductor = gtk_entry_completion_new();
	if( completadorPublicoConductor != nullptr && listaPublicoConductores != nullptr ){
		gtk_entry_completion_set_model( completadorPublicoConductor, GTK_TREE_MODEL( listaPublicoConductores ) );
		gtk_entry_completion_set_text_column( completadorPublicoConductor, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ), completadorPublicoConductor );
	}

	// Crea una nueva lista para los números de placa
    listaPublicoPlacas = gtk_list_store_new( 1, G_TYPE_STRING );

    // Obtiene el nombre de todos los número de placa
    database.open( databaseFile );
    database.query( "select DISTINCT numero_placas from PesajesPublicos" );
    database.close();
    if( results.size() > 0 ){
        for( auto placa : results ){
            GtkTreeIter iterador;
            gtk_list_store_append( listaPublicoPlacas, &iterador );
            gtk_list_store_set( listaPublicoPlacas, &iterador, 0, ( (* placa)[ "numero_placas" ] ).c_str(), -1 );
        }
    }

	// Inicializa el completador de las placas
	completadorPublicoPlacas = gtk_entry_completion_new();
	if( completadorPublicoPlacas != nullptr && listaPublicoPlacas != nullptr ){
		gtk_entry_completion_set_model( completadorPublicoPlacas, GTK_TREE_MODEL( listaPublicoPlacas ) );
		gtk_entry_completion_set_text_column( completadorPublicoPlacas, 0);
		gtk_entry_set_completion( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ), completadorPublicoPlacas);
	}
}

void publico_registros_listar_pendientes( GtkWidget *widget, gpointer info ){
	cout << "publico_registros_listar_pendientes" << endl;
	
	// Actualiza la lista de registros pendientes
	publico_registros_actualizar_pendientes();

	// Conecta las señales
    conectar_senal( senal_publico_numero_placas, G_CALLBACK( convertirMayusculas ), nullptr );
    conectar_senal( senal_publico_leer_bruto, G_CALLBACK( publico_leer_bruto ), nullptr );
    conectar_senal( senal_publico_leer_tara, G_CALLBACK( publico_leer_tara ), nullptr );
	conectar_senal( senal_publico_pendiente_finalizar, G_CALLBACK( publico_pendiente_finalizar ), nullptr );
	conectar_senal( senal_pesajes_nuevo_pendiente, G_CALLBACK( publico_pendiente_nuevo ), nullptr );
	conectar_senal( senal_publico_editar, G_CALLBACK( publico_pendiente_seleccionar ), nullptr );

	// Redirige a la vista de pesajes pendientes
	irA( "PesajesPendientes", false );
}

void publico_pendiente_nuevo( GtkWidget *widget, gpointer info ){
	// Folio
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), "No asignado." );
	
	// Fecha
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ), tiempo_leer_fecha_corta().c_str() );
	
	// Producto
	gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNombreProductoPublico" ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), "" );
	
	// Nombre del conductor
	gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNombreConductorPublico"  ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ), "" );
	
	// Numero de placas
	gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNumeroPlacasPublico"  ) ), TRUE );
	gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ), "" );
	
	// Hora entrada y Peso Bruto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ), "No establecida" );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ), "No establecido" );
	
	// Hora salida y Peso Tara
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ), "No establecida" );
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ), "No establecido" );
	
	// Peso neto
	gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), "No establecido" );

	// Conecta la señal
	conectar_senal( senal_publico_editar_pendiente, G_CALLBACK( publico_pendiente_guardar_nuevo ), nullptr );
	
	// Establece la vista de nuevo ticket
	irA( "PesajePublicoEditable", false );
}

void publico_pendiente_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data ){
	cout << "publico_pendiente_seleccionar" << endl;
	// Obtiene el folio del registro de pesaje
	unsigned int folio = obtenerFolioSelector( row );

	string consulta = "select id_pesaje, fecha, Producto.nombre as producto, numero_placas, nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_tara, peso_neto "
					  "from PesajesPublicos join Producto on Producto.id_producto = PesajesPublicos.id_producto " 
					  "where id_pesaje = ";

	// Obtiene el registro de pesaje
	database.open( databaseFile );
	database.query( consulta + to_string( folio ) );
	database.close();
	if( results.size() > 0 ){
		// Obtiene el pesaje
		unordered_map< string, string > *pesaje = results.at( 0 );

		// Folio
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), (* pesaje)[ "id_pesaje" ].c_str() );
		
		// Fecha
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ), (* pesaje)[ "fecha" ].c_str() );
		
		// Producto
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNombreProductoPublico" ) ), TRUE );
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ), (* pesaje)[ "producto" ].c_str() );
		
		// Nombre del conductor
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNombreConductorPublico"  ) ), TRUE );
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ), (* pesaje)[ "nombre_conductor" ].c_str() );
		
		// Numero de placas
		gtk_editable_set_editable( GTK_EDITABLE( buscar_objeto( "EntradaNumeroPlacasPublico"  ) ), TRUE );
		gtk_entry_set_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ), (* pesaje)[ "numero_placas" ].c_str() );
		
		// Hora entrada y Peso Bruto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ), (* pesaje)[ "hora_entrada" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ), ((* pesaje)[ "peso_bruto" ] + " Kg.").c_str() );
		
		// Hora salida y Peso Tara
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ), "No establecida" );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ), "No establecido" );
		if( (* pesaje)[ "peso_tara" ].compare( "null" ) != 0 && (* pesaje)[ "hora_salida" ].compare( "null" ) != 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ), (* pesaje)[ "hora_salida" ].c_str() );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ), ((* pesaje)[ "peso_tara" ] + " Kg." ).c_str() );
		}
		
		// Peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), "No establecido" );
		if( (* pesaje)[ "peso_neto" ].compare( "null" ) != 0 ){
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), ((* pesaje)[ "peso_neto" ] + "Kg.").c_str()  );
		}

		// Conecta las señales
		conectar_senal( senal_publico_editar_pendiente, G_CALLBACK( publico_pendiente_guardar_edicion ), nullptr );

		irA( "PesajePublicoEditable", false );
	}
}

void publico_pendiente_guardar_nuevo( GtkWidget *widget, gpointer info ){
	try{
		// Obtiene el producto introducido
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ) ) );
    	string fecha = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ) );
		string conductor = publico_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ) ) );
		string numero_placas = publico_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ) ) );
		string id_basculista = to_string( usuario.obtenerClave() );
		//registroPublico -> establecerPendiente( true );
		//registroPublico -> establecerEntradaManual( 0 );// lectorBascula.lecturaManualActivada() );

		// Registra el peso bruto
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ) );
		double peso_tara = 0.f;
		string hora_salida;
		double peso_neto = 0.f;
		
		try{
			// Registra el peso tara
			peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
			hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ) );
		}
		catch( invalid_argument &ia ){
			// registra el peso tara en cero
			peso_tara = 0.f;
			hora_salida = "";
		}
		
		// Registra el peso neto
		peso_neto = abs( peso_bruto - peso_tara );

		stringstream consulta;
		consulta << "insert into PesajesPublicos values( null, '" << fecha << "', " << id_producto << ", '"
				 << numero_placas << "', '" << conductor << "', '" << hora_entrada << "', '" << hora_salida << "', "
				 << peso_bruto << ", " << peso_tara << ", "  << peso_neto << ", " << usuario.obtenerClave() << ", 1, "
				 << bascula_entrada_manual_habilitado << " );  select last_insert_rowid() as Pesaje from PesajesPublicos";
		
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();
		if( results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), (*resultado)[ "Pesaje" ].c_str() );
		}

		publico_hacer_impresion_parcial();

		// Obtiene los tickets pendientes
		publico_registros_actualizar_pendientes();

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

void publico_pendiente_guardar_edicion( GtkWidget *widget, gpointer info ){
	try{
		// Obtiene el producto introducido
		string folio = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) );
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ) ) );
    	string fecha = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ) );
		string conductor = publico_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ) ) );
		string numero_placas = publico_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ) ) );
		string id_basculista = to_string( usuario.obtenerClave() );

		// Registra el peso bruto
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ) );
		double peso_tara = 0.f;
		string hora_salida;
		double peso_neto = 0.f;
		
		try{
			// Registra el peso tara
			peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
			hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ) );
		}
		catch( invalid_argument &ia ){
			// registra el peso tara en cero
			peso_tara = 0.f;
			hora_salida = "";
		}
		
		// Registra el peso neto
		peso_neto = abs( peso_bruto - peso_tara );

		stringstream consulta;
		consulta << "update PesajesPublicos set id_producto = " << id_producto << ", "
				 << "numero_placas = '" << numero_placas << "', " << " nombre_conductor = '" << conductor << "', " 
				 << "hora_entrada = '" << hora_entrada << "', hora_salida = '" << hora_salida << "', "
				 << "peso_bruto = " << peso_bruto << ", peso_tara = " << peso_tara << ", peso_neto = "  << peso_neto << ", "
				 << "entrada_manual = " << bascula_entrada_manual_habilitado << "  where id_pesaje = " << folio;
		
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();
		if( results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), (*resultado)[ "Pesaje" ].c_str() );
		}

		publico_hacer_impresion_parcial();

		// Obtiene los tickets pendientes
		publico_registros_actualizar_pendientes();

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

void publico_pendiente_finalizar( GtkWidget *widget, gpointer info ){
	try{
		// Obtiene el producto introducido
		string folioStr = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) );
   		string id_producto = producto_buscar_existente( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreProductoPublico" ) ) ) );
    	string fecha = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFechaPublico" ) ) );
		string conductor = publico_conductor_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNombreConductorPublico" ) ) ) );
		string numero_placas = publico_placas_validar( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaNumeroPlacasPublico" ) ) ) );
		string id_basculista = to_string( usuario.obtenerClave() );

		// Registra el peso bruto, el tara y revisa el peso neto.
		double peso_bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
		string hora_entrada = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraEntradaPublico" ) ) );
		double peso_tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );
		string hora_salida = gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaHoraSalidaPublico" ) ) );;
		double peso_neto = abs( peso_bruto - peso_tara );

		bool nuevo = false;

		stringstream consulta;

		try{
			unsigned long folio = stoul( folioStr );
			consulta << "update PesajesPublicos set id_producto = " << id_producto << ", "
				 << "numero_placas = '" << numero_placas << "', " << " nombre_conductor = '" << conductor << "', " 
				 << "hora_entrada = '" << hora_entrada << "', hora_salida = '" << hora_salida << "', "
				 << "peso_bruto = " << peso_bruto << ", peso_tara = " << peso_tara << ", peso_neto = "  << peso_neto << ", "
				 << "pendiente = 0, entrada_manual = " << bascula_entrada_manual_habilitado << "  where id_pesaje = " << folio;
		}
		catch( invalid_argument &ia ){
			consulta << "insert into PesajesPublicos values( null, '" << fecha << "', " << id_producto << ", '"
				 << numero_placas << "', '" << conductor << "', '" << hora_entrada << "', '" << hora_salida << "', "
				 << peso_bruto << ", " << peso_tara << ", "  << peso_neto << ", " << usuario.obtenerClave() << ", 0, "
				 << bascula_entrada_manual_habilitado << " ); select last_insert_rowid() as Pesaje from PesajesPublicos";
		
			nuevo = true;
		}
		
		database.open( databaseFile );
		database.query( consulta.str() );
		database.close();

		if( nuevo && results.size() > 0 ){
			unordered_map< string, string > *resultado = results.at( 0 );
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ), (*resultado)[ "Pesaje" ].c_str() );
		}

		// Obtiene los tickets pendientes
		publico_registros_actualizar_pendientes();

		publico_hacer_impresion( nullptr, nullptr );

		//
		regresarVista();

		// Crea o registra el registro según corresponda
		app_mostrar_exito( "Registro finalizado correctamente." );
	}
	catch( invalid_argument &ia ){
		string error = ia.what();
		app_mostrar_error( ia.what() );
		if( error.compare( "stod" ) == 0 ){
			app_mostrar_error( "Es necesario registrar el peso bruto." );
		}
	}
}

// Abre el cuadro de dialogo para leer el peso bruto
void publico_leer_bruto( GtkWidget *widget, gpointer info ){
	// Abre el lector
	bascula_lector_abrir();

	// COnecta la señal
	conectar_senal( senal_bascula_registrar_pesaje, G_CALLBACK( publico_registrar_bruto ), nullptr );
}

// Registra el peso bruto
void publico_registrar_bruto( GtkWidget *widget, gpointer info ){
	// Registra el peso bruto
	bascula_registrar_pesaje( "EntradaPesoBrutoPublico", "EntradaHoraEntradaPublico" );
	
    // Intenta calcular el peso neto
    publico_calcular_neto();
}

// Abre el cuadro de dialogo para leer el peso bruto
void publico_leer_tara( GtkWidget *widget, gpointer info ){
	// Abre el lector
	bascula_lector_abrir();

	// COnecta la señal
	conectar_senal( senal_bascula_registrar_pesaje, G_CALLBACK( publico_registrar_tara ), nullptr );
}

// Registra el peso tara
void publico_registrar_tara( GtkWidget *widget, gpointer info ){
	// Registra el peso tara
	bascula_registrar_pesaje( "EntradaPesoTaraPublico", "EntradaHoraSalidaPublico" );
    
    // Calcula el peso neto
    publico_calcular_neto();
}

// Actualiza el peso neto
void publico_calcular_neto( void ){
	try{
		// Obtiene el peso bruto y tara registrados
		double bruto = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoBrutoPublico" ) ) ) );
		double tara = stod( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaPesoTaraPublico" ) ) ) );

		// Calcula el peso neto
		double neto = abs( bruto - tara );

		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "MensajeError" ) ) );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), pesoString( neto, 2 ).c_str() );
	}
	catch( invalid_argument &ia ){
		// No pasa nada, solo indicamos que no se estableció el peso neto e impedimos que lo registre como finalizado
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "EntradaPesoNetoPublico" ) ), "No establecido" );
	}
}

void publico_registros_listar( GtkWidget *widget, gpointer info ){
	cout << "publico_registros_listar" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorRegistrosPesaje" );

	// Consulta los registros a actualizar
	cout << tiempo_leer_fecha_corta() << endl;
	string consulta = "select id_pesaje, numero_placas, fecha, Producto.nombre as producto from PesajesPublicos " 
					  "join Producto on PesajesPublicos.id_producto = Producto.id_producto where pendiente = 0 and fecha = '";
	database.open( databaseFile );
	database.query( consulta + tiempo_leer_fecha_corta() + "'" );
	database.close();

	// Si no hay resultados regresa
	if( results.size() > 0 ){
		for( unordered_map< string, string > *pesaje : results ){
			GError *error = nullptr;
			GtkBuilder *builder = gtk_builder_new();
			GdkPixbuf *imagen = nullptr;
			
			// Clave del registro
			if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoTicketPublico.glade", &error ) == 0 ){
				throw runtime_error( "Ha ocurrido un error al caragar ElementoTicketPublico.glade" );
			}

			string id_pesaje = (*pesaje)[ "id_pesaje" ];

			gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
				
			gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistrosPesaje" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), stoi( id_pesaje.c_str() ) );
		}
	}

	database.open( databaseFile );
	consulta = "select count( id_pesaje ) as cuenta from PesajesPublicos where fecha = '";
	database.query( consulta + tiempo_leer_fecha_corta() + "'" );
	database.close();
	if( results.size() > 0 ){
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "TicketsContados" ) ), (* results.at( 0 ))[ "cuenta" ].c_str() );
	}

	conectar_senal( senal_pesajes_periodo_seleccionar, G_CALLBACK( irHacia ), (void *)"SeleccionarPeriodo" );
	conectar_senal( senal_pesajes_obtener_seleccionados, G_CALLBACK( publico_registros_listar_rango ), nullptr );
	conectar_senal( senal_pesajes_exportar_seleccionados, G_CALLBACK( publico_registros_exportar ), nullptr );
	conectar_senal( senal_pesajes_seleccionar, G_CALLBACK( publico_seleccionar ), nullptr );
	
	irA( "Pesajes", false );
}

void publico_seleccionar( GtkWidget *widget, GtkListBoxRow *row, gpointer data ){
	cout << "publico_seleccionar" << endl;
	// Obtiene el folio del registro de pesaje
	unsigned int folio = obtenerFolioSelector( row );

	string consulta = "select id_pesaje, fecha, Producto.nombre as producto, numero_placas, nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_tara, peso_neto, entrada_manual "
					  "from PesajesPublicos join Producto on Producto.id_producto = PesajesPublicos.id_producto " 
					  "where id_pesaje = ";

	// Obtiene el registro de pesaje
	database.open( databaseFile );
	database.query( consulta + to_string( folio ) );
	database.close();
	if( results.size() > 0 ){
		// Obtiene el pesaje
		unordered_map< string, string > *pesaje = results.at( 0 );

		// Folio
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ), (* pesaje)[ "id_pesaje" ].c_str() );
		
		// Fecha
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "FechaPublico" ) ), (* pesaje)[ "fecha" ].c_str() );
		
		// Producto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreProductoPublico" ) ), (* pesaje)[ "producto" ].c_str() );
		
		// Nombre del conductor
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreConductorPublico" ) ), (* pesaje)[ "nombre_conductor" ].c_str() );
		
		// Numero de placas
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NumeroPlacasPublico" ) ), (* pesaje)[ "numero_placas" ].c_str() );
		
		// Hora entrada y Peso Bruto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraEntradaPublico" ) ), (* pesaje)[ "hora_entrada" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoBrutoPublico" ) ), ((* pesaje)[ "peso_bruto" ] + " Kg.").c_str() );
		
		// Hora salida y Peso Tara
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "HoraSalidaPublico" ) ), (* pesaje)[ "hora_salida" ].c_str() );
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoTaraPublico" ) ), ((* pesaje)[ "peso_tara" ] + " Kg." ).c_str() );
		
		// Peso neto
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "PesoNetoPublico" ) ), ((* pesaje)[ "peso_neto" ] + "Kg.").c_str()  );

		// 
		gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreBasculistaPublico" ) ), ( usuario.obtenerNombre() + " " + usuario.obtenerApellidos() ).c_str() );

		gtk_widget_hide( GTK_WIDGET( buscar_objeto( "EntradaManualPublico" ) ) );
		if( ( stoi( (* pesaje)[ "entrada_manual" ] ) ) ){
			gtk_widget_show( GTK_WIDGET( buscar_objeto( "EntradaManualPublico" ) ) );
		}

		// Conecta las señales
		conectar_senal( senal_publico_editar_pendiente, G_CALLBACK( publico_pendiente_guardar_edicion ), nullptr );
		conectar_senal( senal_publico_eliminar, G_CALLBACK( app_alerta ), (void *)"¿Deseas eliminar este registro?" );
		conectar_senal( senal_publico_imprimir, G_CALLBACK( publico_hacer_impresion ), nullptr );
		conectar_senal( botonSi, G_CALLBACK( publico_eliminar ), nullptr );

		irA( "PesajePublico", false );
	}
}

void publico_eliminar( GtkWidget *widget, gpointer info ){
	// Obtiene el folio del elemento a eliminar
	unsigned long folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ) ) );

	// Obtiene la cantidad extraída del producto
	database.open( databaseFile );
	database.query( "delete from PesajesPublicos where id_pesaje = " + to_string( folio ) );
	database.close();

	// Cierra la ventana
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "VentanaSiNo" ) ) );

	//
	publico_registros_listar_rango( nullptr, nullptr );

	// Muestra el mensaje de éxito
	app_mostrar_exito( "Pesaje eliminado correctamente." );
}

void publico_registros_actualizar_pendientes( void ){
	cout << "publico_registros_actualizar_pendientes" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorPesajesPendientes" );

	// Consulta los registros a actualizar
	database.open( databaseFile );
	database.query( "select id_pesaje, numero_placas, fecha, Producto.nombre as producto from PesajesPublicos "
 					"join Producto on PesajesPublicos.id_producto = Producto.id_producto where pendiente = 1" );
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
		if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoTicketPublico.glade", &error ) == 0 ){
			throw runtime_error( "Ha ocurrido un error al caragar ElementoTicketPublico.glade" );
		}

		string id_pesaje = (*pesaje)[ "id_pesaje" ];

		gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
		gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
			
		gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorPesajesPendientes" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), stoi( id_pesaje.c_str() ) );
	}
}

void publico_registros_listar_rango(GtkWidget *widget, gpointer info ){
	cout << "publico_registros_listar_rango" << endl;
	// Limpia el contenedor
	limpiar_contenedor( "ContenedorRegistrosPesaje" );

	// Variables de la fecha de los días seleccionado
	unsigned int diaInicio, mesInicio, anioInicio;
	unsigned int diaFin, mesFin, anioFin;

	// Obtiene la fecha
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaInicio" ) ), &diaInicio, &mesInicio, &anioInicio );
	gtk_calendar_get_date( GTK_CALENDAR( buscar_objeto( "FechaFin" ) ), &diaFin, &mesFin, &anioFin );

	stringstream consulta;
	consulta << "select id_pesaje, numero_placas, fecha, Producto.nombre as producto from PesajesPublicos join Producto on PesajesPublicos.id_producto = Producto.id_producto "
			 << " where fecha between '" << tiempo_construir_fecha( diaInicio, mesInicio + 1, anioInicio ) << "' and '" << tiempo_construir_fecha( diaFin, mesFin + 1, anioFin ) << "' "
			 << " order by fecha";

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
			if( gtk_builder_add_from_file( builder, "../recursos/interfaces/ElementoTicketPublico.glade", &error ) == 0 ){
				throw runtime_error( "Ha ocurrido un error al caragar ElementoTicketPublico.glade" );
			}

			string id_pesaje = (*pesaje)[ "id_pesaje" ];

			gtk_widget_set_name( GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFolio" ) ), (*pesaje)[ "id_pesaje" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajePlaca" ) ), (*pesaje)[ "numero_placas" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeFecha" ) ), (*pesaje)[ "fecha" ].c_str() );
			gtk_label_set_text( GTK_LABEL( gtk_builder_get_object( builder, "ElementoPesajeProducto" ) ), (*pesaje)[ "producto" ].c_str() );
				
			gtk_list_box_insert( GTK_LIST_BOX( buscar_objeto( "ContenedorRegistrosPesaje" ) ), GTK_WIDGET( gtk_builder_get_object( builder, "Pesaje" ) ), stoi( id_pesaje.c_str() ) );
		}

		app_mostrar_exito( "Registros encontrados." );
	}
	else{
		app_mostrar_error( "No se encontraron registros en el perido seleccionado." );
	}
}

void publico_registros_exportar( GtkWidget *widget, gpointer info ){
	cout << "publico_registros_exportar" << endl;
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
    				  "from PesajesPublicos join Producto on PesajesPublicos.id_producto = Producto.id_producto "
					  "join Usuario on Usuario.id_usuario = PesajesPublicos.id_basculista "
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

static void publico_hacer_impresion_parcial( void){
    GtkPrintOperation *print = nullptr;
    GtkPrintOperationResult res;
    GError *error;

    print = gtk_print_operation_new();
    if( print == nullptr ){
        throw runtime_error( "Ah ocurrido un error al crear la configuración de la impresora." );
    }

	GtkPrintSettings *settings = gtk_print_settings_new();
    gtk_print_operation_set_n_pages(print, 1);

    guint senal = g_signal_connect(print, "draw_page", G_CALLBACK( publico_imprimir_parcial ), nullptr );

    res = gtk_print_operation_run( print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) ), NULL);

    if (res == GTK_PRINT_OPERATION_RESULT_APPLY){
        if (settings != NULL){
            g_object_unref (settings);
        }

        settings = GTK_PRINT_SETTINGS(g_object_ref (gtk_print_operation_get_print_settings (print)));
    }

    g_object_unref( print );
}

static void publico_hacer_impresion( GtkWidget* widget, GtkWidget* info ){
    GtkPrintOperation *print = nullptr;
    GtkPrintOperationResult res;
    GError *error;

    print = gtk_print_operation_new();
    if( print == nullptr ){
        throw runtime_error( "Ah ocurrido un error al crear la configuración de la impresora." );
    }

	GtkPrintSettings *settings = gtk_print_settings_new();
    gtk_print_operation_set_n_pages(print, 1);

    guint senal = g_signal_connect(print, "draw_page", G_CALLBACK( publico_imprimir ), info );

    res = gtk_print_operation_run( print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) ), NULL);

    if (res == GTK_PRINT_OPERATION_RESULT_APPLY){
        if (settings != NULL){
            g_object_unref (settings);
        }

        settings = GTK_PRINT_SETTINGS(g_object_ref (gtk_print_operation_get_print_settings (print)));
    }

    g_object_unref( print );
}

static void  publico_imprimir_parcial(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data){
    cout << "publico_imprimir_parcial" << endl;
	unsigned long folio;
	try{
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) );
	}
	catch( exception e ){
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ) ) );
	}
    
	unordered_map< string, string > pesaje;
	unordered_map< string, string > empresa;
	string domicilio = "\n";

	string consulta = "select id_pesaje, fecha, Producto.nombre as producto, numero_placas, nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_tara, peso_neto "
					  "from PesajesPublicos join Producto on Producto.id_producto = PesajesPublicos.id_producto " 
					  "where id_pesaje = " + to_string( folio );

	// Obtiene el registro
	database.open( databaseFile );
	database.query( consulta );
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

	// Derecha
	texto = "Producto: " + pesaje[ "producto" ] + "\n\n"
			"Conductor: " + pesaje[ "nombre_conductor" ] + "\n";
	
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	pango_layout_get_pixel_size( layout, &a, &c );
	b = b + c;
	cairo_move_to( cr, 0, b );

	texto = "Peso bruto: \n";

	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	cairo_move_to( cr, 0, b );

	texto = pesaje[ "peso_bruto" ] + " Kg\n";

	pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	b = b + c;
	cairo_move_to( cr, 0, b );

	texto = "Realizado con Sistema de Control de Báscula\n";
	pango_layout_set_alignment( layout, PANGO_ALIGN_CENTER );
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	b = b + c;
	cairo_move_to( cr, 0, b );

    //
    cairo_fill (cr);
}

static void publico_imprimir(GtkPrintOperation *operation, GtkPrintContext *context, int page_nr, GtkWidget *data){
    cout << "publico_imprimir" << endl;
	unsigned long folio;
	try{
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "EntradaFolioPublico" ) ) ) );
	}
	catch( exception e ){
		folio = stoul( gtk_label_get_text( GTK_LABEL( buscar_objeto( "FolioPublico" ) ) ) );
	}
    
	unordered_map< string, string > pesaje;
	unordered_map< string, string > empresa;
	string domicilio = "\n";

	string consulta = "select id_pesaje, fecha, Producto.nombre as producto, numero_placas, nombre_conductor, hora_entrada, hora_salida, peso_bruto, peso_tara, peso_neto "
					  "from PesajesPublicos join Producto on Producto.id_producto = PesajesPublicos.id_producto " 
					  "where id_pesaje = " + to_string( folio );

	// Obtiene el registro
	database.open( databaseFile );
	database.query( consulta );
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

	// Derecha
	texto = "Producto: " + pesaje[ "producto" ] + "\n\n"
			"Conductor: " + pesaje[ "nombre_conductor" ] + "\n";
	
	pango_layout_set_text( layout, texto.c_str(), -1);
	pango_cairo_layout_path( cr, layout );
	pango_layout_get_pixel_size( layout, &a, &c );
	b = b + c;
	cairo_move_to( cr, 0, b );

	texto = "Peso bruto: \n"
			"Peso tara: \n\n"
			"Peso neto: \n";

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
string publico_conductor_validar( std::string nombre ){
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

string publico_placas_validar( std::string placas ){
	// Regex para validar el número económico
	regex formato( "[A-Z0-9Ñ]*" );
	
	// Verifica el formato de lo introducido
	if( placas.size() > 7 || !regex_match( placas, formato ) ){
		throw invalid_argument( "No se pudo establecer el número de placas.\nSolo admite letras, números y contiene 7 caracteres." );
	}
	
	return placas;
}
