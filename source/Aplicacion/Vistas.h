#ifndef VISTAS_H
#define VISTAS_H

#include <gtk/gtk.h>
#include "Registro.h"
#include "ContenedorRegistros.h"

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

void vistaBasculaEliminar();

void vistaBasculaEdicion();

void internoLimpiarFormulario();

void regresarInicio();
void regresarUsuarios();

void vistaBascula( GtkListBox *box, GtkListBoxRow *row, gpointer data );

#endif
