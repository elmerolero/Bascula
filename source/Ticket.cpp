#include "Ticket.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <regex>
#include <iomanip>
using namespace std;

// Establece el folio del ticket
void Ticket::establecerFolio( int folio )
{
	if( folio < 1 ){
		throw invalid_argument( "Ah ocurrido un error estableciendo el folio del ticket." );
	}
	
	this -> folio = folio;
}

// Obtiene el folio del ticket
int Ticket::obtenerFolio() const
{
	return folio;
}

// Establece la fecha de registro
void Ticket::establecerFechaRegistro( string fechaRegistro )
{
	if( fechaRegistro.size() > 10 ){
		throw invalid_argument( "Se ha intentado establecer una fecha no adecuada." );
	}

	this -> fechaRegistro = fechaRegistro;
}

// Obtiene la fecha de registro
std::string Ticket::obtenerFechaRegistro() const
{
	return fechaRegistro;
}

// Establece la empresa
void Ticket::establecerEmpresa( Registro *empresa )
{
	this -> empresa = empresa;
}

// Obtiene la empresa
Registro *Ticket::obtenerEmpresa() const
{
	return empresa;
}

// Establece el producto
void Ticket::establecerProducto( Registro *producto )
{
	this -> producto = producto;
}

// Obtiene el producto
Registro *Ticket::obtenerProducto() const
{
	return producto;
}

// Establece el numero de placas
void Ticket::establecerNumeroPlacas( std::string numeroPlacas )
{
	// Regex para validar el número económico
	regex formato( "[A-Z0-9Ñ]*" );
	
	// Obliga a que se introduzca un número de placas
	if( numeroPlacas.empty() ){
		throw invalid_argument( "No se ha introducido un número de placas." );
	}
	
	// Verifica el formato de lo introducido
	if( numeroPlacas.size() > 7 || !regex_match( numeroPlacas, formato ) ){
		throw invalid_argument( "No se pudo establecer el número de placas.\nSolo admite letras, números y contiene 7 caracteres." );
	} 
	
	this -> numeroPlacas = numeroPlacas;
}

// Obtiene el numero de placas
string Ticket::obtenerNumeroPlacas() const
{
	return numeroPlacas;
}

// Establece el nombre del conductor
void Ticket::establecerNombreConductor( string nombreConductor )
{
	if( nombreConductor.empty() ){
		throw invalid_argument( "Debe introducir el nombre del conductor." );
	}

	if( nombreConductor.size() > 35 ){
		throw invalid_argument( "El nombre introducido es demasiado largo." );
	}
	
	this -> nombreConductor = nombreConductor;
}

// Obtiene el nombre del conductor
string Ticket::obtenerNombreConductor() const
{
	return nombreConductor;
}

// Establece la clave de la procedencia
void Ticket::establecerTipoViaje( int claveTipo )
{
	// Es una clave entera positiva
	if( claveTipo < 0 || claveTipo < 3 ){
		throw invalid_argument( "Se intento introducir un\ntipo de viaje inválido." );
	}
	
	this -> tipoViaje = claveTipo;
}

// Obtiene la clave de procedencia
int Ticket::obtenerTipoViaje() const
{
	return tipoViaje;
}

// Establece el peso bruto del ticket
void Ticket::establecerPesoBruto( double pesoBruto )
{
	if( pesoBruto < 0 ){
		throw invalid_argument( "Mala lectura de la báscula,\ninténtalo de nuevo." );
	}

	this -> pesoBruto = pesoBruto;
}

// Obtiene el peso bruto del ticket
double Ticket::obtenerPesoBruto() const
{
	return pesoBruto;
}

// Establece el peso tara del ticket
void Ticket::establecerPesoTara( double pesoTara )
{
	if( pesoBruto < 0 ){
		throw invalid_argument( "Mala lectura de la báscula,\ninténtalo de nuevo." );
	}

	this -> pesoTara = pesoTara;
}

// Obtiene el peso tara del ticket
double Ticket::obtenerPesoTara() const
{
	return pesoTara;
}

// Calcula el peso neto
void Ticket::calcularPesoNeto()
{
	establecerPesoNeto( abs( obtenerPesoBruto() - obtenerPesoTara() ) );
}

// Establece el peso bruto del ticket
void Ticket::establecerPesoNeto( double pesoNeto )
{
	if( pesoBruto < 0 ){
		throw invalid_argument( "El peso neto debe ser un valor absoluto positivo." );
	}

	this -> pesoNeto = pesoNeto;
}

// Obtiene el peso bruto del ticket
double Ticket::obtenerPesoNeto() const
{
	return pesoNeto;
}

// Permite establecer la hora de entrada
void Ticket::establecerHoraEntrada( std::string horaEntrada )
{
	if( horaEntrada.size() > 8 ){
		throw invalid_argument( "Se ha intentado establecer una hora no válida." );
	}

	this -> horaEntrada = horaEntrada;
}

// Obtiene la hora de entrada
std::string Ticket::obtenerHoraEntrada() const
{
	return horaEntrada;
}

// Establece la hora de salida
void Ticket::establecerHoraSalida( std::string horaSalida )
{
	if( horaSalida.size() > 8 ){
		throw invalid_argument( "Se ha intentado establecer una hora no válida." );
	}

	this -> horaSalida = horaSalida;
}

// Obtiene la hora de salida
std::string Ticket::obtenerHoraSalida() const
{
	return horaSalida;
}

// Establece las observaciones indicadas
void Ticket::establecerObservaciones( string observaciones )
{
	regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
	if( observaciones.size() > 500 || !regex_match( observaciones, formato ) ){
		throw invalid_argument( "Se están dando observaciones, no válidas." );
	}
	
	this -> observaciones = observaciones;
}

// Obtiene las observaciones indicadas
string Ticket::obtenerObservaciones() const
{
	return observaciones;
}

// Establece si está pendiente
void Ticket::establecerPendiente( bool pendiente )
{
	this -> pendiente = pendiente;
}

// Indica si está o no está pendiente el ticket
bool Ticket::estaPendiente() const
{
	return pendiente;
}

// Imprime los datos del ticket
void Ticket::imprimir() const
{
	
}
