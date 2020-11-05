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
        static std::string validarNombreUsuario( std::string nombreUsuarioValidar ){
            std::regex formato( "[a-zA-Z0-9_-]*" );
    
            // Se asegura que no se introduzca un nombre de usuario vacío
            if( nombreUsuarioValidar.empty() ){
                 throw std::invalid_argument( "Debe introducirse un nombre de usuario." );
            }
    
            // Se asegura de que el tamaño del nombre de usuario sea de almenos 6 caracteres y menor a 20
            if( nombreUsuarioValidar.size() < 4 || nombreUsuarioValidar.size() > 20 ){
                throw std::invalid_argument( "El numero de caracteres del usuario introducido se encuentra fuera del rango permitido." );
            }
    
            // Se asegura que no se introduzcan caracteres inválidos en el nombre de usuario
            if( !std::regex_match( nombreUsuarioValidar, formato ) ){
                throw std::invalid_argument( "El nombre introducido no es válido." );
            }
            
            return nombreUsuarioValidar;
        }

        // Nombre de la persona
        void establecerNombre( std::string nombre );
        std::string obtenerNombre() const;
        static std::string validarNombre( std::string nombreValidar ){
            // Formato de validación del nombre introducido
            std::regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
            
            // Se asegura que no se introduzca un nombre vacío
            if( nombreValidar.empty() ){
                throw std::invalid_argument( "Es nesesario introducir un nombre." );
            }
            
            // Se asegura de que el nombre introducido no supere el tamaño 
            if( nombreValidar.size() > 35 || !std::regex_match( nombreValidar, formato ) ){
                throw std::invalid_argument( "El nombre introducido no es válido." );
            } 
            
            return nombreValidar;
        }

        // Apellidos
        void establecerApellidos( std::string apellidos );
        std::string obtenerApellidos() const;
        static std::string validarApellidos( std::string apellidosValidar ){
            // Formato de validación del nombre introducido
            std::regex formato( "[a-zA-Z0-9ÑñáéíóúÁÉÍÓÚ\\s]*" );
    
            // Se asegura que no se introduzca un nombre vacío
            if( apellidosValidar.empty() ){
                throw std::invalid_argument( "Es nesesario introducir un apellido." );
            }
            
            // Se asegura de que el nombre introducido no supere el tamaño 
            if( apellidosValidar.size() > 35 || !std::regex_match( apellidosValidar, formato ) ){
                throw std::invalid_argument( "El apellido introducido no es válido." );
            } 
            
            return apellidosValidar;
        }

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
        static std::string validarContrasena( std::string contrasena ){
            // Formato de validación del nombre introducido
            std::regex formato( "[a-zA-Z0-9_\\-#@?*%$&]*" );
            if( contrasena.empty() ){
                throw std::invalid_argument( "Es necesario introducir una contraseña." );
            }
            
            if( contrasena.size() < 6 || !std::regex_match( contrasena, formato ) ){
                throw std::invalid_argument( "La contraseña introducida no es válida. Asegúrate que tenga al menos 6 caracteres." );
            }
            
            return contrasena;
        }

    private:
        std::string nombreUsuario;
        std::string nombre;
        std::string apellidos;
        std::string hash;
        std::string sal;
        bool administrador;
};

#endif

