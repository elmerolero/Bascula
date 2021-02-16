#include "Senales.h"
#include "Widget.h"

// Señales utilizadas para la gestión de usuarios
Signal entradaConsultarUsuario{ "EntradaConsultarUsuario", "activate", 0 }; 
Signal botonConsultarUsuario{ "BotonConsultarUsuario", "clicked", 0 };
Signal enlaceConsultarUsuariosRegresar{ "EnlaceConsultarUsuariosRegresar", "activate-link", 0 };
Signal enlaceConsultarUsuarioRegresar{ "EnlaceConsultarUsuarioRegresar", "activate-link", 0 };
Signal botonObtenerCodigoRecuperacion{ "BotonObtenerCodigoRecuperacion", "clicked", 0 };
Signal usuarioAdministrador{ "UsuarioAdministrador", "toggled", 0 };