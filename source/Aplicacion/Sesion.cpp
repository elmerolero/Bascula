#include "Sesion.h"
#include "GestorRegistros.h"
#include "sha256.h"
#include "Usuario.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include "Vistas.h"
#include "Funciones.h"
using namespace std;

string nombreUsuario;
string codigoRecuperacion;

void registrarUsuario(){
	cout << "usuario_registrar" << endl;
	try{
		// Obtiene los datos del formulario
		string nombre = Usuario::validarNombre( interfaz.obtenerTextoEntrada( "EntradaRegistroNombre" ) );
		string apellidos = Usuario::validarApellidos( interfaz.obtenerTextoEntrada( "EntradaRegistroApellidos" ) );
		string nombreUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaRegistroNombreUsuario" ) );
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaRegistroContrasena" ) );
		string confirmacion = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaRegistroConfirmarContrasena" ) );
		
		// Se asegura de que el usuario que se desea registrar no existe
		nombreUsuarioOcupado( nombreUsuario );
		
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

void actualizarDatosUsuario()
{
	try{
		// Lee la contraseña actual y cierra la ventana
		ocultar_elemento( "VentanaEntradaContrasena" ); 	
		string contrasenaActual = obtener_texto_entrada( "EntradaContrasenaAutorizacion" );
		establecer_texto_entrada( "EntradaContrasenaAutorizacion", "" );
		
		// Verifica la autenticidad de la contrasena
		verificarContrasena( contrasenaActual, usuario.obtenerHash() );
		
		// Obtiene los datos del formulario
		string nombre = Usuario::validarNombre( obtener_texto_entrada( "EntradaCuentaNombre" ) );
		string apellidos = Usuario::validarApellidos( obtener_texto_entrada( "EntradaCuentaApellidos" ) );
		string nombreUsuario = Usuario::validarNombreUsuario( obtener_texto_entrada( "EntradaCuentaNombreUsuario" ) );
		string contrasena = obtener_texto_entrada( "EntradaCuentaContrasenaNueva" );
		string confirmacion = obtener_texto_entrada( "EntradaCuentaContrasenaConfirmacion" );
		
		// ¿Se introdujo un nombre de usuario diferente?
		if( nombreUsuario.compare( usuario.obtenerNombreUsuario() ) != 0 ){
			nombreUsuarioOcupado( nombreUsuario );
		}
		
		// ¿Se introdujo una nueva contraseña?
		if( !contrasena.empty() ){
			// Se asegura que las contraseñas introducidas sean válidas
			contrasena = Usuario::validarContrasena( contrasena );
			confirmacion = Usuario::validarContrasena( confirmacion );
			
			// Se asegura que las contraseñas introducidas sean iguales
			cambiarContrasena( usuario.obtenerNombreUsuario(), contrasena, confirmacion );
		}
		
		// Actualiza la información de usuario en el usuario
		usuario.establecerNombre( nombre );
		usuario.establecerApellidos( apellidos );
		usuario.establecerNombreUsuario( nombreUsuario );

		// Actualiza la información en la base de datos
		database.open( databaseFile );
		database.query( "update Usuario set nombre = '" + usuario.obtenerNombre() + "', apellidos = '" + usuario.obtenerApellidos() + "', pseudonimo = '" + usuario.obtenerNombreUsuario() + "' where pseudonimo = '" + usuario.obtenerNombreUsuario() + "'" );
		database.close();
		
		// Actuliza los datos de la interfaz
		interfaz.establecerTextoEtiqueta( "NombreUsuario", usuario.obtenerNombre() + "\n" + usuario.obtenerApellidos() );
		
		// Muestra que el registro fue exitoso
		app_mostrar_mensaje( "Datos actualizados de forma exitosa." );
		usuario_cuenta_leer( nullptr, nullptr );
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
		string nombreUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaUsuario" ) );
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaContrasena" ) );
		
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

			// Muestra al usuario
			mostrarUsuario();
			
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
		}
		else{
			app_mostrar_error( "El usuario " + nombreUsuario + " no está registrado." );
		}
	}
	catch( invalid_argument &ia ){
		app_mostrar_error( ia.what() );
	}
}

void mostrarUsuario()
{
	interfaz.establecerTextoEtiqueta( "NombreUsuario", usuario.obtenerNombre() + "\n" + usuario.obtenerApellidos() );
	interfaz.mostrarElemento( "Usuario" );
}

void nombreUsuarioOcupado( std::string nombreUsuario )
{
	// Abre la base de datos
	database.open( databaseFile );
	
	string consulta = "select * from Usuario where pseudonimo = '" + nombreUsuario + "'";
	database.query( consulta );
	if( results.size() > 0 ){
		throw invalid_argument( "El nombre de usuario que deseas registrar ya está en uso." );
	}	
	
	// Cierra la conexión
	database.close();
}

void autorizarCambios()
{
	// Solicita la contraseña para autorizar cambios
	interfaz.mostrarElemento( "VentanaEntradaContrasena" );
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
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaReemplazarContrasena" ) );
		string confirmacion = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaReemplazarConfirmacion" ) );

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


