#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <regex>
#include <stdexcept>

class Usuario
{
    public:
        Usuario();

        /* Modificadores de acceso */
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
        
        // Sal de contrasena
        void establecerSal( std::string sal );
        std::string obtenerSal() const;

        // Permisos de administrador
        void establecerAdministrador( bool permiso );
        bool esAdministrador() const;
        
        // Valida la contrasena del usuario
        static std::string validarContrasena( std::string contrasena );

    private:
        std::string nombreUsuario;
        std::string nombre;
        std::string apellidos;
        std::string hash;
        std::string sal;
        bool administrador;
};

#endif

