#ifndef SESION_H
#define SESION_H
#include "Aplicacion.h"
#include <gtk/gtk.h>
#include <string>

extern std::string nombreUsuario;
extern std::string codigoRecuperacion;

void registrarUsuario();
void iniciarSesion();
void sesion_Cerrar( GtkWidget *widget, gpointer ptr );

std::string crearSal();

void verificarContrasena( std::string contrasena, std::string hash );
void cambiarContrasenaUsuario();
void cambiarContrasena( std::string nombreUsuario, std::string contrasena, std::string confirmacion );
bool existenUsuariosRegistrados();

#endif
