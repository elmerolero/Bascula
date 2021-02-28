#ifndef CONTENEDOR_REGISTROS_H
#define CONTENEDOR_REGISTROS_H

#include <string>
#include <list>
#include <gtk/gtk.h>
#include "Database.h"
#include "Registro.h"
#include "Widget.h"

class ContenedorRegistros
{
	public:
		void establecerNombrePlural( std::string nombrePlural );
		std::string obtenerNombrePlural() const;
		
		void establecerNombreSingular( std::string nombreSingular );
		std::string obtenerNombreSingular() const;
		
		void obtenerRegistros();
		
		Registro *agregarNuevoRegistro( std::string nombre );
		void actualizarRegistro( Registro *registro );
		void eliminarRegistro( Registro *registro );
		
		Registro *buscarRegistroPorClave( unsigned int clave );
		Registro *buscarRegistroPorNombre( std::string nombre );
		
		void actualizarListaRegistros();
		GtkEntryCompletion *obtenerCompletador() const;
	
	private:
		std::string nombrePlural;				// Nombre plural (ejemplo 'registros')
		std::string nombreSingular;				// Nombre singular (ejemplo 'registro' )
		unsigned int claveActual;				// Clave actual en la que van los registros
		std::list< Registro * >registros; 		// Lista con los registros dados
		GtkListStore *listaNombresRegistros;	// Lista con los nombres de los registros para autocompletado
		GtkEntryCompletion * completador;		// Objeto de completado para los GkEntry
};

#endif
