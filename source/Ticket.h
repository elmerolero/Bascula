#ifndef TICKET_H
#define TICKET_H
#include <string>
#include "Registro.h"

const int TIPO_REGISTRO_ENTRADA = 0;
const int TIPO_REGISTRO_SALIDA = 1;

class Ticket
{
	public:
		// Constructor
		Ticket();
		
		// Folio
		void establecerFolio( unsigned int folio );
		unsigned int obtenerFolio() const;
		
		// Fecha de registro
		void establecerFechaRegistro( std::string fechaRegistro );
		std::string obtenerFechaRegistro() const;
		
		// Tipo de registro
		void establecerTipoRegistro( int tipoRegistro );
		int obtenerTipoRegistro() const;
		
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
		
		// Peso bruto
		void establecerPesoBruto( std::string pesoBrutoStr );
		void establecerPesoBruto( double pesoBruto );
		double obtenerPesoBruto() const;
		void establecerPesoBrutoEstablecido( bool pesoBrutoEstablecido );
		bool estaPesoBrutoEstablecido() const;
		
		// Peso tara
		void establecerPesoTara( std::string pesoTara );
		void establecerPesoTara( double pesoTara );
		double obtenerPesoTara() const;
		void establecerPesoTaraEstablecido( bool pesoTaraEstablecido );
		bool estaPesoTaraEstablecido() const;
		
		// Bandera que indica si permite descuento o no
		void permitirDescuento( bool opcionDescuento );
		bool permitirDescuento() const;
		
		// Descuento
		void establecerDescuento( std::string descuentoStr );
		void establecerDescuento( double descuento );
		double obtenerDescuento() const;
		void establecerDescuentoEstablecido( bool descuentoEstablecido );
		bool estaDescuentoEstablecido() const;
		
		// Peso neto
		void calcularPesoNeto();
		void establecerPesoNeto( double pesoNeto );
		double obtenerPesoNeto() const;
		void establecerPesoNetoCalculado( bool pesoNetoEstablecido );
		bool estaPesoNetoCalculado() const;

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
		
		// Establece si el peso se introdujo de manera manual o automática
		void establecerEntradaManual( bool entradaManual );
		bool esEntradaManual() const;
		
		// Establece el nombre del basculista
		void establecerNombreBasculista( std::string nombre );
		std::string obtenerNombreBasculista() const;
		
		// Imprime los datos del ticket
		void imprimir( std::string nombreEmpresa ) const;
	
	private:
		unsigned int folio;
		std::string fechaRegistro;
		Registro *empresa;
		Registro *producto;
		std::string numeroPlacas;
		std::string nombreConductor;
		int tipoRegistro;
		std::string horaEntrada;
		std::string horaSalida;
		double pesoBruto;
		bool pesoBrutoEstablecido;
		double pesoTara;
		bool pesoTaraEstablecido;
		double descuento;
		bool descuentoEstablecido;
		double pesoNeto;
		bool pesoNetoCalculado;
		std::string observaciones;
		bool entradaManual;
		bool pendiente;
		bool habilitarDescuento;
		std::string nombreBasculista;
};

#endif
