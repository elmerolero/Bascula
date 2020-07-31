#include "Empresa.h"
#include <string>
using namespace std;

// Constructor
Empresa::Empresa():
    codigoEmpresa( 0 )
{
    // Nada que hacer aquí
}

// Establece el código empresa
void Empresa::establecerCodigoEmpresa( int codigoEmpresa )
{
    this -> codigoEmpresa = codigoEmpresa;
}

// Obtiene el código de la empresa
int Empresa::obtenerCodigoEmpresa() const
{
    return codigoEmpresa;
}

// Establece la razón social de la empresa
void Empresa::establecerRazonSocial( std::string razonSocial )
{
    this -> razonSocial = razonSocial;
}

// Obtiene la razón social de la empresa
std::string Empresa::obtenerRazonSocial() const
{
    return razonSocial;
}

// Establece el domicilio
void Empresa::establecerDomicilio( Domicilio domicilio )
{
    this -> domicilio = domicilio;
}

// Obtener el domicilio
Domicilio Empresa::obtenerDomicilio() const
{
    return domicilio;
}

