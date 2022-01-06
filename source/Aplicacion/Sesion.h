#ifndef SESION_H
#define SESION_H
#include "Aplicacion.h"
#include <string>

extern std::string nombreUsuario;
extern std::string codigoRecuperacion;

void registrarUsuario();
void actualizarDatosUsuario();
void iniciarSesion();
void mostrarUsuario();

void nombreUsuarioOcupado( std::string nombreUsuario );
void autorizarCambios();
std::string crearSal();

void verificarContrasena( std::string contrasena, std::string hash );
void cambiarContrasenaUsuario();
void cambiarContrasena( std::string nombreUsuario, std::string contrasena, std::string confirmacion );
bool existenUsuariosRegistrados();

#endif
