#include "Bascula.h"
#include <stdexcept>
#include <sstream>
#include <string>
#include <regex>
#include <iostream>
using namespace std;

HANDLE dispositivo;

Bascula::Bascula():
	codigo( 0 ), nombre( "Bascula" ), puerto( "" ), baudRate( CBR_9600 ), parity( NOPARITY ), 
	stopBits( 0 ), ignoredBytes( 0 ), activo( false ), peso( 0.f )
{
	
}

Bascula::Bascula( unordered_map< string, string > *renglon )
{
	// Se asegura que no se esté intentando establecer un renglón nulo
    if( renglon == nullptr ){
	    throw invalid_argument( "El renglón que se desea asignar está vacío." );
    }

	cout << "Clave: " << (* renglon)[ "clave" ] << endl
        << "Nombre: " << (* renglon)[ "nombre" ] << endl
		<< "Puerto: " << (* renglon)[ "puerto" ] << endl
		<< "Baudrate: " << (* renglon)[ "baudrate" ] << endl
		<< "Bytesize: " << (* renglon)[ "bytesize" ] << endl
		<< "Parity: " << stoi( (* renglon)[ "parity" ] ) << endl
		<< "StopBits: " << (* renglon)[ "stopbits" ] << endl
		<< "Bytes ignorades: " << (* renglon)[ "ignoredbytes" ] << endl << endl;

	// Establece los datos solicitados
	establecerCodigo( (* renglon)[ "clave" ] );
    establecerNombre( (* renglon)[ "nombre" ] );
    establecerPuerto( (* renglon)[ "puerto" ] );
    establecerBaudRate( (* renglon)[ "baudrate" ] );
    establecerByteSize( (* renglon)[ "bytesize" ] );
    establecerParity( stoi( (* renglon)[ "parity" ] ) );
    establecerStopBits( (* renglon)[ "stopbits" ] );
    establecerBytesIgnorados( (* renglon)[ "ignoredbytes" ] );
}

void Bascula::conectar()
{
    dispositivo = CreateFile( obtenerPuerto().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

	if( dispositivo == INVALID_HANDLE_VALUE ){
		throw runtime_error( "No fue encontrado ningún puerto." );
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof( dcbSerialParams );

	BOOL status = GetCommState( dispositivo, &dcbSerialParams );

	dcbSerialParams.BaudRate = obtenerBaudRate();
	dcbSerialParams.ByteSize = obtenerByteSize();
	dcbSerialParams.StopBits = obtenerStopBits();
	dcbSerialParams.Parity = obtenerParity();

	status = SetCommState( dispositivo, &dcbSerialParams );
	if( status == FALSE ){
		throw runtime_error( "No se pudo realizar la conexión.");
	}

	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 10;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;

	status = SetCommTimeouts( dispositivo, &timeout );
	if( status == FALSE ){
		throw runtime_error( "Error al establecer la información de tiempos de espera.");
	}

	status = SetCommMask( dispositivo, EV_RXCHAR );
	if( status == FALSE ){
		throw runtime_error( "Error al configurar los parámetros de recepción.");
	}

    // Indica que la báscula se encuentra activa
    establecerActivo( true );
}

void Bascula::desconectar()
{
    CancelIoEx( dispositivo, NULL );
	CloseHandle( dispositivo );
}

double Bascula::leer() const
{
	return peso;
}

void Bascula::actualizarPeso()
{
    if( !estaActivo() ){
        throw runtime_error( "No se puede leer una báscula inactiva." );
    }
    
    BOOL status = FALSE;
    DWORD noBytesRead;
    char bufferRead[ 256 ];
	char buffer[ 256 ] = "";
    char tempChar = ' ';
	unsigned int i = 0;

    do{
		status = ReadFile( dispositivo, &tempChar, sizeof( tempChar ), &noBytesRead, NULL );
		if( tempChar != '\r' ){
			bufferRead[ i ] = tempChar;
			++i;
		}
	}while( tempChar != '\n' && status == TRUE );

	if( status == TRUE ){
		size_t j = obtenerBytesIgnorados();
		size_t k = 0;

		while( j < i ){
			buffer[ k ] = bufferRead[ j ];
			++j;
			++k;
			buffer[ k ] = '\0';
		}

		// Convierte el peso leído a un número double
    	try{
    		peso = stod( buffer );
    	}
    	catch( invalid_argument &ia ){
    		peso = 0.f;
    	}
	}
}

bool Bascula::estaActivo() const
{
    return activo;
}

void Bascula::establecerActivo( bool estado )
{
    this -> activo = estado;
}

void Bascula::establecerCodigo( string codigo )
{
    try{
        establecerCodigo( stoi( codigo ) );
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "Se ha especificado un código no válido." );
    }
}

void Bascula::establecerCodigo( unsigned int codigo )
{
    try{
        this -> codigo = codigo;
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "Se ha especificado un código no válido." );
    }
} 

unsigned int Bascula::obtenerCodigo() const
{
    return codigo;
}

void Bascula::establecerNombre( string nombre )
{
    // Formato de validación de nombres
    regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );

    // Se asegura de que se hubiera introducido un nombre
    if( nombre.empty() ){
        throw invalid_argument( "Es necesario establecer un nombre identificativo para la báscula." );
    }

    // Se asegura que el nombre asignado a la báscula sea menor o igual a 50
    if( nombre.size() > 50 ){
        throw invalid_argument( "El nombre de identificativo de la báscula no debe superar los cincuenta caracteres." );
    }

    // Se asegura que no se introduzcan caracteres inválidos en el nombre de usuario
    if( !regex_match( nombre, formato ) ){
        throw std::invalid_argument( "El nombre introducido no es válido." );
    }

    // Establece el nombre de la báscula
    this -> nombre = nombre;
}

string Bascula::obtenerNombre() const
{
    return nombre;
}

void Bascula::establecerPuerto( const string &puerto )
{
	// La opción elegida no es la que dice seleccionar
	if( nombre.compare( "Seleccionar" ) == 0 ){
		throw invalid_argument( "Debe seleccionar una opción válida." );
	}

	// El puerto seleccionado por alguna razón supera los 256 caracteres
    if( puerto.size() > 256 ){
        throw invalid_argument( "El puerto indicado es muy largo." );
    }
    this -> puerto = puerto;
}

string Bascula::obtenerPuerto() const
{
    return puerto;
}

void Bascula::establecerBaudRate( const string baudRateStr )
{
    try{
        establecerBaudRate( stoi( baudRateStr ) );
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "El baudrate especificado no es válido." );
    }
}

void Bascula::establecerBaudRate( DWORD baudRate )
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
			throw invalid_argument( "El baudrate especificado no es válido." );
			break;
	}
}

DWORD Bascula::obtenerBaudRate() const
{
    return baudRate;
}

void Bascula::establecerByteSize( string byteSize )
{
    try{
        establecerByteSize( stoi( byteSize ) );
    }
    catch( invalid_argument &ia ){
        throw invalid_argument( "El tamaño de datos que se desea establecer no es válido.\nEste debe ser un número entre cinco y ocho." );
    }
}

void Bascula::establecerByteSize( BYTE byteSize )
{
    if( byteSize < 5 || byteSize > 8 ){
		throw invalid_argument( "El tamaño de datos que se desea establecer no es válido.\nEste debe ser un número entre cinco y ocho." );
	}

	this -> byteSize = byteSize;
}

BYTE Bascula::obtenerByteSize() const
{
	return byteSize;
}

void Bascula::establecerParity( string parityStr )
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

void Bascula::establecerParity( BYTE parity )
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

BYTE Bascula::obtenerParity() const
{
	return parity;
}

void Bascula::establecerStopBits( string stopBits )
{
	try{
		establecerStopBits( stoi( stopBits ) );
	}
	catch( invalid_argument &ia ){
		throw invalid_argument( "Solo es posible establecer uno o dos bits de stop." );
	}
}

void Bascula::establecerStopBits( BYTE stopBits )
{
	if( stopBits < 0 || stopBits > 2 ){
		throw invalid_argument( "Solo es posible establecer uno o dos bits de stop." );
	}

	this -> stopBits = stopBits;
}

BYTE Bascula::obtenerStopBits() const
{
	return stopBits;
}

void Bascula::establecerBytesIgnorados( string bytesIgnorados )
{
	try{
		establecerBytesIgnorados( stoi( bytesIgnorados ) );
	}
	catch( invalid_argument &ia ){
		throw invalid_argument( "El numero de bytes a ignorar no es válido." );
	}
}

void Bascula::establecerBytesIgnorados( unsigned int bytesIgnorados )
{
	if( bytesIgnorados < 0 || bytesIgnorados > 10 ){
		throw invalid_argument( "El número de bytes ignorados no puede superar los 10 bytes." );
	}

	this -> ignoredBytes = bytesIgnorados;
}

unsigned int Bascula::obtenerBytesIgnorados() const
{
	return ignoredBytes;
}
