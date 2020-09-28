#ifndef LECTOR_BASCULA
#define LECTOR_BASCULA
#include <gtk/gtk.h>
#include "Widget.h"

class LectorBascula
{
	public:
		void abrir( Widget &interfaz );
		const char *leer();
		void actualizarOpciones();
		void iniciarOpcion();
		void cerrar();
		
		void establecerIdSenal( guint senal );
		
		void establecerLecturaManual( bool lecturaManual );
		bool lecturaManualActivada() const;
		
		void establecerActivo( bool activo );
		bool estaActivo() const;
	
	private:
		Widget interfaz; 
		bool activado;					// Indica si la báscula se encuentra activa o no
		bool lecturaManual;				// Indica si la lectura del peso se hizo de manera manual
		guint botonLectorBasculaId;		// Identificador del boton de lectura
		guint comboBoxLectorBasculaId;  // Indentificador del comboBoxText con las opciones que puede manejar la báscula
};

/* Eventos que permitan llamar a la entidad global báscula */
extern LectorBascula lectorBascula;

void lectorBasculaIniciarOpcion( GtkWidget *widget, gpointer ptr );
void lectorBasculaCerrar( GtkWidget *widget, gpointer ptr );

#endif
