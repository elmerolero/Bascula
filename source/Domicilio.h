#ifndef DOMICILIO_H
#define DOMICILIO_H
#include <string>

class Domicilio
{
    public:
        Domicilio();

        /* Manipuladores de acceso */
        // Calle
        void establecerCalle( std::string calle );
        std::string obtenerCalle() const;

        // Número
        void establecerNumero( int numero );
        int obtenerNumero() const;

        // Número Interior
        void establecerNumeroInterior( int numeroInterior );
        int obtenerNumeroInterior() const;

        // Colonia
        void establecerColonia( std::string colonia );
        std::string obtenerColonia() const;

        // Código postal
        void establecerCodigoPostal( std::string codigoPosal );
        std::string obtenerCodigoPostal() const;

        // Municipio
        void establecerMunicipio( std::string municipio );
        std::string obtenerMunicipio() const;

        // Estado
        void establecerEstado( std::string estado );
        std::string obtenerEstado() const;

        // País
        void establecerPais( std::string pais );
        std::string obtenerPais() const;

        // Obtiene el domicilio en formato String
        std::string obtenerDomicilio() const;

    private:
        std::string calle;
        int numero;
        int numeroInterior;
        std::string colonia;
        std::string codigoPostal;
        std::string municipio;
        std::string estado;
        std::string pais;
};

#endif
