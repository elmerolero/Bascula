#include "Registro.h"
#include <stdexcept>
#include <regex>
#include <string>
using namespace std;

// Constructor
Registro::Registro():
	clave( 0 )
{

}

// Destructor
Registro::~Registro()
{
	clave = 0;
	nombre.clear();
}

// Clave
void Registro::establecerClave( unsigned int clave )
{
	this -> clave = clave;
}

unsigned int Registro::obtenerClave() const
{
	return clave;
}

// Nombre
void Registro::establecerNombre( string nombre )
{
	regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ.\\s]*" );
	if( nombre.empty() ){
		throw  invalid_argument( "Error, no puede establecerse un nombre vacío a un registro." );
	}
	
	if( nombre.size() > 50 || !regex_match( nombre, formato ) ){
		throw invalid_argument( "Error, no el nombre no puede contener más de cuarenta caracteres ni símbolos especiales." );
	}
	
	this -> nombre = nombre;
}

string Registro::obtenerNombre() const
{
	return nombre;
}
