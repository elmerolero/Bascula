#ifndef SENALES_H
#define SENALES_H

#include "Widget.h"

// Señales base
extern Signal enlaceRegistrarNuevoUsuario;
extern Signal enlaceRecuperarContrasena;
extern Signal botonIniciarSesion;
extern Signal entradaContrasena;

// Registro de usuario
extern Signal botonRegistrarUsuario;
extern Signal enlaceRegistrarUsuarioRegresar;

// Recuperacion de contraseña
extern Signal enlaceRecuperarContrasenaRegresar;
extern Signal botonReemplazarContrasena;
extern Signal entradaRecuperacionCodigoSeguridad;

// Boton aceptar
extern Signal botonAceptar;

// Vista Inicio
extern Signal botonBascula;
extern Signal botonConfiguracion;

// Enlace hacia los créditos
extern Signal enlaceCreditos;
extern Signal enlaceCreditosRegresar;
    
// Barra de usuario
extern Signal enlaceCuenta;
extern Signal enlaceCuentaRegresar;
extern Signal botonActualizarCuenta;

// Seleccion servicio
extern Signal enlaceTipoBasculaRegresar;
extern Signal botonBasculaPublica;
extern Signal botonBasculaInterna;
    
// Uso bascula de uso interno
extern Signal entradaSeguimiento;
extern Signal entradaSeguimientoMayusculas;
extern Signal enlaceBasculaRegresar;
    
// Nuevo para ticket interno
extern Signal enlaceRegresarInterno;
extern Signal botonLeerPesoBrutoInterno;
extern Signal botonLeerPesoTaraInterno;
extern Signal botonRegistrarPendienteInterno;
extern Signal botonFinalizarPendienteInterno;
extern Signal entradaNumeroPlacasInterno;
extern Signal botonCalcularDescuento;
extern Signal opcionDescuentoInterno;
extern Signal opcionRegistraEntrada;

// Nuevo para ticket publico
extern Signal entradaNumeroPlacasPublico;
extern Signal botonLeerPesoBrutoPublico;
extern Signal botonLeerPesoTaraPublico;
extern Signal botonRegistrarPendientePublico;
extern Signal botonFinalizarPendientePublico;

// Vista de registro de peso
extern Signal botonCancelarLectura;
extern Signal ventanaLectorPeso;
    
// Vista que solicita la contrasena
extern Signal botonPermitirCambios;

// Vista de configuración
extern Signal enlaceConfiguracionRegresar;
extern Signal botonGuardarConfiguracion;

// Consultar registro interno
extern Signal imprimirRegistroInterno;

// Señales utilizadas para la gestión de usuarios
extern Signal entradaConsultarUsuario;
extern Signal botonConsultarUsuario;
extern Signal enlaceConsultarUsuariosRegresar;
extern Signal enlaceConsultarUsuarioRegresar;
extern Signal botonObtenerCodigoRecuperacion; 
extern Signal usuarioAdministrador;

#endif