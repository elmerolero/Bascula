#include "Sesion.h"
#include "GestorRegistros.h"
#include "sha256.h"
#include "Usuario.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Vistas.h"
using namespace std;

string nombreUsuario;
string codigoRecuperacion;

void registrarUsuario()
{
	// Conecta con la base de datos
	database.open( nombreArchivo );

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
		compararContrasenas( contrasena, confirmacion );
		
		// Crea la contrasena que se insertará dentro de la base de datos
		string sal = crearSal();
		string contrasenaFinal = crearHash( contrasena, sal );
		
		// Conecta con la base de datos
		database.open( nombreArchivo );
		
		// Intenta realizar el registro
		string consulta = "insert into usuarios values( '" + nombreUsuario + "', '" +  contrasenaFinal + "', '" + sal + "', '" + nombre + "', '" + apellidos + "', " + ( esInicio ? "1" : "0" ) + " )";
		database.query( consulta );
		
		// Cierra la base de datos
		database.close();
		
		// Muestra un mensaje de que efectivamente se registro al usuario y lo redirige a la página de inicio de sesion
		mostrarMensaje( "Usuario registrado correctamente." );
		irHacia( nullptr, (void *)"IniciarSesion" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorRegistro", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorRegistro" );
	}

	// Desconecta de la base de datos
	database.close();
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
		string sal;
		string contrasenaFinal;
		
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
			compararContrasenas( contrasena, confirmacion );
			
			// Crea la contrasena que se insertará dentro de la base de datos
			sal = crearSal();
			contrasenaFinal = crearHash( contrasena, sal );
		}
		else{
			contrasenaFinal = usuario.obtenerHash();
			sal = usuario.obtenerSal();
		}
		
		// Actualiza la información en la base de datos
		database.open( nombreArchivo );
		string consulta = "update usuarios set nombre = '" + nombre + "', apellidos = '" + apellidos + "', nombre_usuario = '" + nombreUsuario + "', contrasena = '" + contrasenaFinal + "', sal = '" + sal + "' where nombre_usuario = '" + usuario.obtenerNombreUsuario() + "';";
		database.query( consulta );
		database.close();
		
		// Actualiza la información de usuario en el usuario
		usuario.establecerNombre( nombre );
		usuario.establecerApellidos( apellidos );
		usuario.establecerNombreUsuario( nombreUsuario );
		usuario.establecerHash( contrasenaFinal );
		usuario.establecerSal( sal );
		
		// Actuliza los datos de la interfaz
		interfaz.establecerTextoEtiqueta( "NombreUsuario", usuario.obtenerNombre() + "\n" + usuario.obtenerApellidos() );
		
		// Muestra que el registro fue exitoso
		mostrarMensaje( "Datos actualizados de forma exitosa." );
		interfaz.ocultarElemento( "MensajeErrorCuenta" );
		vistaCuenta( nullptr, nullptr );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorCuenta", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorCuenta" );
	}
}

void iniciarSesion()
{
	// Conecta con la base de datos
	database.open( nombreArchivo );
		
	try{
		// Establece el nombre de usuario
		string nombreUsuario = Usuario::validarNombreUsuario( interfaz.obtenerTextoEntrada( "EntradaUsuario" ) );
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaContrasena" ) );
		
		// Busca en la base de datos el usuario solicitado
		string consulta = "select * from usuarios where nombre_usuario = '" + nombreUsuario + "'";
		database.query( consulta );
		
		// Si hay resultados (usuario encontrado)
		if( rows.size() > 0 ){
			// Obtiene la contraseña y la sal
			string hash = rows.at( 0 ) -> columns.at( 1 );
			string sal = rows.at( 0 ) -> columns.at( 2 );
			string nombre = rows.at( 0 ) -> columns.at( 3 );
			string apellidos = rows.at( 0 ) -> columns.at( 4 );
			bool administrador = stoi( rows.at( 0 ) -> columns.at( 5 ) );
			
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
			irHacia( nullptr, (void *)"Inicio" );
			
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
			interfaz.establecerTextoEtiqueta( "MensajeErrorIniciarSesion", "El usuario " + nombreUsuario + " no está registrado." );
			interfaz.mostrarElemento( "MensajeErrorIniciarSesion" );
		}
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorIniciarSesion", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorIniciarSesion" );
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
	database.open( nombreArchivo );
	
	string consulta = "select * from usuarios where nombre_usuario = '" + nombreUsuario + "'";
	database.query( consulta );
	if( rows.size() > 0 ){
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

string crearHash( std::string contrasena, std::string sal )
{
	SHA256 hashTool;
	
	return hashTool( contrasena + sal );
}

void compararContrasenas( const string &contrasena, const string &contrasena2 )
{
	// Realiza la comparativa de las contraseñas y lanza una excepción si no son iguales
	if( contrasena.compare( contrasena2 ) != 0 ){
		throw invalid_argument( "Los códigos introducidos no coinciden." );
	}
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

void cambiarContrasenaUsuario()
{
	try{
		// Obtiene los campos introducidos
		string contrasena = Usuario::validarContrasena( interfaz.obtenerTextoEntrada( "EntradaReemplazarContrasena" ) );
		string confirmacion = interfaz.obtenerTextoEntrada( "EntradaReemplazarConfirmacion" );

		// Compara las contraseñas
		compararContrasenas( contrasena, confirmacion );
	
		// Crea la contrasena que se insertará dentro de la base de datos
		string sal = crearSal();
		string contrasenaFinal = crearHash( contrasena, sal );

		// Actualiza los datos de sal, de la nueva contraseña y elimina el código de recuperación
		database.open( nombreArchivo );
		database.query( "update usuarios set contrasena = '" + contrasenaFinal + "', sal = '" + sal + "' where nombre_usuario = '" + nombreUsuario + "';"  );
		database.query( "delete from codigos_recuperacion where codigo = '" + codigoRecuperacion + "';");
		database.close();

		// Limpia el nombre de usuario
		nombreUsuario.clear();
		codigoRecuperacion.clear();

		// Indica que la contraseña fue reiniciada exitosamente
		mostrarMensaje( "Cambio de contraseña exitoso." );

		// Redirige al inicio de sesión
		irHacia( nullptr, (void *)"IniciarSesion" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorReemplazarContrasena", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorReemplazarContrasena" );
	}
}

void consultarExistenciaUsuarios()
{
	try{
		// Abre la base de datos
		database.open( nombreArchivo );

		// Consulta todos los usuarios registrados
		database.query( "select * from usuarios where nombre_usuario != 'admin'" );
		if( rows.size() <= 0 ){
			irHacia( nullptr, (void *)"RegistrarUsuario" );
		}

		// Cierra la conexión
		database.close();
	}
	catch( exception &e ){
		cerr << e.what() << endl;
	}
}
