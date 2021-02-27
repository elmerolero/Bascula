#include "Ticket.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <regex>
#include <fstream>
#include <iomanip>
#include <Windows.h>
using namespace std;

// Constructor
Ticket::Ticket():
	folio( 0 ), empresa( nullptr ), producto( nullptr ), tipoRegistro( TIPO_REGISTRO_ENTRADA ), pesoBruto( 0 ), pesoBrutoEstablecido( false ), pesoTara( 0 ), pesoTaraEstablecido( false ), 
	descuento( 0 ), habilitarDescuento( 0 ), descuentoEstablecido( 0 ), pesoNeto( 0 ), pesoNetoCalculado( false ), entradaManual( false ), pendiente( true )
{
	// Nada que hacer aquí
}

// Establece el folio del ticket
void Ticket::establecerFolio( unsigned int folio )
{
	if( folio < 1 ){
		throw invalid_argument( "Ah ocurrido un error estableciendo el folio del ticket." );
	}
	
	this -> folio = folio;
}

// Obtiene el folio del ticket
unsigned int Ticket::obtenerFolio() const
{
	return folio;
}

// Establece la fecha de registro
void Ticket::establecerFecha( string fechaRegistro )
{
	if( fechaRegistro.size() > 10 ){
		throw invalid_argument( "Se ha intentado establecer una fecha no adecuada." );
	}

	this -> fechaRegistro = fechaRegistro;
}

// Obtiene la fecha de registro
std::string Ticket::obtenerFecha() const
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
	regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
	
	if( nombreConductor.empty() ){
		throw invalid_argument( "Debe introducir el nombre del conductor." );
	}

	if( nombreConductor.size() > 35 || !regex_match( nombreConductor, formato ) ){
		throw invalid_argument( "El nombre del conductor introducido no es válido." );
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
		throw invalid_argument( "El peso bruto introducido no es válido." );
	}
}

void Ticket::establecerPesoBruto( double pesoBruto )
{
	// Debe ser un número positivo
	if( pesoBruto < 0.f ){
		throw invalid_argument( "El peso bruto introducido no es válido, inténtelo de nuevo." );
	}

	// Establece el peso bruto
	this -> pesoBruto = pesoBruto;
}

// Obtiene el peso bruto del ticket
double Ticket::obtenerPesoBruto() const
{
	return pesoBruto;
}

// Indica que el precio bruto se hubiera establecido adecuadamente
void Ticket::establecerPesoBrutoEstablecido( bool pesoBrutoEstablecido )
{
	this -> pesoBrutoEstablecido = pesoBrutoEstablecido;
}

bool Ticket::estaPesoBrutoEstablecido() const
{
	return pesoBrutoEstablecido;
}

// Establece el peso tara del ticket a partir de un string
void Ticket::establecerPesoTara( std::string pesoTaraStr )
{
	try{
		// Convierte el string en un double y lo establece
		double pesoTara = stod( pesoTaraStr );
		establecerPesoTara( pesoTara );
	}
	catch( invalid_argument &ia ){
		throw invalid_argument( "El peso tara que se intenta registrar no es válido. Inténtelo de nuevo." );
	}
}

void Ticket::establecerPesoTara( double pesoTara )
{
	if( pesoBruto < 0 ){
		throw invalid_argument( "El peso tara que se intenta registrar no es válido. Inténtelo de nuevo." );
	}

	this -> pesoTara = pesoTara;
}

// Obtiene el peso tara del ticket
double Ticket::obtenerPesoTara() const
{
	return pesoTara;
}

// Indica que el precio tara se hubiera establecido adecuadamente
void Ticket::establecerPesoTaraEstablecido( bool pesoTaraEstablecido )
{
	this -> pesoTaraEstablecido = pesoTaraEstablecido;
}

bool Ticket::estaPesoTaraEstablecido() const
{
	return pesoTaraEstablecido;
}

// Descuento desde string
void Ticket::establecerDescuento( string descuentoStr )
{
	if( descuentoStr.empty() ){
		throw invalid_argument( "Indicó que se introduciría un descuento, pero el campo está vacío." );
	}

	try{
		double descuento = stod( descuentoStr );
		establecerDescuento( descuento );
 	}
	catch( invalid_argument &ia ){
		throw invalid_argument( "El descuento introducido no es válido." );
	}
}

// Establece el descuento
void Ticket::establecerDescuento( double descuento )
{
	if( descuento < 0 ){
		throw invalid_argument( "El descuento introducido no es válido." );
	}
	
	this -> descuento = descuento;
}

// Obtien el descuento
double Ticket::obtenerDescuento() const
{
	return descuento;
}

// Bandera que decide si calcular con descuento o no
void Ticket::permitirDescuento( bool opcion )
{
	// Deshabilita la opcion
	this -> habilitarDescuento = opcion;
}

bool Ticket::permitirDescuento() const
{
	return habilitarDescuento;
}

// Indica que el precio descuento se estableció adecuadamente
void Ticket::establecerDescuentoEstablecido( bool descuentoEstablecido )
{
	this -> descuentoEstablecido = descuentoEstablecido;
}

bool Ticket::estaDescuentoEstablecido() const
{
	return descuentoEstablecido;
}

// Establece el descuento a partir de un string dado
void Ticket::establecerPesoNeto( string pesoNetoStr )
{
	try{
		double pesoNeto = stod( pesoNetoStr );
		establecerPesoNeto( pesoNeto );
	}
	catch( invalid_argument &ia ){
		throw invalid_argument( "El peso neto introducido no es válido." );
	}
}

// Establece el peso neto
void Ticket::establecerPesoNeto( double pesoNeto )
{
	if( pesoNeto < 0 ){
		throw invalid_argument( "El peso neto debe ser un valor absoluto positivo. Asegurése de que el \npeso neto introducido no sea mayor que la diferencia." );
	}

	this -> pesoNeto = pesoNeto;
}

// Obtiene el peso bruto del ticket
double Ticket::obtenerPesoNeto() const
{
	return pesoNeto;
}

// Indica que el peso neto se hubiera establecido adecuadamente
void Ticket::establecerPesoNetoEstablecido( bool pesoNetoCalculado )
{
	this -> pesoNetoCalculado = pesoNetoCalculado;
}

bool Ticket::estaPesoNetoEstablecido() const
{
	return pesoNetoCalculado;
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
	regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ,\\s.]*" );
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

// Establece el nombre del basculista
void Ticket::establecerNombreBasculista( std::string nombre )
{
	if( nombre.empty() ){
		throw invalid_argument( "Intento de establecer un nombre de basculista vacío." );
	}
	
	if( nombre.size() > 70 ){
		throw invalid_argument( "Intento de establecer un nombre de basculista mayor a 70 caracteres." );
	}
	
	this -> nombreBasculista = nombre;
}

std::string Ticket::obtenerNombreBasculista() const
{
	return nombreBasculista;
}

// Imprime los datos del ticket
void Ticket::imprimir( std::string nombreEmpresa, unsigned int numeroFormatos, unsigned int numeroCopias ) const
{
	// Esta finalizado (o no está pendiente)
	if( estaPendiente() ){
		throw runtime_error( "Intento de impresión de un ticket que está pendiente." );
	}
	
	// Si la cantidad de formatos es menor o igual que cero
	if( numeroFormatos <= 0 && numeroCopias <= 0 ){
		return;	// Regresa (no hay nada que hacer)
	}

	// Archivo HTML con la información del ticket
	ofstream pesajeInterno;
	
	// Se abre el archivo
	pesajeInterno.open( "../resources/data/pesaje.html", ios_base::out );
	if( !pesajeInterno ){
		throw runtime_error( "No se pudo crear el registro de pesaje. Consulte la ayuda para conocer posibles soluciones." );
	}
	
	// Datos temporales de completado
	string nombreBasculista = "Nombre del Basculista";
	
	// Registra el encabezado 
	pesajeInterno << "<!DOCTYPE html><html><head><meta charset='utf-8'><style>*{font-family: sans-serif; font-size: 15px; "
			         "margin: 1px; }</style></head><body>";

	// Incrusta el número de formatos solicitados
	for( size_t contador = 0; contador < numeroFormatos; ++contador ){
		pesajeInterno << "<div><strong><p align='center'>" << nombreEmpresa
					  << "</p></strong><hr> <div style='display: flex; direction: row; justify-content: space-between;'>"
					  << "<p><strong>FECHA:</strong>" << obtenerFecha() 
					  << "</p><p align='center'><strong>" << ( obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Proveedor " : "Cliente " )
					  << "</strong>" << obtenerEmpresa() -> obtenerClave() << " - " << obtenerEmpresa() -> obtenerNombre()
					  << "</p><p><strong>FOLIO: </strong>" << obtenerFolio()
					  << "</p></div><div style='display: flex; justify-content: space-between;'><div style='width: 50%;'>"
						 "<table style='border: 0; text-align: left;'><tr>"
					  << "<th>Registra: </th><td>" << ( obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" )
					  << "</td></tr><tr>"
					  << "<th>Producto: </th><td>" << obtenerProducto() -> obtenerClave() << " - " << obtenerProducto() -> obtenerNombre()
					  << "</td></tr><tr>"
					  << "<th>Placas: </th><td>" << obtenerNumeroPlacas()
					  << "</td></tr><tr>"
					  << "<th>Conductor: </th><td>" << obtenerNombreConductor()
					  << "</td></tr></table></div><div style='display: flex; align-items: space-between;'>"
					  << "<table style='border: 0; text-align: left;'><tr>"
					  << "<th>Hora entrada:</th><td>" << obtenerHoraEntrada()
					  << "</td></tr><tr>"
					  << "<th>Hora salida:</th><td>" << obtenerHoraSalida()
					  << "</td></tr></table><table style='border: 0; text-align: left; margin-left: auto;'><tr>"
					  << "<th>Peso bruto:</th><td>" << obtenerPesoBruto()
					  << "</td></tr><tr>"
					  << "<th>Peso tara:</th><td>" << obtenerPesoTara()
					  << "</td></tr><tr>"
					  << "<th>Descuento:</th><td>" << obtenerDescuento()
					  << "</td></tr></table></div></div><hr><table style='border: 0; text-align: left; margin-left: auto;'><tr>"
					  << "<th>Peso neto:</th><td>" << obtenerPesoNeto()
					  << "</td></tr></table><hr><p><strong>Observaciones:</strong></p>"
					  << "<p>" << obtenerObservaciones() << "</p>"
					  << "<hr>"
					  << "<p>" << ( esEntradaManual() ? "El pesaje en alguno de los campos fue introducido de manera manual. " : "" ) 
					  << "Registro realizado por el basculista <u>" << obtenerNombreBasculista() 
					  << "</u></p><br><p><strong>Autoriza: </strong>_______________________</p><br><hr></div>" << endl;
	}

	// Crea el número de copias
	for( size_t contador = 0; contador < numeroCopias; ++contador ){
		pesajeInterno << "<div><strong><p align='center'>" << nombreEmpresa
					  << "</p></strong><hr> <div style='display: flex; direction: row; justify-content: space-between;'>"
					  << "<p><strong>FECHA:</strong>" << obtenerFecha() 
					  << "</p><p align='center'><strong>" << ( obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Proveedor " : "Cliente " )
					  << "</strong>" << obtenerEmpresa() -> obtenerClave() << " - " << obtenerEmpresa() -> obtenerNombre()
					  << "</p><p><strong>FOLIO: </strong>" << obtenerFolio()
					  << "</p></div><div style='display: flex; justify-content: space-between;'><div style='width: 50%;'>"
						 "<table style='border: 0; text-align: left;'><tr>"
					  << "<th>Registra: </th><td>" << ( obtenerTipoRegistro() == TIPO_REGISTRO_ENTRADA ? "Entrada" : "Salida" )
					  << "</td></tr><tr>"
					  << "<th>Producto: </th><td>" << obtenerProducto() -> obtenerClave() << " - " << obtenerProducto() -> obtenerNombre()
					  << "</td></tr><tr>"
					  << "<th>Placas: </th><td>" << obtenerNumeroPlacas()
					  << "</td></tr><tr>"
					  << "<th>Conductor: </th><td>" << obtenerNombreConductor()
					  << "</td></tr></table></div><div style='display: flex; align-items: space-between;'>"
					  << "<table style='border: 0; text-align: left;'><tr>"
					  << "<th>Hora entrada:</th><td>" << obtenerHoraEntrada()
					  << "</td></tr><tr>"
					  << "<th>Hora salida:</th><td>" << obtenerHoraSalida()
					  << "</td></tr></table><table style='border: 0; text-align: left; margin-left: auto;'><tr>"
					  << "<th>Peso bruto:</th><td>" << obtenerPesoBruto()
					  << "</td></tr><tr>"
					  << "<th>Peso tara:</th><td>" << obtenerPesoTara()
					  << "</td></tr><tr>"
					  << "<th>Descuento:</th><td>" << obtenerDescuento()
					  << "</td></tr></table></div></div><hr><table style='border: 0; text-align: left; margin-left: auto;'><tr>"
					  << "<th>Peso neto:</th><td>" << obtenerPesoNeto()
					  << "</td></tr></table><hr><p><strong>Observaciones:</strong></p>"
					  << "<p>" << obtenerObservaciones() << "</p>"
					  << "<hr><br><p><strong>Autoriza: </strong>_______________________</p><br><hr></div>" << endl;
	}

	// Establece el pié de página del documento
	pesajeInterno << "</body></html>" << endl;
	
	// Cierra archivo
	pesajeInterno.close();
	
	ShellExecute(NULL, "open", "..\\resources\\data\\pesaje.html", NULL, NULL, SW_HIDE );
}
