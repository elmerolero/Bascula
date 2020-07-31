#ifndef EMPRESA_H
#define EMPRESA_H
#include <string>
#include "Domicilio.h"

class Empresa
{
    public:
        Empresa();
        
        /* Manipuladores de acceso */
        // Codigo empresa
        void establecerCodigoEmpresa( int codigoEmpresa );
        int obtenerCodigoEmpresa() const;

        // Razón social
        void establecerRazonSocial( std::string razonSocial );
        std::string obtenerRazonSocial() const;

        // Domicilio
        void establecerDomicilio( Domicilio domicilio );
        Domicilio obtenerDomicilio() const;
    
    private:
        int codigoEmpresa;
        std::string razonSocial;
        Domicilio domicilio;
};

#endif

