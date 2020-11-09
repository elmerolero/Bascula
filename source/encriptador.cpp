#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// Encripta la cadena dado un número
string encriptar( string cadena, int numero )
{
	// Entrada
	string str = cadena;

	// Proceso
	for( char& c : str ){
		c = c + numero;
	}

	return str;
}

int main()
{
	string texto;
	string textoEncriptado;

	cout << "Introduce el texto a encriptar: " << endl;
	getline( cin, texto );

	textoEncriptado = encriptar( texto, 5 );

	// Archivo HTML con la información del ticket
	ofstream archivo;
	
	// Se abre el archivo
	archivo.open( "textoEncriptado.txt", ios_base::out );
	if( !archivo ){
		throw runtime_error( "Error" );
	}

	archivo << textoEncriptado << endl;

	archivo.close();

	return 0;
}
