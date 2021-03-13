#ifndef VISTAS_H
#define VISTAS_H

#include <gtk/gtk.h>
#include "Registro.h"

void vistaCuenta( GtkWidget *widget, gpointer ptr );
void vistaNuevoRegistro( GtkWidget *widget, gpointer ptr );
void vistaNuevoProducto( GtkWidget *widget, gpointer ptr );
void vistaRegistrosEmpresas( GtkWidget *widget, gpointer ptr );
void vistaRegistroEmpresa( GtkWidget *widget, gpointer ptr );
void vistaRegistrosProductos( GtkWidget *widget, gpointer ptr );
void vistaRegistroProducto( GtkWidget *widget, gpointer ptr );
void vistaRegistro( Registro *registro );
void vistaRegistroEditar( GtkWidget *widget, gpointer ptr );
void vistaConfiguracion( GtkWidget *widget, gpointer ptr );
void vistaBasculaPublica( GtkWidget *widget, gpointer ptr );
void vistaBasculaInterna( GtkWidget *widget, gpointer ptr );
void vistaCrearRegistroPublico( GtkWidget *widget, gpointer ptr );
void vistaCrearRegistro( GtkWidget *widget, gpointer ptr );
void vistaLeerPesoBruto();
void vistaLeerPesoTara();
void vistaInternoEditarRegistro();
void vistaFinalizarRegistroPublico();
void vistaLeerPesoBrutoPublico();
void vistaLeerPesoTaraPublico();
void vistaConsultarPesajesInternos();
void vistaConsultarPesajesPublicos();
void vistaSeleccionarDia();
void vistaObtenerInforme();
void vistaConsultarRegistroInterno();
void vistaConsultarRegistroPublico();
void vistaConsultarUsuarios();
void vistaConsultarUsuario();

void internoLimpiarFormulario();

void regresarInicio();
void regresarUsuarios();

#endif
