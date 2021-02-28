#ifndef REGISTRO_H
#define REGISTRO_H
#include <string>

class Registro
{
	public:
		Registro();
		~Registro();
	
		// Clave
		void establecerClave( unsigned int clave );
		unsigned int obtenerClave() const;
		
		// Nombre
		void establecerNombre( std::string nombre );
		std::string obtenerNombre() const;
		
	private:
		unsigned int clave;
		std::string nombre;
};

#endif
