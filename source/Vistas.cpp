#include "Senales.h"
#include "Globales.h"
#include <iostream>
using namespace std;

//Establece la vista para registrar un usuario
void vistaRegistrarUsuario( GtkWidget *widget, gpointer ptr )
{
	aplicacion.mostrarVista( "RegistrarUsuario" );
}

//Establece la vista para iniciar sesion
void vistaIniciarSesion( GtkWidget *widget, gpointer ptr )
{
	aplicacion.mostrarVista( "IniciarSesion" );
}

// Función de prueba, imprime un hola
void hola( GtkWidget *widget, gpointer ptr )
{
	cout << "Hola" << endl;
}
