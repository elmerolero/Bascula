#include "Sesion.h"
#include "GestorRegistros.h"
#include "sha256.h"
#include "Usuario.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include "Imagen.h"
#include "Vistas.h"
#include "Funciones.h"
using namespace std;

string nombreUsuario;
string codigoRecuperacion;

Signal enlaceSesionCerrar{ "EnlaceCerrarSesion", "activate-link", 0 };

void registrarUsuario(){
	cout << "usuario_registrar" << endl;
	try{
		// Obtiene los datos del formulario
		
		string nombre = Usuario::validarNombre( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRegistroNombre" ) ) ) );
		string apellidos = Usuario::validarApellidos( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRegistroApellidos" ) ) ) );
		string nombreUsuario = Usuario::validarNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRegistroNombreUsuario" ) ) ) );
		string contrasena = Usuario::validarContrasena( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRegistroContrasena" ) ) ) );
		string confirmacion = Usuario::validarContrasena( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaRegistroConfirmarContrasena" ) ) ) );
		
		// Se asegura de que el usuario que se desea registrar no existe
		usuario_PseudonimoOcupado( nombreUsuario );
		
		// Se asegura que las contraseñas introducidas sean iguales
		Usuario::compararContrasenas( contrasena, confirmacion );
		
		// Crea la contrasena que se insertará dentro de la base de datos
		string contrasenaFinal = crearHash( contrasena );
		
		string administrador = existenUsuariosRegistrados() ? "0" : "1";

		// Registra al usuario en la base de datos
		database.open( databaseFile );
		database.query( "insert into Usuario values( null, '" + nombreUsuario + "', '" +  contrasenaFinal + "', '" + nombre + "', '" + apellidos + "', null, null, null, '" + tiempo_leer_fecha_corta() + "', null, " + administrador + " )" );
		database.close();
		
		// Muestra un mensaje de que efectivamente se registro al usuario y lo redirige a la página de inicio de sesion
		app_mostrar_mensaje( "¡Bienvenido!" );
		irA( "IniciarSesion", true );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() ); 
	}
}

void iniciarSesion(){
	cout << "iniciar_sesion" << endl;
	// Conecta con la base de datos
	database.open( databaseFile );
		
	try{
		// Establece el nombre de usuario
		string nombreUsuario = Usuario::validarNombreUsuario( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaUsuario" ) ) ) );
		string contrasena = Usuario::validarContrasena( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaContrasena" ) ) ) );
		
		// Busca en la base de datos el usuario solicitado
		string consulta = "select * from Usuario where pseudonimo = '" + nombreUsuario + "'";
		database.query( consulta );
		database.close();

		// Si hay resultados (usuario encontrado)
		if( results.size() > 0 ){
			auto resultado = results.at( 0 );

			// Verifica la autenticidad de la contrasena
			verificarContrasena( contrasena, (* resultado)[ "contrasena" ] );
			
			// Establece los datos restantes
			usuario.establecerClave( stoi( (* resultado)[ "id_usuario" ] ) );
			usuario.establecerNombreUsuario( nombreUsuario );
			usuario.establecerNombre( (* resultado)[ "nombre" ] );
			usuario.establecerApellidos( (* resultado)[ "apellidos" ] );
			usuario.establecerHash( (* resultado)[ "contrasena" ] );
			usuario.establecerAdministrador( stoi( (* resultado)[ "administrador" ] ) );

			// Si hay una foto de perfil establecida, la cambia
			gtk_image_set_from_file( GTK_IMAGE( buscar_objeto( "ImagenUsuario" ) ), "../recursos/imagenes/iconos/Usuario64.png" );
			if( (* resultado)[ "imagen" ].compare( "null" ) != 0 ){
				gtk_image_set_from_pixbuf( GTK_IMAGE( buscar_objeto( "ImagenUsuario" ) ), imagen_cargar_escalar( "../recursos/imagenes/usuarios/" + (* resultado)[ "imagen" ], 60, 60 ) );
			}

			// Muestra al usuario
			gtk_label_set_text( GTK_LABEL( buscar_objeto( "NombreUsuario" ) ), ( usuario.obtenerNombre() + "\n" + usuario.obtenerApellidos() ).c_str() );
			gtk_widget_show( GTK_WIDGET( buscar_objeto( "Usuario" ) ) );
			
			// Redirige hacia la vista de inicio
			irA( "Inicio", true );
			
			// Conecta las señales
			if( usuario.esAdministrador() ){
				conectarSenalesAdministrador();
			}

			//
			conectar_senal( enlaceCuenta, G_CALLBACK( usuario_cuenta_leer ), nullptr );

			// Manda a conectar todas las señales de las vistas
        	conectarSenales();

			// Carga la información
			cargarInformacion();
			conectar_senal( enlaceSesionCerrar, G_CALLBACK( sesion_Cerrar ), nullptr );
			
		}
		else{
			app_mostrar_error( "El usuario " + nombreUsuario + " no está registrado." );
		}
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void sesion_Cerrar( GtkWidget *widget, gpointer ptr ){
	cout << "sesion_Cerrar" << endl;

	// Establece el usuario en cero
	usuario.establecerClave( 0 );

	// Oculta el usuario
	gtk_widget_hide( GTK_WIDGET( buscar_objeto( "Usuario" ) ) );

	// Desconecta la señal de cerrar sesion
	desconectar_senal( enlaceSesionCerrar );

	// Lo redirige hacia la aplicacion
	irA( "IniciarSesion", true );
}

string crearSal()
{
	// String que devolverá la sal
	stringstream sal;
	
	// Establece la semilla
	srand( time( 0 ) );
	
	for( size_t contador = 0; contador < 5; ++contador ){
		sal << ( rand() % 10 ) ;
	}
	
	// Devuelve el string con la sal
	return sal.str();
}

void verificarContrasena( string contrasena, string hash )
{
	// Realiza el hash para esa contrasena
	SHA256 hashTool;
	string inputHash = hashTool( contrasena );
	
	// Realiza la comparativa del hash creado con la del hash que se encuentra en la base de datos
	if( inputHash.compare( hash ) != 0 ){
		throw invalid_argument( "La contraseña introducida es incorrecta." );
	}
}

void cambiarContrasena( string usuario, string contrasena, string confirmacion )
{
	try{
		// Compara las contraseñas dadas
		Usuario::compararContrasenas( contrasena, confirmacion );

		// Crea la contrasena que se insertará dentro de la base de datos
		string contrasenaFinal = crearHash( contrasena );

		// Actualiza los datos de sal, de la nueva contraseña y elimina el código de recuperación
		database.open( databaseFile );
		database.query( "update Usuario set contrasena = '" + contrasenaFinal + "'" + " where pseudonimo = '" + usuario + "';"  );
		database.close();
	}
	catch( exception &ex ){
		throw ex;
	}
}

void cambiarContrasenaUsuario()
{
	try{
		if( codigoRecuperacion.empty() ){
			app_mostrar_error( "Ha ocurrido un error relacionado con el código de seguridad." );
			return;
		}

		// Obtiene los campos introducidos
		string usuario = Usuario::validarNombreUsuario( nombreUsuario );
		string contrasena = Usuario::validarContrasena( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaReemplazarContrasena" ) ) ) );
		string confirmacion = Usuario::validarContrasena( gtk_entry_get_text( GTK_ENTRY( buscar_objeto( "EntradaReemplazarConfirmacion" ) ) ) );

		// Cambia la contraseña del usuario especificado
		cambiarContrasena( usuario, contrasena, confirmacion );

		// Elimina el código de seguridad
		database.open( databaseFile );
		database.query( "delete from codigos_recuperacion where codigo = '" + codigoRecuperacion + "';" );
		database.close();

		// Limpia el nombre de usuario
		nombreUsuario.clear();
		codigoRecuperacion.clear();

		// Indica que la contraseña fue reiniciada exitosamente
		app_mostrar_mensaje( "Cambio de contraseña exitoso." );

		// Redirige al inicio de sesión
		irA( "IniciarSesion", true );
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

// Indica si se han registrado usuarios en el programa
bool existenUsuariosRegistrados(){
	cout << "usuario_existe" << endl;

	// Consulta los usuarios registrados cuyo nombre de usuario no sea 'admin'
	database.open( databaseFile );
	database.query( "select * from Usuario where pseudonimo != 'administrador'" );
	database.close();

	// Retorna si hubo resultados
	return results.size() > 0;
}


