#include "Usuario.h"
#include <string.h>
using namespace std;

// Constructor
Usuario::Usuario():
    nombreUsuario( 0 ), nombre( 0 ), apellidos( 0 ), administrador( false )
{
    // Nada que hacer
}

// Establece el nombre del usuario
void Usuario::establecerNombreUsuario( string nombreUsuario )
{
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
    this -> apellidos = apellidos;
}

// Obtiene los apellidos del usuario
string Usuario::obtenerApellidos() const
{
    return apellidos;
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

