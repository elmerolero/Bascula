#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <regex>
#include <unordered_map>
#include <stdexcept>
#include <gtk/gtk.h>
#include "Widget.h"

extern Signal enlaceCuenta;

class Usuario
{
    public:
        // Constructor
        Usuario();

        // Constructor con unordered_map como renglon de una base de datos
        Usuario( std::unordered_map< std::string, std::string > *renglon );

        /* Modificadores de acceso */
        // Folio
		void establecerClave( unsigned int clave );
		unsigned int obtenerClave() const;

        // Nombre de usuario
        void establecerNombreUsuario( std::string nombreUsuario );
        std::string obtenerNombreUsuario() const;
        static std::string validarNombreUsuario( std::string nombreUsuarioValidar );

        // Nombre de la persona
        void establecerNombre( std::string nombre );
        std::string obtenerNombre() const;
        static std::string validarNombre( std::string nombreValidar );

        // Apellidos
        void establecerApellidos( std::string apellidos );
        std::string obtenerApellidos() const;
        static std::string validarApellidos( std::string apellidosValidar );

        // Hash de contrasena
        void establecerHash( std::string hash );
        std::string obtenerHash() const;
        
        // Aspectos relacionados a la contraseña
        static std::string validarContrasena( std::string contrasena );
        static void compararContrasenas( std::string contrasena, std::string confirmacion );

        // Permisos de administrador
        void establecerAdministrador( bool permiso );
        bool esAdministrador() const;

    private:
        unsigned int clave;
        std::string nombreUsuario;
        std::string nombre;
        std::string apellidos;
        std::string hash;
        bool administrador;
};

// Usuario
extern Usuario usuario;

void usuario_cuenta_leer( GtkWidget *widget, gpointer info );

#endif

