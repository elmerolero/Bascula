#ifndef TICKET_PUBLICO_H
#define TICKET_PUBLICO_H

#include <string>
#include "Registro.h"

const int VIAJE_LOCAL = 0;
const int VIAJE_FORANEO = 1;

class TicketPublico
{
	public:
		// Constructor
		TicketPublico();
		
		// Folio
		void establecerFolio( unsigned int folio );
		unsigned int obtenerFolio() const;
		
		// Fecha de registro
		void establecerFecha( std::string fechaRegistro );
		std::string obtenerFecha() const;
		
		// Numero de placas
		void establecerNumeroPlacas( std::string numeroPlacas );
		std::string obtenerNumeroPlacas() const;
		
		// Nombre del conductor
		void establecerNombreConductor( std::string nombreConductor );
		std::string obtenerNombreConductor() const;
		
		// Producto que registra
		void establecerProducto( Registro *producto );
		Registro *obtenerProducto() const;
		
		// Tipo viaje
		void establecerTipoViaje( int tipoViaje );
		int obtenerTipoViaje() const;
		
		// Hora de entrada
		void establecerHoraEntrada( std::string horaEntrada );
		std::string obtenerHoraEntrada() const;
		
		// Hora de salida
		void establecerHoraSalida( std::string horaSalida );
		std::string obtenerHoraSalida() const;
		
		// Peso bruto
		void establecerPesoBruto( std::string pesoBrutoStr );
		void establecerPesoBruto( double pesoBruto );
		double obtenerPesoBruto() const;
		
		// Peso tara
		void establecerPesoTara( std::string pesoTara );
		void establecerPesoTara( double pesoTara );
		double obtenerPesoTara() const;
		
		// Peso neto
		void establecerPesoNeto( std::string pesoNetoStr );
		void establecerPesoNeto( double pesoNeto );
		double obtenerPesoNeto() const;
		
		// Bandera que indica que el peso bruto fue establecido
		void establecerPesoBrutoEstablecido( bool pesoBrutoEstablecido );
		bool estaPesoBrutoEstablecido() const;
		
		// Bandera que indica que el peso tara fue establecido
		void establecerPesoTaraEstablecido( bool pesoTaraEstablecido );
		bool estaPesoTaraEstablecido() const;
		
		// Bandera que indica que el peso neto fue establecido
		void establecerPesoNetoEstablecido( bool pesoNetoEstablecido );
		bool estaPesoNetoEstablecido() const;
		
		// Establece el nombre del basculista
		void establecerNombreBasculista( std::string nombreBasculista );
		std::string obtenerNombreBasculista() const;
		
		// Establece si el ticket de báscula publica esta pendiente
		void establecerPendiente( bool pendiente );
		bool estaPendiente() const;
		
		// Establece si alguno de los pesos fue registrado de manera manual
		void establecerEntradaManual( bool entradaManual );
		bool esEntradaManual() const;
	
		void imprimir( std::string nombreEmpresa ) const;
	private:
		unsigned int folio;
		std::string fechaRegistro;
		std::string numeroPlacas;
		std::string nombreConductor;
		Registro *producto;
		int tipoViaje;
		std::string horaEntrada;
		std::string horaSalida;
		double pesoBruto;
		double pesoTara;
		double pesoNeto;
		bool pesoBrutoEstablecido;
		bool pesoTaraEstablecido;
		bool pesoNetoEstablecido;
		std::string nombreBasculista;
		bool pendiente;
		bool entradaManual;	
};

#endif
