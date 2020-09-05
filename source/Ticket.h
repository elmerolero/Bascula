#ifndef TICKET_H
#define TICKET_H
#include <string>
#include "Registro.h"

const int VIAJE_INTERNO = 0;
const int VIAJE_LOCAL = 1;
const int VIAJE_FORANEO = 2;

class Ticket
{
	public:
		// Folio
		void establecerFolio( int folio );
		int obtenerFolio() const;
		
		// Fecha de registro
		void establecerFechaRegistro( std::string fechaRegistro );
		std::string obtenerFechaRegistro() const;
		
		// Empresa
		void establecerEmpresa( Registro *empresa );
		Registro *obtenerEmpresa() const;
		
		// Producto
		void establecerProducto( Registro *producto );
		Registro *obtenerProducto() const;
		
		// Numero de placas
		void establecerNumeroPlacas( std::string numeroPlacas );
		std::string obtenerNumeroPlacas() const;
		
		// Nombre del conductor
		void establecerNombreConductor( std::string nombreConductor );
		std::string obtenerNombreConductor() const;
		
		// Tipo de viaje
		void establecerTipoViaje( int claveTipo );
		int obtenerTipoViaje() const;
		
		// Peso bruto
		void establecerPesoBruto( double pesoBruto );
		double obtenerPesoBruto() const;
		
		// Peso tara
		void establecerPesoTara( double pesoTara );
		double obtenerPesoTara() const;
		
		// Peso neto
		void calcularPesoNeto();
		void establecerPesoNeto( double pesoNeto );
		double obtenerPesoNeto() const;

		// Permite  establecer la hora de entrada
		void establecerHoraEntrada( std::string horaEntrada );
		std::string obtenerHoraEntrada() const;

		// Permite establecer la hora de salida
		void establecerHoraSalida( std::string horaSalida );
		std::string obtenerHoraSalida() const;
		
		// Permite establecer observaciones
		void establecerObservaciones( std::string observaciones );
		std::string obtenerObservaciones() const;
		
		// Estado del ticket pendiente (verdadero) o finalizado (falso)
		void establecerPendiente( bool pendiente );
		bool estaPendiente() const;
		
		// Imprime los datos del ticket
		void imprimir() const;
	
	private:
		int folio;
		std::string fechaRegistro;
		Registro *empresa;
		Registro *producto;
		std::string numeroPlacas;
		std::string nombreConductor;
		int tipoViaje;
		std::string horaEntrada;
		std::string horaSalida;
		double pesoBruto;
		double pesoTara;
		double pesoNeto;
		std::string observaciones;
		bool pendiente;
};

#endif
