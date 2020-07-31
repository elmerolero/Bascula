#ifndef APLICACION_H
#define APLICACION_H
#include <array>
#include "Widget.h"
#include "Usuario.h"
#include "Empresa.h"
#include "Domicilio.h"

const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

class Aplicacion
{
    public:
        Aplicacion();
        int iniciar();
		Widget *obtenerWidget() const;

    private:
        Usuario usuario();
        Empresa empresa();

        Widget *ventana;
};

#endif

