#ifndef BASCULA_H
#define BASCULA_H

#include <Windows.h>
#include <unordered_map>
#include <string>

class Bascula
{
    public:
        Bascula( std::unordered_map< std::string, std::string > *renglon );

        void conectar();
        void desconectar();

        void actualizarPeso();
        double leer() const;

        bool estaActivo() const;
        void establecerActivo( bool estado );

        void establecerCodigo( std::string codigoStr );
        void establecerCodigo( unsigned int codigo );
        unsigned int obtenerCodigo() const;

        void establecerNombre( std::string nombre );
        std::string obtenerNombre() const;

        void establecerPuerto( const std::string &puerto );
        std::string obtenerPuerto() const;
    
        void establecerBaudRate( std::string baudRateStr );
		void establecerBaudRate( DWORD baudRate );
		DWORD obtenerBaudRate() const;

		void establecerByteSize( std::string byteSizeStr );
		void establecerByteSize( BYTE byteSize );
		BYTE obtenerByteSize() const;

        void establecerParity( std::string parity );
		void establecerParity( BYTE parity );
		BYTE obtenerParity() const;

		void establecerStopBits( std::string stopBitsStr );
		void establecerStopBits( BYTE stopBits );
		BYTE obtenerStopBits() const;

        void establecerBytesIgnorados( std::string bytesIgnorados );
		void establecerBytesIgnorados( unsigned int bytesIgnorados );
		unsigned int obtenerBytesIgnorados() const;

    private:
        unsigned int codigo;
        std::string nombre;
        std::string puerto;
        DWORD baudRate;
        BYTE byteSize;
        BYTE parity;
        BYTE stopBits;
        unsigned ignoredBytes;
        bool activo;

        double peso;
};

#endif
