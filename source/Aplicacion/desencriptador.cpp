#include <iostream>
#include <string>
using namespace std;

// Desencripta la cadena dado un número
string desencriptar( string cadena, int numero )
{
	// Entrada
	string str = cadena;

	// Proceso
	for( char& c : str ){
		c = c - numero;
	}

	return str;
}

int main()
{
	string texto;
	string textoDesencriptado;

	cout << "Introduce el texto a desencriptar: " << endl;
	getline( cin, texto );

	textoDesencriptado = desencriptar( texto, 5 );

	cout << "El texto desencriptado es: " << endl;
	cout << textoDesencriptado << endl;

	return 0;
}
