#include "Registro.h"
#include <stdexcept>
#include <regex>
#include <iostream>
#include <string>
using namespace std;

// Constructor
Registro::Registro():
	clave( 0 )
{

}

// Constructor a partir de un resulado de la base de datos
Registro::Registro( unordered_map< string, string > *renglon, string tipo )
{
	// Verifica que el renglón que se desea asignar no sea un renglón vacío
	if( renglon == nullptr ){
		throw invalid_argument( "Intento de asignación de un renglón vacío." );
	}

	// Establece los campos necesarios
	establecerClave( stoi( (* renglon)[ ( "clave_" + tipo ) ] ) );
	establecerNombre( (* renglon)[ "nombre_" + tipo ] );
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
