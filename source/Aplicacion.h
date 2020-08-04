#ifndef APLICACION_H
#define APLICACION_H
#include <array>
#include "Widget.h"
#include "Usuario.h"
#include "Empresa.h"
#include "Domicilio.h"

const std::array< std::string, 12 > meses{ "enero", "febrero", "marzo", "abril", "mayo", "junio", "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };

void establecerUsuario( GtkWidget *widget, gpointer ptr );

class Aplicacion
{
    public:
        Aplicacion();
        int iniciar();
        Widget *obtenerWidget() const;
        void autenticarUsuario( const std::string nombreUsuario, const std::string contrasena );

    private:
        Usuario usuario();
        Empresa empresa();

        Widget *ventana;
};

#endif

