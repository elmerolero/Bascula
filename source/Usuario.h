#ifndef USUARIO_H
#define USUARIO_H

#include <string>

class Usuario
{
    public:
        Usuario();

        /* Modificadores de acceso */
        // Nombre de usuario
        void establecerNombreUsuario( std::string nombreUsuario );
        std::string obtenerNombreUsuario() const;

        // Nombre de la persona
        void establecerNombre( std::string nombre );
        std::string obtenerNombre() const;

        // Apellidos
        void establecerApellidos( std::string apellidos );
        std::string obtenerApellidos() const;

        // Establece los permisos de administrador
        void establecerPermisosAdministrador( bool permiso );
        bool esAdministrador() const;

    private:
        std::string nombreUsuario;
        std::string nombre;
        std::string apellidos;
        bool administrador;
};

#endif

