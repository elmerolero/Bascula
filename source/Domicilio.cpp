#include "Domicilio.h"
#include <string>
using namespace std;

// Constructor
Domicilio::Domicilio():
    numero( 0 ), numeroInterior( 0 )
{
    // Nada por hacer aquí
}

// Establece el nombre de la calle
void Domicilio::establecerCalle( string calle )
{
    this -> calle = calle;
}

// Obtine el nombre de la calle
string Domicilio::obtenerCalle() const
{
    return calle;
}

// Establece el número
void Domicilio::establecerNumero( int numero )
{
    this -> numero = numero;
}

// Obtiene el número
int Domicilio::obtenerNumero() const
{
    return numero;
}

// Establece el número interior
void Domicilio::establecerNumeroInterior( int numeroInterior )
{
    this -> numeroInterior = numeroInterior;
}

// Obtiene el número interior
int Domicilio::obtenerNumeroInterior() const
{
    return numeroInterior;
}

// Establece la colonia en la que se encuentre la entidad
void Domicilio::establecerColonia( string colonia )
{
    this -> colonia = colonia;
}

// Obtiene la colonia
string Domicilio::obtenerColonia() const
{
    return colonia;
}

//Establece el código postal
void Domicilio::establecerCodigoPostal( string codigoPostal )
{
    this -> codigoPostal = codigoPostal;
}

// Obtiene el código postal
string Domicilio::obtenerCodigoPostal() const
{
    return codigoPostal;
}

// Establece el municipio
void Domicilio::establecerMunicipio( string municipio )
{
    this -> municipio = municipio;
}

// Obtiene el municipio
string Domicilio::obtenerMunicipio() const
{
    return municipio;
}

// Establece el estado
void Domicilio::establecerEstado( string estado )
{
    this -> estado = estado;
}

// Obtener estado
string Domicilio::obtenerEstado() const
{
    return estado;
}

// Establece el país
void Domicilio::establecerPais( string pais )
{
    this -> pais = pais;
}

// Obtiene el país
string Domicilio::obtenerPais() const
{
    return pais;
}

// Obtiene el domicilio en formato string
string Domicilio::obtenerDomicilio() const
{
    // String a devolver
    string domicilio;

    // Concatena la calle,
    domicilio = calle + " " + to_string( obtenerNumero() );

    // el número interior (si es distinto de cero)
    if( numeroInterior != 0 ){
        domicilio = domicilio + " " + to_string( obtenerNumeroInterior() );
    }

    // la colonia,
    domicilio = domicilio + ',' + ' ' + obtenerColonia();
    
    // el código postal,
    domicilio = domicilio + " " + obtenerCodigoPostal();

    // el municipio,
    domicilio = domicilio + ", " + obtenerMunicipio();
    
    // el estado,
    domicilio = domicilio + " " + obtenerEstado();

    // y el país
    domicilio = domicilio + ", " + obtenerPais();

    return domicilio;
}

