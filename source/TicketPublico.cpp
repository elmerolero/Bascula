#include "TicketPublico.h"
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <regex>
#include <Windows.h>
using namespace std;

// Constructor
TicketPublico::TicketPublico():
	folio( 0 ), producto( nullptr ), tipoViaje( VIAJE_LOCAL ), pesoBruto( 0 ), pesoTara( 0 ), pesoNeto( 0 ), 
	pesoBrutoEstablecido( false ), pesoTaraEstablecido( false ), pesoNetoEstablecido( false ), pendiente( true ), entradaManual( true )
{
	 // Vacío
}

// Folio
void TicketPublico::establecerFolio( unsigned int folio )
{
	if( folio < 1 ){
		throw invalid_argument( "Ha ocurrido un error estableciendo el folio del ticket." );
	}
	
	this -> folio = folio;
}

unsigned int TicketPublico::obtenerFolio() const
{
	return folio;
}

// Fecha de registro
void TicketPublico::establecerFecha( std::string fechaRegistro )
{
	if( fechaRegistro.size() > 10 ){
		throw invalid_argument( "Se ha intentado establecer una fecha no adecuada." );
	}

	this -> fechaRegistro = fechaRegistro;
}

std::string TicketPublico::obtenerFecha() const
{
	return fechaRegistro;
}

// Numero de placas
void TicketPublico::establecerNumeroPlacas( std::string numeroPlacas )
{
	// Regex para validar el número económico
	regex formato( "[A-Z0-9Ñ]*" );
	
	// Verifica el formato de lo introducido
	if( numeroPlacas.size() > 7 || !regex_match( numeroPlacas, formato ) ){
		throw invalid_argument( "No se pudo establecer el número de placas.\nSolo admite letras, números y contiene 7 caracteres." );
	} 
	
	this -> numeroPlacas = numeroPlacas;
}

string TicketPublico::obtenerNumeroPlacas() const
{
	return numeroPlacas;
}

// Nombre del conductor
void TicketPublico::establecerNombreConductor( string nombreConductor )
{
	regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
	
	if( nombreConductor.empty() ){
		throw invalid_argument( "Debe introducir el nombre del conductor." );
	}

	if( nombreConductor.size() > 35 || !regex_match( nombreConductor, formato ) ){
		throw invalid_argument( "El nombre del conductor introducido no es válido." );
	}
	
	this -> nombreConductor = nombreConductor;
}

string TicketPublico::obtenerNombreConductor() const
{
	return nombreConductor;
}

// Producto
void TicketPublico::establecerProducto( Registro *producto )
{
	this -> producto = producto;
}

Registro *TicketPublico::obtenerProducto() const
{
	return producto;
}

// Tipo de viaje
void TicketPublico::establecerTipoViaje( int tipoViaje )
{
	if( tipoViaje > 1 || tipoViaje < 0 ){
		throw invalid_argument( "El tipo de viaje que se desea registrar no es válido." );
	}
	
	this -> tipoViaje = tipoViaje;
}

int TicketPublico::obtenerTipoViaje() const
{
	return tipoViaje;
}

// Hora de entrada
void TicketPublico::establecerHoraEntrada( std::string horaEntrada )
{
	if( horaEntrada.size() > 8 ){
		throw invalid_argument( "Se ha intentado establecer una hora no válida." );
	}

	this -> horaEntrada = horaEntrada;
}

std::string TicketPublico::obtenerHoraEntrada() const
{
	return horaEntrada;
}

// Hora de salida
void TicketPublico::establecerHoraSalida( std::string horaSalida )
{
	if( horaSalida.size() > 8 ){
		throw invalid_argument( "Se ha intentado establecer una hora no válida." );
	}

	this -> horaSalida = horaSalida;
}

std::string TicketPublico::obtenerHoraSalida() const
{
	return horaSalida;
}

// Peso bruto
void TicketPublico::establecerPesoBruto( string pesoBrutoStr )
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
			establecerPesoBrutoEstablecido( false );
			throw invalid_argument( "El peso bruto introducido no es válido." );
		}
		throw invalid_argument( ia.what() );
	}
}

void TicketPublico::establecerPesoBruto( double pesoBruto )
{
	// Debe ser un número positivo
	if( pesoBruto < 0 ){
		establecerPesoBrutoEstablecido( false );
		throw invalid_argument( "El peso bruto introducido no es válido, inténtalo de nuevo." );
	}

	// Establece el peso bruto
	this -> pesoBruto = pesoBruto;
	establecerPesoBrutoEstablecido( true );
}

double TicketPublico::obtenerPesoBruto() const
{
	return pesoBruto;
}

// Peso tara
void TicketPublico::establecerPesoTara( std::string pesoTaraStr )
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
			establecerPesoTaraEstablecido( false );
			throw invalid_argument( "El peso tara que se intenta registrar no es válido." );
		}
		
		throw invalid_argument( ia.what() );
	}
}

void TicketPublico::establecerPesoTara( double pesoTara )
{
	if( pesoBruto < 0 ){
		establecerPesoTaraEstablecido( false );
		throw invalid_argument( "El peso tara que se intenta registrar no es válido." );
	}

	this -> pesoTara = pesoTara;
	establecerPesoTaraEstablecido( true );
}

double TicketPublico::obtenerPesoTara() const
{
	return pesoTara;
}

// Peso neto
void TicketPublico::calcularPesoNeto()
{
	// ¿Se establecieron correctamente los campos necesarios para calcular el peso neto?
	if( !estaPesoBrutoEstablecido() || !estaPesoTaraEstablecido() ){
		establecerPesoNetoEstablecido( false );
		throw invalid_argument( "No se han establecido los campos necesarios para calcular el peso neto." );
	}

	establecerPesoNeto( abs( obtenerPesoBruto() - obtenerPesoTara() ) );
}

void TicketPublico::establecerPesoNeto( double pesoNeto )
{
	if( !estaPesoBrutoEstablecido() || !estaPesoTaraEstablecido() ){
		establecerPesoNetoEstablecido( false );
		throw invalid_argument( "El peso neto debe ser un valor absoluto positivo." );
	}

	this -> pesoNeto = pesoNeto;
	establecerPesoNetoEstablecido( true );
}

double TicketPublico::obtenerPesoNeto() const
{
	return pesoNeto;
}

// Bandera peso bruto establecido
void TicketPublico::establecerPesoBrutoEstablecido( bool pesoBrutoEstablecido )
{
	this -> pesoBrutoEstablecido = pesoBrutoEstablecido;
}

bool TicketPublico::estaPesoBrutoEstablecido() const
{
	return pesoBrutoEstablecido;
}

// Bandera de peso tara establecido
void TicketPublico::establecerPesoTaraEstablecido( bool pesoTaraEstablecido )
{
	this -> pesoTaraEstablecido = pesoTaraEstablecido;
}

bool TicketPublico::estaPesoTaraEstablecido() const
{
	return pesoTaraEstablecido;
}

// Bandera de peso neto establecido
void TicketPublico::establecerPesoNetoEstablecido( bool pesoNetoEstablecido )
{
	this -> pesoNetoEstablecido = pesoNetoEstablecido;
}

bool TicketPublico::estaPesoNetoEstablecido() const
{
	return pesoNetoEstablecido;
}

// Nombre del basculista
void TicketPublico::establecerNombreBasculista( std::string nombre )
{
	if( nombre.empty() ){
		throw invalid_argument( "Intento de establecer un nombre de basculista vacío." );
	}
	
	if( nombre.size() > 70 ){
		throw invalid_argument( "Intento de establecer un nombre de basculista mayor a 70 caracteres." );
	}
	
	this -> nombreBasculista = nombre;
}

std::string TicketPublico::obtenerNombreBasculista() const
{
	return nombreBasculista;
}

// Establece si está pendiente
void TicketPublico::establecerPendiente( bool pendiente )
{
	this -> pendiente = pendiente;
}

// Indica si está o no está pendiente el ticket
bool TicketPublico::estaPendiente() const
{
	return pendiente;
}

// Entrada manual
void TicketPublico::establecerEntradaManual( bool entradaManual )
{
	this -> entradaManual = entradaManual;
}

bool TicketPublico::esEntradaManual() const
{
	return entradaManual;
}

// Imprime los datos del ticket
void TicketPublico::imprimir( string nombreEmpresa ) const
{
	// Esta finalizado (o no está pendiente)
	if( estaPendiente() ){
		throw runtime_error( "Intento de impresión de un ticket que está pendiente." );
	}
	
	// Archivo HTML con la información del ticket
	ofstream pesajePublico;
	
	// Se abre el archivo
	pesajePublico.open( "../resources/data/pesaje.html", ios_base::out );
	if( !pesajePublico ){
		throw runtime_error( "No se pudo crear el registro de pesaje. Consulte la ayuda para conocer posibles soluciones." );
	}
	
	// Envía el formato html con los datos incluídos
	pesajePublico << "<!DOCTYPE html><html><head><title></title>Registro No. " << obtenerFolio() << "<style>*{ font-family: sans-serif; margin: 5px; }"
 					 "</style></head><body><p align='center'><strong>" << nombreEmpresa << "</strong></p><hr><p align='center'><strong>SERVICIO DE BÁSC"
 					 "ULA PUBLICA</strong></p><div style='display: flex; direction: row; justify-content: space-between; margin: 0;'><p><strong>Fecha:</strong>" 
 					 << obtenerFecha() << "</p><p><strong>Folio: </strong>" << setfill( '0' )<< setw( 7 ) << obtenerFolio() << "</p></div>" << setfill( ' ' ) <<
					 "<p align='center'><strong>Datos de pesaje</strong></p><div style='display: flex; direction: row; justify-content: space-between; margin: 0;'>"
					 "<div style='margin: 0;'><table style='border: 0; text-align: left;'><tr><th>Placas: </th><td>"
					 << obtenerNumeroPlacas() << "</td></tr><tr><th>Conductor:</th><td>"
					 << obtenerNombreConductor() << "</td></tr><tr><th>Producto: </th><td>"
					 << obtenerProducto() -> obtenerNombre() << "</td></tr><tr><th>Viaje: </th><td>" 
					 << (obtenerTipoViaje() == VIAJE_LOCAL ? "Local" : "Foráneo" ) << "</td></tr></table></div><div style='margin: 0;'>"
					 "<table style='border: 0; text-align: left;'><tr><th>Peso bruto: </th><td>"
					 << obtenerPesoBruto() << "</td><th>Hora entrada: </th><td>"
					 << obtenerHoraEntrada() << "</td></tr><tr><th>Peso tara: </th><td>"
					 << obtenerPesoTara() << "</td><th>Hora salida: </th><td>"
					 << obtenerHoraSalida() << "</td></tr></table><hr><table style='border: 0; text-align: left;'>"
					 "<tr style='border-top: black 1px solid;'><th>Peso neto:</th><td>"
					 << obtenerPesoNeto() <<"</td></tr></table></div></div><hr><p>Registro realizado por el basculista <u>"
					 << obtenerNombreBasculista() << "</u></p></body></html>" << endl;
	
	// Cierra archivo
	pesajePublico.close();
	
	ShellExecute(NULL, "open", "..\\resources\\data\\pesaje.html", NULL, NULL, SW_HIDE );
}
