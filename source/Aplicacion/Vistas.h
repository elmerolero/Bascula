#ifndef VISTAS_H
#define VISTAS_H

#include <gtk/gtk.h>
#include "Registro.h"
#include "ContenedorRegistros.h"

void vistaCuenta( GtkWidget *widget, gpointer ptr );
void vistaRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void vistaRegistroEditar( GtkWidget *widget, gpointer data );
void vistaRegistrosEmpresas( GtkWidget *widget, gpointer ptr );
void vistaRegistrosProductos( GtkWidget *widget, gpointer ptr );
void vistaConfiguracion( GtkWidget *widget, gpointer ptr );
void vistaBasculaPublica( GtkWidget *widget, gpointer ptr );
void vistaBasculaInterna( GtkWidget *widget, gpointer ptr );
void vistaCrearRegistroPublico( GtkWidget *widget, gpointer ptr );
void vistaCrearRegistro( GtkWidget *widget, gpointer ptr );
void vistaLeerPesoBruto();
void vistaLeerPesoTara();
void vistaLeerPesoBrutoPublico();
void vistaLeerPesoTaraPublico();
void vistaConsultarPesajesInternos();
void vistaConsultarPesajesPublicos();
void vistaSeleccionarDia();
void vistaObtenerInforme();
void vistaInternoConsultarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void vistaInternoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void vistaPublicoEditarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void vistaPublicoConsultarRegistro( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void vistaConsultarUsuarios();
void vistaConsultarUsuario();

void internoLimpiarFormulario();

void regresarInicio();
void regresarUsuarios();

#endif
