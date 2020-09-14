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

// Establece el tipo de registro
void Ticket::establecerTipoRegistro( int tipoRegistro )
{
	// Un número dentro del rango
	if( tipoRegistro < 0 || tipoRegistro > 1 ){
		throw invalid_argument( "Tipo de registro no conocido." );
	}
	
	this -> tipoRegistro = tipoRegistro;
}

// Obtiene el tipo de registro
int Ticket::obtenerTipoRegistro() const
{
	return tipoRegistro;
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

// Establece el peso bruto del ticket
void Ticket::establecerPesoBruto( string pesoBrutoStr )
{
	try{
		// Convierte el string a un número entero
		double pesoBruto = stod( pesoBrutoStr );
		
		// Establece el peso bruto
		establecerPesoBruto( pesoBruto );
	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stod" ) == 0 ){
			throw invalid_argument( "El número introducido no es válido." );
		}
		
		throw invalid_argument( ia.what() );
	}
}

void Ticket::establecerPesoBruto( double pesoBruto )
{
	// Debe ser un número positivo
	if( pesoBruto < 0 ){
		throw invalid_argument( "El peso bruto introducido no es válido, inténtalo de nuevo." );
	}

	// Establece el peso bruto
	this -> pesoBruto = pesoBruto;
}

// Obtiene el peso bruto del ticket
double Ticket::obtenerPesoBruto() const
{
	return pesoBruto;
}

// Establece el peso tara del ticket
void Ticket::establecerPesoTara( std::string pesoTaraStr )
{
	try{
		// Convierte el string en un double
		double pesoTara = stod( pesoTaraStr );
		
		// Establece el número convertido
		establecerPesoTara( pesoTara );
	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stod" ) == 0 ){
			throw invalid_argument( "El número que se intenta registrar no es válido." );
		}
		
		throw invalid_argument( ia.what() );
	}
}

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

// Bandera que decide si calcular con descuento o no
void Ticket::permitirDescuento( bool opcion )
{
	// Establece el descuento
	this -> habilitarDescuento = opcion;
}

bool Ticket::permitirDescuento() const
{
	return habilitarDescuento;
}

// Descuento
void Ticket::establecerDescuento( string descuentoStr )
{
	try{
		double descuento = permitirDescuento() ? stod( descuentoStr ) : 0;
		
		establecerDescuento( descuento );
 	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stod" ) == 0 ){
			throw invalid_argument( "El número introducido no es válido." );
		}
		
		throw invalid_argument( ia.what() );
	}
}

void Ticket::establecerDescuento( double descuento )
{
	if( descuento < 0 || descuento > 100 ){
		throw invalid_argument( "Un descuento debe ser un número positivo no mayor a cien." );
	}
	
	this -> descuento = descuento;
}

double Ticket::obtenerDescuento() const
{
	return descuento;
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

// Establece si el peso se introdujo de manera manual o automática
void Ticket::establecerEntradaManual( bool entradaManual )
{
	this -> entradaManual = entradaManual;
}

bool Ticket::esEntradaManual() const
{
	return entradaManual;
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
