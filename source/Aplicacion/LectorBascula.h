#ifndef LECTOR_BASCULA
#define LECTOR_BASCULA
#include <array>
#include <gtk/gtk.h>
#include <Windows.h>
#include "Widget.h"

class LectorBascula
{
	public:
		LectorBascula();

		void abrir( Widget &interfaz );
		std::string leer();
		void actualizarOpciones();
		void iniciarOpcion();
		void cerrar();
		
		void cargarConfiguracion();

		void establecerIdSenal( guint senal );
		
		void establecerLecturaManual( bool lecturaManual );
		bool lecturaManualActivada() const;

		void establecerPeso( double peso );
		double obtenerPeso() const;
		
		void establecerActivo( bool activo );
		bool estaActivo() const;

		void establecerPuerto( std::string puerto );
		std::string obtenerPuerto() const;

		void establecerBytesIgnorados( std::string bytesIgnoradosStr );
		void establecerBytesIgnorados( unsigned int bytesIgnorados );
		unsigned int obtenerBytesIgnorados() const;
	
		void establecerBaudRate( std::string baudRateStr );
		void establecerBaudRate( DWORD baudRate );
		DWORD obtenerBaudRate() const;

		void establecerByteSize( std::string byteSizeStr );
		void establecerByteSize( BYTE byteSize );
		BYTE obtenerByteSize() const;

		void establecerStopBits( std::string stopBitsStr );
		void establecerStopBits( BYTE stopBits );
		BYTE obtenerStopBits() const;

		void establecerParity( std::string parity );
		void establecerParity( BYTE parity );
		BYTE obtenerParity() const;

	private:
		Widget interfaz; 
		bool activado;					// Indica si la báscula se encuentra activa o no
		bool lecturaManual;				// Indica si la lectura del peso se hizo de manera manual
		guint botonLectorBasculaId;		// Identificador del boton de lectura
		guint comboBoxLectorBasculaId;  // Indentificador del comboBoxText con las opciones que puede manejar la báscula
		double peso;					// Peso leído por la báscula

		std::string puerto;				// Puerto seleccionado
		unsigned int bytesIgnorados; 	// Indica el número de bytes a ignorar
		DWORD baudRate;					// Velocidad de conexión
		BYTE byteSize;					// Tamaño en bits
		BYTE stopBits;					// Bits de stop
		BYTE parity;					// Paridad
};

/* Eventos que permitan llamar a la entidad global báscula */
extern LectorBascula lectorBascula;
extern HANDLE dispositivo;
const std::array< std::string, 5 > parityOptions { "None", "Impar", "Par", "Marca", "Espacio" };

void lectorBasculaActualizarOpciones();
void lectorBasculaIniciarOpcion( GtkWidget *widget, gpointer ptr );
void lectorBasculaCerrar( GtkWidget *widget, gpointer ptr );
void lectorBasculaActualizarPeso();
void lectorBasculaConectar();
void lectorBasculaObtenerPeso();

#endif
