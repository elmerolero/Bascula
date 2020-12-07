#include "LectorBascula.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "Widget.h"
#include "Aplicacion.h"
using namespace std;

// Lector Bascula
LectorBascula lectorBascula;

// Elementos para poder ejecutar la báscula
HANDLE dispositivo;
BOOL status;
stringstream data;
DWORD dwEventMask;
char tempChar;
char serialBuffer[ 256 ];
DWORD noBytesRead;
bool actualizando;

LectorBascula::LectorBascula():
	activado( false ), lecturaManual( false ), botonLectorBasculaId( 0 ), comboBoxLectorBasculaId( 0 ), bytesIgnorados( 4 ), baudRate( CBR_9600 ),
	byteSize( 8 ), stopBits( 0 ), parity( NOPARITY )
{

}

void LectorBascula::abrir( Widget &interfaz )
{
	this -> interfaz = interfaz;
	
	// Muestra la ventana del lector
	interfaz.mostrarElemento( "VentanaLectorPeso" );
	
	// Establece las opciones de entrada
	actualizarOpciones();
	comboBoxLectorBasculaId = interfaz.conectarSenal( "BasculasDisponibles", "changed", G_CALLBACK( lectorBasculaIniciarOpcion ), nullptr );

	interfaz.establecerTextoEntrada( "EntradaPeso", "" );
	interfaz.establecerTextoEtiqueta( "EtiquetaPeso", "0.0 Kg" );
}

const char *LectorBascula::leer()
{
	// Establece el peso leído en la etiqueta
	return ( lecturaManualActivada() ? interfaz.obtenerTextoEntrada( "EntradaPeso" ) : interfaz.obtenerTextoEtiqueta( "EtiquetaPeso" ) );
}

void LectorBascula::actualizarOpciones()
{
	// Limpia las opciones que había anteriormente
	interfaz.limpiarComboBoxText( "BasculasDisponibles" );
	
	// Agrega las opciones seleccionar y manual
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Seleccionar", "Seleccionar" );
	interfaz.establecerActivoComboBoxText( "BasculasDisponibles", "Seleccionar" );
	interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", "Entrada manual", NULL );

	// Agrega los puertos seriales disponibles
	char directorioPuerto[ 5000 ];

	// Revisa si hay puertos disponibles
	int contadorDispositivos = 0;
	for( unsigned contador = 0; contador < 255; ++contador ){
		string nombrePuerto = "COM" + to_string( contador );
		DWORD intento = QueryDosDevice( nombrePuerto.c_str(), directorioPuerto, 5000 );
		if( intento != 0 ){
			interfaz.agregarOpcionComboBoxText( "BasculasDisponibles", ( contador > 9 ? "\\\\.\\" + nombrePuerto : nombrePuerto ).c_str(), NULL );
			contadorDispositivos++;
		}
	}

	if( contadorDispositivos < 1 ){
		interfaz.establecerTextoEtiqueta( "DialogoMensaje", "No se detectaron básculas." );
    	interfaz.mostrarElemento( "VentanaMensaje" );
	}
}

void LectorBascula::iniciarOpcion()
{
	// Obtiene la opcion
	string opcion = interfaz.obtenerOpcionComboBoxText( "BasculasDisponibles" );
	interfaz.establecerTextoEntrada( "EntradaPeso", "" );
	interfaz.establecerTextoEtiqueta( "EtiquetaPeso", "0.0 Kg" );

	// Si la opcion seleccionada es "Introducción manual
	if( opcion.compare( "Entrada manual" ) == 0 ){
		establecerLecturaManual( true );
		interfaz.ocultarElemento( "EtiquetaPeso" );
		interfaz.mostrarElemento( "EntradaPeso" );
		interfaz.habilitarEdicionEntrada( "EntradaPeso" );
	}
	else if( opcion.compare( "Seleccionar" ) != 0 ){
		establecerLecturaManual( false );
		interfaz.deshabilitarEdicionEntrada( "EntradaPeso" );
		interfaz.ocultarElemento( "EntradaPeso" );
		interfaz.mostrarElemento( "EtiquetaPeso" );
		establecerPuerto( opcion );
		lectorBasculaConectar();
		establecerActivo( true );
	}
}

void LectorBascula::cerrar()
{
	// Oculta la ventana
	interfaz.ocultarElemento( "VentanaLectorPeso" );
	
	// Deshabilita la entrada peso
	interfaz.deshabilitarEdicionEntrada( "EntradaPeso" );
	
	// Desconecta la señal
	interfaz.desconectarSenal( "BotonRegistrarPeso", botonLectorBasculaId );
	interfaz.desconectarSenal( "BasculasDisponibles", comboBoxLectorBasculaId );

	establecerActivo( false );
	CancelIoEx( dispositivo, NULL );
	CloseHandle( dispositivo );
}

void LectorBascula::cargarConfiguracion()
{
	ifstream archivoConfig;

	// Intenta abrir el archivo
	archivoConfig.open( "../resources/data/config.dat" );
	if( !archivoConfig.is_open() ){
		return;	// No hace nada, se conserva la configuración por defecto
	}

	// Establece los datos 
	int bytesIgnorados;
	int baudrate;
	int byteSize;
	int stopBits;
	int parity;

	// Lee cada uno de los parametros del archivo
	archivoConfig >> bytesIgnorados;
	archivoConfig >> baudrate;
	archivoConfig >> byteSize;
	archivoConfig >> stopBits;
	archivoConfig >> parity;

	// Cierra el archivo
	archivoConfig.close();

	try{
		// Intenta establecer los valores del archivo de configuracion
		lectorBascula.establecerBytesIgnorados( bytesIgnorados );
		lectorBascula.establecerBaudRate( baudrate );
		lectorBascula.establecerByteSize( byteSize );
		lectorBascula.establecerStopBits( stopBits );
		lectorBascula.establecerParity( parity );
	}
	catch( invalid_argument &ia ){
		// Hubo un error con alguno de los valores, se establece la configuración por defecto
		lectorBascula.establecerBytesIgnorados( 4 );
		lectorBascula.establecerBaudRate( CBR_9600 );
		lectorBascula.establecerByteSize( 8 );
		lectorBascula.establecerStopBits( 0 );
		lectorBascula.establecerParity( NOPARITY );
	}
}

void LectorBascula::establecerIdSenal( guint senal )
{
	botonLectorBasculaId = senal;
}

void LectorBascula::establecerLecturaManual( bool lecturaManual )
{
	this -> lecturaManual = lecturaManual;
}

bool LectorBascula::lecturaManualActivada() const
{
	return lecturaManual;
}

void LectorBascula::establecerActivo( bool activo )
{
	this -> activado = activo;
}

bool LectorBascula::estaActivo() const
{
	return activado;
}

void LectorBascula::establecerPuerto( string puerto )
{
	this -> puerto = puerto;
}

string LectorBascula::obtenerPuerto() const
{
	return puerto;
}

void LectorBascula::establecerBytesIgnorados( string bytesIgnoradosStr )
{
	try{
		unsigned int bytesIgnorados = stoi( bytesIgnoradosStr );
		
		establecerBytesIgnorados( bytesIgnorados );
	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stoi" ) == 0 ){
			throw invalid_argument( "El numero de bytes a ignorar no es válido." );
		}

		throw invalid_argument( ia.what() );
	}
}

void LectorBascula::establecerBytesIgnorados( unsigned int bytesIgnorados )
{
	if( bytesIgnorados < 0 || bytesIgnorados > 10 ){
		throw invalid_argument( "El número de bytes ignorados no puede superar los 10 bytes." );
	}

	this -> bytesIgnorados = bytesIgnorados;
}

unsigned int LectorBascula::obtenerBytesIgnorados() const
{
	return bytesIgnorados;
}

void LectorBascula::establecerBaudRate( string baudRateStr )
{
	try{
		if( baudRateStr.compare( "110 bps" ) == 0 ){
			establecerBaudRate( CBR_110 );
		}
		else if( baudRateStr.compare( "300 bps" ) == 0 ){
			establecerBaudRate( CBR_300 );
		}
		else if( baudRateStr.compare( "600 bps" ) == 0 ){
			establecerBaudRate( CBR_600 );
		}
		else if( baudRateStr.compare( "1200 bps" ) == 0 ){
			establecerBaudRate( CBR_1200 );
		}
		else if( baudRateStr.compare( "2400 bps" ) == 0 ){
			establecerBaudRate( CBR_2400 );
		}
		else if( baudRateStr.compare( "4800 bps" ) == 0 ){
			establecerBaudRate( CBR_4800 );
		}
		else if( baudRateStr.compare( "9600 bps" ) == 0 ){
			establecerBaudRate( CBR_9600 );
		}
		else if( baudRateStr.compare( "14400 bps" ) == 0 ){
			establecerBaudRate( CBR_14400 );
		}
		else if( baudRateStr.compare( "19200 bps" ) == 0 ){
			establecerBaudRate( CBR_19200 );
		}
		else if( baudRateStr.compare( "38400 bps" ) == 0 ){
			establecerBaudRate( CBR_38400 );
		}
		else if( baudRateStr.compare( "57600 bps" ) == 0 ){
			establecerBaudRate( CBR_57600 );
		}
		else if( baudRateStr.compare( "115200 bps" ) == 0 ){
			establecerBaudRate( CBR_115200 );
		}
		else if( baudRateStr.compare( "128000 bps" ) == 0 ){
			establecerBaudRate( CBR_128000 );
		}
		else if( baudRateStr.compare( "256000 bps" ) == 0 ){
			establecerBaudRate( CBR_256000 );
		}
		else{
			throw invalid_argument( "El baudrate introducido no es válido." );
		}
	}
	catch( invalid_argument &ia ){
		throw invalid_argument( ia.what() );
	}
}

void LectorBascula::establecerBaudRate( DWORD baudRate )
{
	// Establece el baudrate
	switch( baudRate ){
		case CBR_110:
			this -> baudRate = CBR_110;
			break;
		case CBR_300:
			this -> baudRate = CBR_300;
			break;
		case CBR_600:
			this -> baudRate = CBR_600;
			break;
		case CBR_1200:
			this -> baudRate = CBR_1200;
			break;
		case CBR_2400:
			this -> baudRate = CBR_2400;
			break;
		case CBR_4800:
			this -> baudRate = CBR_4800;
			break;
		case CBR_9600:
			this -> baudRate = CBR_9600;
			break;
		case CBR_14400:
			this -> baudRate = CBR_14400;
			break;
		case CBR_19200:
			this -> baudRate = CBR_19200;
			break;
		case CBR_38400:
			this -> baudRate = CBR_38400;
			break;
		case CBR_57600:
			this -> baudRate = CBR_57600;
			break;
		case CBR_115200:
			this -> baudRate = CBR_115200;
			break;
		case CBR_128000:
			this -> baudRate = CBR_128000;
			break;
		case CBR_256000:
			this -> baudRate = CBR_256000;
			break;
		default:
			throw invalid_argument( "El baudrate que se desea introducir no es válido." );
			break;
	}
}

DWORD LectorBascula::obtenerBaudRate() const
{
	return baudRate;
}

void LectorBascula::establecerByteSize( std::string byteSizeStr )
{
	try{
		BYTE byteSize = stoi( byteSizeStr );

		establecerByteSize( byteSize );
	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stoi" ) == 0 ){
			throw invalid_argument( "El tamaño de bit que se desea establecer no es válido." );
		}

		throw invalid_argument( ia.what() );
	}
}

void LectorBascula::establecerByteSize( BYTE byteSize )
{
	if( byteSize < 5 || byteSize > 8 ){
		throw invalid_argument( "El tamaño indicado no es válido." );
	}

	this -> byteSize = byteSize;
}

BYTE LectorBascula::obtenerByteSize() const
{
	return byteSize;
}

void LectorBascula::establecerStopBits( string stopBitsStr )
{
	try{
		BYTE stopBits = stoi( stopBitsStr );

		establecerStopBits( stopBits );
	}
	catch( invalid_argument &ia ){
		string what = ia.what();
		if( what.compare( "stoi" ) == 0 ){
			throw invalid_argument( "Los bits de stop indicados no son válidos." );
		}

		throw invalid_argument( ia.what() );
	}
}

void LectorBascula::establecerStopBits( BYTE stopBits )
{
	if( stopBits < 0 || stopBits > 2 ){
		throw invalid_argument( "Solo es posible establecer uno o dos bits de stop." );
	}

	this -> stopBits = stopBits;
}

BYTE LectorBascula::obtenerStopBits() const
{
	return stopBits;
}

void LectorBascula::establecerParity( string parityStr )
{
	if( parityStr.compare( "Ninguno" ) == 0 ){
		establecerParity( NOPARITY );
	}
	else if( parityStr.compare( "Impar" ) == 0 ){
		establecerParity( ODDPARITY );
	}
	else if( parityStr.compare( "Par" ) == 0 ){
		establecerParity( EVENPARITY );
	}
	else if( parityStr.compare( "Marca" ) == 0 ){
		establecerParity( MARKPARITY );
	}
	else if( parityStr.compare( "Espacio" ) == 0 ){
		establecerParity( SPACEPARITY );
	}
	else{
		throw invalid_argument( "La paridad indicada no son válida." );
	}
}

void LectorBascula::establecerParity( BYTE parity )
{
	switch( parity ){
		case NOPARITY:
			this -> parity = NOPARITY;
			break;
		case ODDPARITY:
			this -> parity = ODDPARITY;
			break;
		case EVENPARITY:
			this -> parity = EVENPARITY;
			break;
		case MARKPARITY:
			this -> parity = MARKPARITY;
			break;
		case SPACEPARITY:
			this -> parity = SPACEPARITY;
			break;
		default:
			throw invalid_argument( "La paridad que se desea establecer no es válida." );
			break;
	}
}

BYTE LectorBascula::obtenerParity() const
{
	return parity;
}

// Actualiza la configuración
void lectorBasculaActualizarOpciones()
{
	// Respalda las opciones establecidas antes de hacer cualquier cambio
	unsigned int bytesIgnorados = lectorBascula.obtenerBytesIgnorados();
	DWORD baudrate = lectorBascula.obtenerBaudRate();
	BYTE byteSize = lectorBascula.obtenerByteSize();
	BYTE stopBits = lectorBascula.obtenerStopBits();
	BYTE parity = lectorBascula.obtenerParity();

	try{
		// Intenta actualizar la configuración de la báscula
		lectorBascula.establecerBaudRate( interfaz.obtenerOpcionComboBoxText( "OpcionesBaudrate" ) );
		lectorBascula.establecerByteSize( interfaz.obtenerTextoEntrada( "OpcionesBitsDatos" ) );
		lectorBascula.establecerStopBits( interfaz.obtenerTextoEntrada( "OpcionesBitsStop" ) );
		lectorBascula.establecerParity( interfaz.obtenerOpcionComboBoxText( "OpcionesParidad" ) );
		lectorBascula.establecerBytesIgnorados( interfaz.obtenerTextoEntrada( "OpcionesBytesIgnorados" ) );

		// Actualiza el archivo de configuración
		ofstream archivoConfig;
	
		// Se abre el archivo
		archivoConfig.open( "../resources/data/config.dat", ios_base::out );
		if( !archivoConfig ){
			throw runtime_error( "No se pudo crear el archivo de configuración." );
		}

		archivoConfig << (int)lectorBascula.obtenerBytesIgnorados() << endl
					  << (int)lectorBascula.obtenerBaudRate() << endl
					  << (int)lectorBascula.obtenerByteSize() << endl
					  << (int)lectorBascula.obtenerStopBits() << endl
					  << (int)lectorBascula.obtenerParity() << endl;

		archivoConfig.close();

		mostrarMensaje( "Configuración actualizada." );
		irHacia( nullptr, (void *)"Inicio" );
	}
	catch( invalid_argument &ia ){
		interfaz.establecerTextoEtiqueta( "MensajeErrorConfiguracion", ia.what() );
		interfaz.mostrarElemento( "MensajeErrorConfiguracion" );

		lectorBascula.establecerBaudRate( baudrate );
		lectorBascula.establecerByteSize( byteSize );
		lectorBascula.establecerStopBits( stopBits );
		lectorBascula.establecerParity( parity );
		lectorBascula.establecerBytesIgnorados( bytesIgnorados );
	}
}

// Establece las opciones de la báscula
void lectorBasculaIniciarOpcion( GtkWidget *widget, gpointer ptr )
{
	lectorBascula.iniciarOpcion();
}

// Actualiza el peso que se está leyendo
void lectorBasculaActualizarPeso()
{
	// Conecta con el puerto indicado
	while( aplicacionActiva ){
		while( lectorBascula.estaActivo() ){
			actualizando = true;
			try{
				lectorBasculaObtenerPeso();
			}
			catch( runtime_error &re ){
				;
			}
			actualizando = false;
		}
	}
}

// Conecta con el medidor
void lectorBasculaConectar()
{
	dispositivo = CreateFile( lectorBascula.obtenerPuerto().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
	
	if( dispositivo != INVALID_HANDLE_VALUE ){
		status = TRUE;
	}
	
	if( status == FALSE ){
		throw runtime_error( "No fue encontrado ningún puerto." );
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof( dcbSerialParams );

	status = GetCommState( dispositivo, &dcbSerialParams );

	dcbSerialParams.BaudRate = lectorBascula.obtenerBaudRate();
	dcbSerialParams.ByteSize = lectorBascula.obtenerByteSize();
	dcbSerialParams.StopBits = lectorBascula.obtenerStopBits();
	dcbSerialParams.Parity = lectorBascula.obtenerParity();

	status = SetCommState( dispositivo, &dcbSerialParams );
	if( status == FALSE ){
		throw runtime_error( "No se pudo realizar la conexión.");
	}

	/*COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 10;

	status = SetCommTimeouts( dispositivo, &timeout );
	if( status == FALSE ){
		throw runtime_error( "Error al establecer la información de .");
	}*/

	status = SetCommMask( dispositivo, EV_RXCHAR );
	if( status == FALSE ){
		throw runtime_error( "error informacion.");
	}
}

// Obtener peso
void lectorBasculaObtenerPeso()
{
	data.str( "" );
	unsigned int i = 0;

    do{
		status = ReadFile( dispositivo, &tempChar, sizeof( tempChar ), &noBytesRead, NULL );
		if( tempChar != '\r' ){
			serialBuffer[ i ] = tempChar;
			++i;
		}
	}while( tempChar != '\n' && status == TRUE );

	if( status == TRUE ){
    	for( unsigned int j = lectorBascula.obtenerBytesIgnorados(); j < i; ++j ){
        	data << serialBuffer[ j ];
    	}

		// Establece el tamaño en la etiqueta
		interfaz.establecerTextoEtiqueta( "EtiquetaPeso", data.str() );
	}
}

// Cierra la bascula
void lectorBasculaCerrar( GtkWidget *widget, gpointer ptr )
{
	lectorBascula.cerrar();
}
