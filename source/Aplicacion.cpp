#include <gtk/gtk.h>
#include <iostream>
#include <stdexcept>
#include "Aplicacion.h"
#include "Widget.h"
using namespace std;

Aplicacion::Aplicacion():
    ventana( nullptr )
{
    // Nada por hacer
}

int Aplicacion::iniciar()
{
    try{
        // Carga la ventana principal y conecta la función para cerrar la ventana y finalizar el programa
        ventana = new Widget( "../resources/interfaces/RegistrosEmpresas.glade" );
        ventana -> conectarSenal( "VentanaPrincipal", "destroy", G_CALLBACK( gtk_main_quit ), nullptr );
    }
    catch( runtime_error &excepcion ){
        cerr << excepcion.what() << endl;
        return 1;
    }

    return 0;
}

Widget* Aplicacion::obtenerWidget() const
{
	return ventana;
}

void establecerUsuario( GtkWidget *widget, gpointer ptr )
{
    try{
        static Widget * widget = new Widget( "../resources/interfaces/Usuario.glade" );
    
        // Añade el elemento
        static_cast< Widget * >( ptr ) -> insertarElementoAGrid( widget, "Usuario", "Tiempo", 2, 0, 1, 1 ); 
    }
    catch( runtime_error &e ){
        cerr << e.what() << endl;
    }
}

