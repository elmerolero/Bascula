#include "Senales.h"
#include "Widget.h"

/* SEÑALES BASE (son necesarias para iniciar en el programa */
Signal enlaceRegresar{ "EnlaceRegresar", "activate-link", 0 };

// Registro de usuarios e inicio de sesión
Signal enlaceRegistrarNuevoUsuario{ "EnlaceRegistrarNuevoUsuario", "activate-link", 0 };
Signal enlaceRecuperarContrasena{ "EnlaceRecuperarContrasena", "activate-link", 0 };
Signal botonIniciarSesion{ "BotonIniciarSesion", "clicked", 0 };
Signal entradaContrasena{ "EntradaContrasena", "activate", 0 };

// Registro de usuario
Signal botonRegistrarUsuario{ "BotonRegistrarUsuario", "clicked", 0 };
Signal enlaceRegistrarUsuarioRegresar{ "EnlaceRegistrarUsuarioRegresar", "activate-link", 0 };

// Recuperacion de contraseña
Signal enlaceRecuperarContrasenaRegresar{ "EnlaceRecuperarContrasenaRegresar", "activate-link", 0 };
Signal botonReemplazarContrasena{ "BotonReemplazarContrasena", "clicked", 0 };
Signal entradaRecuperacionCodigoSeguridad{ "EntradaRecuperacionCodigoSeguridad", "activate", 0 };

// Boton aceptar
Signal botonAceptar{ "BotonAceptar", "clicked", 0 };

/* SEÑALES PARA EL MODO BASCULISTA */
// Vista Inicio
Signal botonBascula{ "BotonBascula", "clicked", 0 };
Signal botonConfiguracion{ "BotonConfiguracion", "clicked", 0 };

// Enlace hacia los créditos
Signal enlaceCreditos{ "EnlaceCreditos", "activate-link", 0 };
Signal enlaceCreditosRegresar{ "EnlaceCreditosRegresar", "activate-link", 0 };
    
// Barra de usuario
Signal enlaceCuenta{ "EnlaceCuenta", "activate-link", 0 };
Signal enlaceCuentaRegresar{ "EnlaceCuentaRegresar", "activate-link", 0 };
Signal botonActualizarCuenta{ "BotonActualizarCuenta", "clicked", 0 };

// Seleccion servicio
Signal enlaceTipoBasculaRegresar{ "EnlaceTipoBasculaRegresar", "activate-link", 0 };
Signal botonBasculaPublica{ "BotonBasculaPublica", "clicked", 0 };
Signal botonBasculaInterna{ "BotonBasculaInterna", "clicked", 0 };
    
// Uso bascula de uso interno
Signal botonBasculaNuevo{ "BotonBasculaNuevo", "clicked", 0 };
Signal botonSeguimiento{ "BotonSeguimiento", "clicked", 0 };
Signal entradaSeguimientoMayusculas{ "EntradaSeguimiento", "insert-text", 0 };
Signal entradaSeguimiento{ "EntradaSeguimiento", "activate", 0 };
    
// Nuevo para ticket interno
Signal botonLeerPesoBrutoInterno{ "BotonLeerPesoBrutoInterno", "clicked", 0 };
Signal botonLeerPesoTaraInterno{ "BotonLeerPesoTaraInterno", "clicked", 0 };
Signal botonRegistrarPendienteInterno{ "BotonRegistrarPendienteInterno", "clicked", 0 };
Signal botonFinalizarPendienteInterno{ "BotonFinalizarPendienteInterno", "clicked", 0 };
Signal entradaNumeroPlacasInterno{ "EntradaNumeroPlacasInterno", "insert-text", 0 };
Signal botonCalcularDescuento{ "BotonCalcularDescuento", "clicked", 0 };
Signal opcionDescuentoInterno{ "NoDescuentoInterno", "toggled", 0 };
Signal opcionRegistraEntrada{ "RegistraEntrada", "toggled", 0 };

// Nuevo para ticket publico
Signal entradaNumeroPlacasPublico{ "EntradaNumeroPlacasPublico", "insert-text", 0 };
Signal botonLeerPesoBrutoPublico{ "BotonLeerPesoBrutoPublico", "clicked", 0 };
Signal botonLeerPesoTaraPublico{ "BotonLeerPesoTaraPublico", "clicked", 0 };
Signal botonRegistrarPendientePublico{ "BotonRegistrarPendientePublico", "clicked", 0 };
Signal botonFinalizarPendientePublico{ "BotonFinalizarPendientePublico", "clicked", 0 };

// Vista de registro de peso
Signal botonCancelarLectura{ "BotonCancelarLectura", "clicked", 0 };
Signal ventanaLectorPeso{ "VentanaLectorPeso", "destroy", 0 };
    
// Vista que solicita la contrasena
Signal botonPermitirCambios{ "BotonPermitirCambios", "clicked", 0 };

// Vista de configuración
Signal botonGuardarConfiguracion{ "BotonGuardarConfiguracion", "clicked", 0 };

// Consultar registro interno
Signal imprimirRegistroInterno{ "ImprimirRegistroInterno", "clicked", 0 };

// Señales utilizadas para la gestión de usuarios
Signal entradaConsultarUsuario{ "EntradaConsultarUsuario", "activate", 0 }; 
Signal botonConsultarUsuario{ "BotonConsultarUsuario", "clicked", 0 };
Signal enlaceConsultarUsuariosRegresar{ "EnlaceConsultarUsuariosRegresar", "activate-link", 0 };
Signal enlaceConsultarUsuarioRegresar{ "EnlaceConsultarUsuarioRegresar", "activate-link", 0 };
Signal botonObtenerCodigoRecuperacion{ "BotonObtenerCodigoRecuperacion", "clicked", 0 };
Signal usuarioAdministrador{ "UsuarioAdministrador", "toggled", 0 };

// Noc
Signal imprimirRegistroPublico{ "ImprimirRegistroPublico", "clicked", 0 };
Signal eliminarRegistro{ "EliminarRegistroInterno", "clicked", 0 };
Signal botonNuevoRegistro{ "BotonNuevoRegistro", "clicked", 0 };
Signal botonNuevoRegistro2{ "BotonNuevoRegistro2", "clicked", 0 };
Signal entradaConsultarNombre{ "EntradaConsultarNombre", "activate", 0 };
Signal botonConsultarNombre{ "BotonConsultarNombre", "clicked", 0 };

Signal botonGuardarEdicionRegistro{ "BotonGuardarEdicionRegistro", "clicked", 0 };
Signal botonGuardarNuevoRegistro{ "BotonGuardarNuevoRegistro", "clicked", 0 };
Signal botonCancelarNuevoRegistro{ "BotonCancelarNuevoRegistro", "clicked", 0 };
Signal botonSi{ "BotonSi", "clicked", 0 };

Signal botonRegistrosPesajeSeleccionarDia{ "BotonRegistrosPesajeSeleccionarDia", "clicked", 0 };
Signal botonRegistrosPesajeObtenerInforme{ "BotonRegistrosPesajeObtenerInforme", "clicked", 0 };
Signal entradaRegistrosPesajeFolio{ "EntradaRegistrosPesajeFolio", "activate", 0 };
Signal botonRegistrosPesajeConsultarRegistro{ "BotonRegistrosPesajeConsultarRegistro", "clicked" };

Signal botonConsultarDia{ "BotonConsultarDia", "clicked", 0 };
Signal botonObtenerRegistrosRango{ "BotonObtenerRegistrosRango", "clicked", 0 };
Signal botonObtenerRegistros{ "BotonObtenerRegistros", "clicked", 0 };

Signal botonEliminarUsuario{ "BotonEliminarUsuario", "clicked", 0 };
Signal botonCambiarContrasena{ "BotonCambiarContrasena", "clicked", 0 };
Signal entradaReemplazarConfirmacion{ "EntradaReemplazarConfirmacion", "activate", 0 };
