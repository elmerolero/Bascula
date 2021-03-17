#ifndef SENALES_H
#define SENALES_H

#include "Widget.h"

extern Signal enlaceRegresar;

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
    
// Barra de usuario
extern Signal enlaceCuenta;
extern Signal botonActualizarCuenta;

// Seleccion servicio
extern Signal botonBasculaPublica;
extern Signal botonBasculaInterna;
    
// Uso bascula de uso interno
extern Signal entradaSeguimiento;
extern Signal entradaSeguimientoMayusculas;

// Uso bascula de uso interno
extern Signal botonBasculaNuevo;
extern Signal botonSeguimiento;
extern Signal entradaSeguimientoMayusculas;
extern Signal entradaSeguimiento;

// Nuevo para ticket interno
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
extern Signal botonGuardarConfiguracion;

// Consultar registro interno
extern Signal imprimirRegistroInterno;

// Señales utilizadas para la gestión de usuarios
extern Signal entradaConsultarUsuario;
extern Signal botonConsultarUsuario;
extern Signal botonObtenerCodigoRecuperacion; 
extern Signal usuarioAdministrador;

// Señales para la consulta de registros
extern Signal contenedorRegistrosActivado;
extern Signal contenedorRegistrosSeleccionado;
extern Signal botonRegistroNuevo;
extern Signal botonRegistroEditar;
extern Signal botonRegistroEliminar;

// Señales para la edición de un registro

extern Signal botonRegistroGuardarEdicion;
extern Signal botonRegistroCancelarEdicion;

// Noc
extern Signal imprimirRegistroPublico;
extern Signal eliminarRegistro;
extern Signal botonRegistroNuevo;

extern Signal botonRegistroGuardarNuevo;
extern Signal botonRegistroCancelarNuevo;
extern Signal botonSi;

extern Signal enlaceRegistrosPesajeRegresar;
extern Signal botonRegistrosPesajeSeleccionarDia;
extern Signal botonRegistrosPesajeObtenerInforme;
extern Signal entradaRegistrosPesajeFolio;
extern Signal botonRegistrosPesajeConsultarRegistro;

extern Signal botonConsultarDia;
extern Signal botonObtenerRegistrosRango;
extern Signal botonObtenerRegistros;

extern Signal botonEliminarUsuario;
extern Signal botonCambiarContrasena;
extern Signal entradaReemplazarConfirmacion;

extern Signal ticketsPendientesSeleccionados;
extern Signal ticketsConsultadosSeleccionados;

#endif