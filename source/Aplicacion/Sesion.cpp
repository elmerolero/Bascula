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
using namespace std;

string nombreUsuario;
string codigoRecuperacion;

void registrarUsuario()
{
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
		string sal = crearSal();
		string contrasenaFinal = crearHash( contrasena + sal );
		
		string administrador = existenUsuariosRegistrados() ? "0" : "1";

		// Registra al usuario en la base de datos
		database.open( databaseFile );
		database.query( "insert into usuarios values( '" + nombreUsuario + "', '" +  contrasenaFinal + "', '" + sal + "', '" + nombre + "', '" + apellidos + "', " + administrador + " )" );
		database.close();
		
		// Muestra un mensaje de que efectivamente se registro al usuario y lo redirige a la página de inicio de sesion
		mostrarMensaje( "¡Bienvenido!" );
		irA( "IniciarSesion", true );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() ); 
	}
}

void actualizarDatosUsuario()
{
	try{
		// Lee la contraseña actual y cierra la ventana
		interfaz.ocultarElemento( "VentanaEntradaContrasena" );
		string contrasenaActual = interfaz.obtenerTextoEntrada( "EntradaContrasenaAutorizacion" );
		interfaz.establecerTextoEntrada( "EntradaContrasenaAutorizacion", "" );
		
		// Verifica la autenticidad de la contrasena
		verificarContrasena( contrasenaActual, usuario.obtenerSal(), usuario.obtenerHash() );
		
		// Obtiene los datos del formulario
		string nombre = Usuario::validarNombre( interfaz.obtenerTextoEntrada( "EntradaCuentaNombre" ) );
		string apellidos = Usuario::validarApellidos( interfaz.obtenerTextoEntrada( "EntradaCuentaApellidos" ) );
		string nombreUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaCuentaNombreUsuario" ) );
		string contrasena = interfaz.obtenerTextoEntrada( "EntradaCuentaContrasenaNueva" );
		string confirmacion = interfaz.obtenerTextoEntrada( "EntradaCuentaContrasenaConfirmacion" );
		
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
		database.query( "update usuarios set nombre = '" + usuario.obtenerNombre() + "', apellidos = '" + usuario.obtenerApellidos() + "', nombre_usuario = '" + usuario.obtenerNombreUsuario() + "' where nombre_usuario = '" + usuario.obtenerNombreUsuario() + "'" );
		database.close();
		
		// Actuliza los datos de la interfaz
		interfaz.establecerTextoEtiqueta( "NombreUsuario", usuario.obtenerNombre() + "\n" + usuario.obtenerApellidos() );
		
		// Muestra que el registro fue exitoso
		mostrarMensaje( "Datos actualizados de forma exitosa." );
		vistaCuenta( nullptr, nullptr );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
}

void iniciarSesion()
{
	// Conecta con la base de datos
	database.open( databaseFile );
		
	try{
		// Establece el nombre de usuario
		string nombreUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaUsuario" ) );
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaContrasena" ) );
		
		// Busca en la base de datos el usuario solicitado
		string consulta = "select * from usuarios where nombre_usuario = '" + nombreUsuario + "'";
		database.query( consulta );
		
		// Si hay resultados (usuario encontrado)
		if( results.size() > 0 ){
			// Obtiene la contraseña y la sal
			string hash = (* results.at( 0 ))[ "contrasena" ];
			string sal = (* results.at( 0 ))[ "sal" ];
			string nombre = (* results.at( 0 ))[ "nombre" ];
			string apellidos = (* results.at( 0 ))[ "apellidos" ];
			bool administrador = stoi( (* results.at( 0 ))[ "administrador" ] );
			
			// Verifica la autenticidad de la contrasena
			verificarContrasena( contrasena, sal, hash );
			
			// Establece los datos restantes
			usuario.establecerNombreUsuario( nombreUsuario );
			usuario.establecerNombre( nombre );
			usuario.establecerApellidos( apellidos );
			usuario.establecerHash( hash );
			usuario.establecerSal( sal );
			usuario.establecerAdministrador( administrador );
			
			// Muestra al usuario
			mostrarUsuario();
			
			// Redirige hacia la vista de inicio
			irA( "Inicio", true );
			
			// Carga la información
			cargarInformacion();
			
			// Conecta las señales
			if( usuario.esAdministrador() ){
				conectarSenalesAdministrador();
			}

			// Manda a conectar todas las señales de las vistas
        	conectarSenales();
		}
		else{
			mostrarMensajeError( "El usuario " + nombreUsuario + " no está registrado." );
		}
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
	
	// Cierra la conexión
	database.close();
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
	
	string consulta = "select * from usuarios where nombre_usuario = '" + nombreUsuario + "'";
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

void verificarContrasena( string contrasena, string sal, string hash )
{
	// Realiza el hash para esa contrasena
	SHA256 hashTool;
	string inputHash = hashTool( contrasena + sal );
	
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
		string sal = crearSal();
		string contrasenaFinal = crearHash( contrasena + sal );

		// Actualiza los datos de sal, de la nueva contraseña y elimina el código de recuperación
		database.open( databaseFile );
		database.query( "update usuarios set contrasena = '" + contrasenaFinal + "', sal = '" + sal + "' where nombre_usuario = '" + usuario + "';"  );
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
			mostrarMensajeError( "Ha ocurrido un error relacionado con el código de seguridad." );
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
		mostrarMensaje( "Cambio de contraseña exitoso." );

		// Redirige al inicio de sesión
		irA( "IniciarSesion", true );
	}
	catch( invalid_argument &ia ){
		mostrarMensajeError( ia.what() );
	}
}

// Indica si se han registrado usuarios en el programa
bool existenUsuariosRegistrados()
{
	// Consulta los usuarios registrados cuyo nombre de usuario no sea 'admin'
	database.open( databaseFile );
	database.query( "select * from usuarios where nombre_usuario != 'admin'" );
	database.close();

	// Retorna si hubo resultados
	return results.size() > 0;
}

string crearHash( std::string contrasena )
{
	SHA256 hashTool;
	return hashTool( contrasena );
}
