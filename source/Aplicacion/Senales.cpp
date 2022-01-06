#include "Senales.h"
#include "Aplicacion.h"
#include "Sesion.h"
#include "Vistas.h"
#include "GestorBasculas.h"
#include "Funciones.h"
#include "GestorRegistros.h"
#include "RegistrosInternos.h"
#include "RegistrosPublicos.h"
#include "Producto.h"
#include "Widget.h"

Signal ventanaPrincipal{ "VentanaPrincipal", "destroy", G_CALLBACK( gtk_widget_destroy ), 0 };
Signal botonInicioRazonSocial{ "BotonRegistrarEmpresaPropia", "clicked", G_CALLBACK( empresa_inicio_registrar ), 0 };

Signal botonRegistros{ "BotonRegistros", "clicked", G_CALLBACK( irHacia ), 0 };
Signal botonUsuarios{ "BotonUsuarios", "clicked", G_CALLBACK( vistaConsultarUsuarios ), 0 };

Signal botonRegistrosEmpresas{ "BotonRegistrosEmpresas", "clicked", G_CALLBACK( vista_registros ), 0 };
Signal botonRegistrosProductos{ "BotonRegistrosProductos", "clicked", G_CALLBACK( producto_listar_registros ), 0 };
Signal botonRegistrosPesajesInternos{ "BotonRegistrosPesajesInternos", "clicked", G_CALLBACK( vistaConsultarPesajesInternos ), 0 };
Signal botonRegistrosBasculaPublica{ "BotonRegistrosBasculaPublica", "clicked", G_CALLBACK( vistaConsultarPesajesPublicos ), 0 };

/* SEÑALES BASE (son necesarias para iniciar en el programa */
Signal enlaceRegresar{ "EnlaceRegresar", "activate-link", G_CALLBACK( regresarVista ), 0 };

// Registro de usuarios e inicio de sesión
Signal enlaceRegistrarNuevoUsuario{ "EnlaceRegistrarNuevoUsuario", "activate-link", G_CALLBACK( irHacia ), 0 };
Signal enlaceRecuperarContrasena{ "EnlaceRecuperarContrasena", "activate-link", G_CALLBACK( irHacia ), 0 };
Signal botonIniciarSesion{ "BotonIniciarSesion", "clicked", iniciarSesion, 0 };
Signal entradaContrasena{ "EntradaContrasena", "activate", iniciarSesion, 0 };

// Registro de usuario
Signal botonRegistrarUsuario{ "BotonRegistrarUsuario", "clicked", registrarUsuario, 0 };
Signal enlaceRegistrarUsuarioRegresar{ "EnlaceRegistrarUsuarioRegresar", "activate-link", 0 };

// Recuperacion de contraseña
Signal enlaceRecuperarContrasenaRegresar{ "EnlaceRecuperarContrasenaRegresar", "activate-link", 0 };
Signal botonReemplazarContrasena{ "BotonReemplazarContrasena", "clicked", G_CALLBACK( validarCodigoRecuperacion ), 0 };
Signal entradaRecuperacionCodigoSeguridad{ "EntradaRecuperacionCodigoSeguridad", "activate", G_CALLBACK( validarCodigoRecuperacion ), 0 };

// Boton aceptar
Signal botonAceptar{ "BotonAceptar", "clicked", G_CALLBACK( app_aceptar_mensaje ), 0 };

/* SEÑALES PARA EL MODO BASCULISTA */
// Vista Inicio
Signal botonBascula{ "BotonBascula", "clicked", G_CALLBACK( irHacia ), 0 };
Signal botonConfiguracion{ "BotonConfiguracion", "clicked", G_CALLBACK( vistaConfiguracion ), 0 };

// Enlace hacia los créditos
Signal enlaceCreditos{ "EnlaceCreditos", "activate-link", G_CALLBACK( irHacia ), 0 };
Signal enlaceCreditosRegresar{ "EnlaceCreditosRegresar", "activate-link", 0 };
    
// Barra de usuario
Signal enlaceCuenta{ "EnlaceCuenta", "activate-link", G_CALLBACK( vistaCuenta ), 0 };
Signal enlaceCuentaRegresar{ "EnlaceCuentaRegresar", "activate-link", 0 };
Signal botonActualizarCuenta{ "BotonActualizarCuenta", "clicked", G_CALLBACK( autorizarCambios ), 0 };

// Seleccion servicio
Signal enlaceTipoBasculaRegresar{ "EnlaceTipoBasculaRegresar", "activate-link", 0 };
Signal botonBasculaPublica{ "BotonBasculaPublica", "clicked", G_CALLBACK( vistaBasculaPublica ), 0 };
Signal botonBasculaInterna{ "BotonBasculaInterna", "clicked", G_CALLBACK( vistaBasculaInterna ), 0 };
    
// Uso bascula de uso interno
Signal botonBasculaNuevo{ "BotonBasculaNuevo", "clicked", 0 };
Signal botonSeguimiento{ "BotonSeguimiento", "clicked", 0 };
Signal entradaSeguimientoMayusculas{ "EntradaSeguimiento", "insert-text", 0 };
Signal entradaSeguimiento{ "EntradaSeguimiento", "activate", 0 };
    
// Nuevo para ticket interno
Signal botonLeerPesoBrutoInterno{ "BotonLeerPesoBrutoInterno", "clicked", G_CALLBACK( vistaLeerPesoBruto ), 0 };
Signal botonLeerPesoTaraInterno{ "BotonLeerPesoTaraInterno", "clicked", G_CALLBACK( vistaLeerPesoTara ), 0 };
Signal botonRegistrarPendienteInterno{ "BotonRegistrarPendienteInterno", "clicked", G_CALLBACK( internoRegistrarPendiente ), 0 };
Signal botonFinalizarPendienteInterno{ "BotonFinalizarPendienteInterno", "clicked", G_CALLBACK( internoFinalizarPendiente ), 0 };
Signal entradaNumeroPlacasInterno{ "EntradaNumeroPlacasInterno", "insert-text", G_CALLBACK( convertirMayusculas ), 0 };
Signal botonCalcularDescuento{ "BotonCalcularDescuento", "clicked", G_CALLBACK( internoActualizarPesoNeto ), 0 };
Signal opcionDescuentoInterno{ "NoDescuentoInterno", "toggled", G_CALLBACK( internoHabilitarDescuento ), 0 };
Signal opcionRegistraEntrada{ "RegistraEntrada", "toggled", G_CALLBACK( internoSeleccionarTipo ), 0 };

// Nuevo para ticket publico
Signal entradaNumeroPlacasPublico{ "EntradaNumeroPlacasPublico", "insert-text", G_CALLBACK( convertirMayusculas ), 0 };
Signal botonLeerPesoBrutoPublico{ "BotonLeerPesoBrutoPublico", "clicked", G_CALLBACK( vistaLeerPesoBrutoPublico ), 0 };
Signal botonLeerPesoTaraPublico{ "BotonLeerPesoTaraPublico", "clicked", G_CALLBACK( vistaLeerPesoTaraPublico ), 0 };
Signal botonRegistrarPendientePublico{ "BotonRegistrarPendientePublico", "clicked", G_CALLBACK( publicoRegistrarPendiente ), 0 };
Signal botonFinalizarPendientePublico{ "BotonFinalizarPendientePublico", "clicked", G_CALLBACK( publicoFinalizarPendiente ), 0 };
    
// Vista que solicita la contrasena
Signal botonPermitirCambios{ "BotonPermitirCambios", "clicked", G_CALLBACK( actualizarDatosUsuario ), 0 };

// Vista de configuración
Signal botonGuardarConfiguracion{ "BotonGuardarConfiguracion", "clicked",  G_CALLBACK( guardarConfiguracion ), 0 };

// Consultar registro interno
Signal imprimirRegistroInterno{ "ImprimirRegistroInterno", "clicked", G_CALLBACK( internoImprimirSeleccionado ), 0 };

// Señales utilizadas para la gestión de usuarios
Signal entradaConsultarUsuario{ "EntradaConsultarUsuario", "activate", G_CALLBACK( vistaConsultarUsuario ), 0 }; 
Signal botonConsultarUsuario{ "BotonConsultarUsuario", "clicked", G_CALLBACK( vistaConsultarUsuario ), 0 };
Signal enlaceConsultarUsuariosRegresar{ "EnlaceConsultarUsuariosRegresar", "activate-link", 0 };
Signal enlaceConsultarUsuarioRegresar{ "EnlaceConsultarUsuarioRegresar", "activate-link", 0 };
Signal botonObtenerCodigoRecuperacion{ "BotonObtenerCodigoRecuperacion", "clicked", G_CALLBACK( generarCodigoRecuperacion ), 0 };
Signal usuarioAdministrador{ "UsuarioAdministrador", "toggled", G_CALLBACK( actualizarEstadoAdministrador ), 0 };

// Señales para la consulta de registros 
Signal botonRegistroVistaConsultar{ "ContenedorRegistros", "row-activated", 0, 0 };
Signal botonRegistroVistaNuevo{ "BotonRegistroNuevo", "clicked", G_CALLBACK( irHacia ), 0 };
Signal botonRegistroVistaEditar{ "BotonRegistroEditar", "clicked", G_CALLBACK( vistaRegistroEditar ), 0 };
Signal botonRegistroVistaEliminar{ "BotonRegistroEliminar", "clicked", G_CALLBACK( vistaRegistroEliminar ), 0 };

// Boton de edición de registro
Signal botonRegistroGuardarEdicion{ "BotonRegistroGuardarEdicion", "clicked", G_CALLBACK( actualizarRegistro ), 0 };
Signal botonRegistroCancelarEdicion{ "BotonRegistroCancelarEdicion", "clicked", G_CALLBACK( regresarVista ), 0 };

// Noc
Signal imprimirRegistroPublico{ "ImprimirRegistroPublico", "clicked", 0 };
Signal eliminarRegistroInterno{ "EliminarRegistroInterno", "clicked", G_CALLBACK( internoAlertaEliminar ), 0 };
Signal eliminarRegistroPublico{ "EliminarRegistroPublico", "clicked", G_CALLBACK( internoAlertaEliminar ), 0 };
Signal botonRegistroGuardarNuevo{ "BotonRegistroGuardarNuevo", "clicked", G_CALLBACK( crearRegistro ), 0 };
Signal botonRegistroCancelarNuevo{ "BotonRegistroCancelarNuevo", "clicked", 0 };
Signal botonSi{ "BotonSi", "clicked", G_CALLBACK( eliminarRegistro ), 0 };
Signal botonNo{ "BotonNo", "clicked", G_CALLBACK( cancelarAccion ), 0 };

Signal botonRegistrosPesajeSeleccionarDia{ "BotonRegistrosPesajeSeleccionarDia", "clicked", 0 };
Signal botonRegistrosPesajeObtenerInforme{ "BotonRegistrosPesajeObtenerInforme", "clicked", 0 };
Signal entradaRegistrosPesajeFolio{ "EntradaRegistrosPesajeFolio", "activate", 0 };
Signal botonRegistrosPesajeConsultarRegistro{ "BotonRegistrosPesajeConsultarRegistro", "clicked" };

Signal botonConsultarDia{ "BotonConsultarDia", "clicked", 0 };
Signal botonObtenerRegistrosRango{ "BotonObtenerRegistrosRango", "clicked", 0 };
Signal botonObtenerRegistros{ "BotonObtenerRegistros", "clicked", 0 };

Signal botonEliminarUsuario{ "BotonEliminarUsuario", "clicked", 0 };
Signal botonCambiarContrasena{ "BotonCambiarContrasena", "clicked", G_CALLBACK( cambiarContrasenaUsuario ), 0 };
Signal entradaReemplazarConfirmacion{ "EntradaReemplazarConfirmacion", "activate", G_CALLBACK( cambiarContrasenaUsuario ), 0 };

Signal ticketsPendientesSeleccionados{ "ContenedorTickets", "row-activated", 0 };
Signal ticketsConsultadosSeleccionados{ "ContenedorRegistrosPesaje", "row-activated", 0 };

// Vista de las basculas
Signal basculaSeleccionada{ "ContenedorBasculas", "row-activated",  G_CALLBACK( vistaBascula ), 0 };
Signal botonBasculaAgregar{ "BotonBasculaAgregar", "clicked", G_CALLBACK( vistaBasculaEdicion ), 0 };
Signal botonBasculaEditar{ "BotonBasculaEditar", "clicked", 0 };
Signal botonBasculaEliminar{ "BotonBasculaEliminar", "clicked", G_CALLBACK( vistaBasculaEliminar ), 0 };

// Basculas Agregar/Editar
Signal botonGuardarBascula{ "BotonGuardarBascula", "clicked", 0 };

// Ventana lector peso
Signal ventanaLectorPeso{ "VentanaLectorPeso", "delete-event", G_CALLBACK( basculaCerrarLector ), 0 };
Signal comboBoxBasculasRegistradas{ "BasculasRegistradas", "changed", 0 };
Signal botonCancelarPeso{ "BotonCancelarLectura", "clicked", G_CALLBACK( basculaCerrarLector ), 0 };
Signal botonRegistrarPeso{ "BotonRegistrarLectura", "clicked", 0 };

