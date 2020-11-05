#include "Usuario.h"
#include <string>
#include <stdexcept>
#include <regex>
using namespace std;

// Constructor
Usuario::Usuario():
    administrador( false )
{
    // Nada que hacer
}

// Establece el nombre del usuario
void Usuario::establecerNombreUsuario( string nombreUsuario )
{
    regex formato( "[a-zA-Z0-9_-]*" );
    
    // Se asegura que no se introduzca un nombre de usuario vacío
    if( nombreUsuario.empty() ){
        throw invalid_argument( "Debe introducirse un nombre de usuario." );
    }
    
    // Se asegura de que el tamaño del nombre de usuario sea de almenos 6 caracteres y menor a 20
    if( nombreUsuario.size() < 4 || nombreUsuario.size() > 20 ){
        throw invalid_argument( "El numero de caracteres del usuario introducido se encuentra fuera del rango permitido." );
    }
    
    // Se asegura que no se introduzcan caracteres inválidos en el nombre de usuario
    if( !regex_match( nombreUsuario, formato ) ){
        throw invalid_argument( "El nombre introducido no es válido." );
    }
    
    this -> nombreUsuario = nombreUsuario;
}

// Obtiene el nombre del usuario
string Usuario::obtenerNombreUsuario() const
{
    return nombreUsuario;
}

// Establece el nombre real del usuario
void Usuario::establecerNombre( string nombre )
{
    // Formato de validación del nombre introducido
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
    // Se asegura que no se introduzca un nombre vacío
    if( nombre.empty() ){
        throw invalid_argument( "Es nesesario introducir un nombre." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( nombre.size() > 35 || !regex_match( nombre, formato ) ){
        throw invalid_argument( "El nombre introducido no es válido." );
    } 
    
    this -> nombre = nombre;
}

// Obtiene el nombre real del usuario
string Usuario::obtenerNombre() const
{
    return nombre;
}

// Establece los apellidos del usuario
void Usuario::establecerApellidos( string apellidos )
{
    // Formato de validación del nombre introducido
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
    // Se asegura que no se introduzca un nombre vacío
    if( apellidos.empty() ){
        throw invalid_argument( "Es nesesario introducir un apellido." );
    }
    
    // Se asegura de que el nombre introducido no supere el tamaño 
    if( apellidos.size() > 35 || !regex_match( apellidos, formato ) ){
        throw invalid_argument( "El apellido introducido no es válido." );
    } 

    this -> apellidos = apellidos;
}

// Obtiene los apellidos del usuario
string Usuario::obtenerApellidos() const
{
    return apellidos;
}

// Establece el hash del usuario a registrar
void Usuario::establecerHash( string hash )
{
    this -> hash = hash;
}

// Obtiene el hash del usuario a registrar
string Usuario::obtenerHash() const
{
    return hash;
}

// Establece la sal
void Usuario::establecerSal( string sal )
{
    this -> sal = sal;
}

// Obtiene la sal
std::string Usuario::obtenerSal() const
{
    return sal;
}

// Establece los permisos de administrador
void Usuario::establecerAdministrador( bool permiso )
{
    this -> administrador = permiso;
}

// Revisa si el usuario tiene permisos de administrador
bool Usuario::esAdministrador() const
{
    return administrador;
}

