#ifndef SENALES_H
#define SENALES_H

#include "Widget.h"

extern Signal ventanaPrincipal;

extern Signal enlaceRegresar;

// 
extern Signal botonInicioRazonSocial;
extern Signal botonRegistros;
extern Signal botonUsuarios;

extern Signal botonRegistrosEmpresas;
extern Signal botonRegistrosProductos;
extern Signal botonRegistrosPesajesInternos;

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
extern Signal botonConfiguracion;

// Enlace hacia los créditos
extern Signal enlaceCreditos;

// Seleccion servicio
extern Signal botonBasculaAdministrativo;
    
// Uso bascula de uso interno
extern Signal entradaSeguimiento;
extern Signal entradaSeguimientoMayusculas;

// Uso bascula de uso interno
extern Signal botonSeguimiento;
extern Signal entradaSeguimientoMayusculas;
extern Signal entradaSeguimiento;

// Nuevo para ticket interno
extern Signal entradaNumeroPlacasInterno;
extern Signal botonCalcularDescuento;
extern Signal opcionDescuentoInterno;
extern Signal opcionRegistraEntrada;

// Vista de registro de peso
extern Signal botonCancelarLectura;
extern Signal ventanaLectorPeso;

// Señales utilizadas para la gestión de usuarios
extern Signal entradaConsultarUsuario;
extern Signal botonConsultarUsuario;
extern Signal botonObtenerCodigoRecuperacion; 
extern Signal usuarioAdministrador;

// Señales para la consulta de registros
extern Signal botonRegistroVistaConsultar;
extern Signal botonRegistroVistaNuevo;
extern Signal botonRegistroVistaEditar;
extern Signal botonRegistroVistaEliminar;

// Señales para la edición de un registro

extern Signal botonRegistroGuardarEdicion;
extern Signal botonRegistroCancelarEdicion;

// Noc
extern Signal imprimirRegistroPublico;

extern Signal botonRegistroGuardarNuevo;
extern Signal botonRegistroCancelarNuevo;
extern Signal botonSi;
extern Signal botonNo;

extern Signal enlaceRegistrosPesajeRegresar;
extern Signal botonRegistrosPesajeObtenerInforme;
extern Signal entradaRegistrosPesajeFolio;
extern Signal botonRegistrosPesajeConsultarRegistro;

extern Signal botonConsultarDia;
extern Signal botonObtenerRegistros;

extern Signal botonEliminarUsuario;
extern Signal botonCambiarContrasena;
extern Signal entradaReemplazarConfirmacion;

#endif