#ifndef EMPRESA_H
#define EMPRESA_H

#include "Widget.h"
#include <gtk/gtk.h>
#include <string>

extern Signal senal_empresa_propia_editar;

// Lista de empresas registradas
extern GtkListStore *listaEmpresas;

//
void empresa_inicio_senales_conectar();


void empresa_obtener_registros( void );
void empresa_listar_registros( GtkWidget *widget, gpointer info );
void empresa_actualizar_registros( void );

std::string empresa_validar_razon_social( std::string razon_social );
void empresa_propia_editar( GtkWidget *widget, gpointer info );
void empresa_propia_guardar_edicion( GtkWidget *widget, gpointer info );
void empresa_propia_domicilio_guardar_nuevo( GtkWidget *widget, gpointer info );
void empresa_propia_domicilio_guardar_edicion( GtkWidget *widget, gpointer info );
void empresa_propia_domicilio_editar( GtkWidget *widget, gpointer info );
void empresa_propia_domicilio_cancelar_edicion( GtkWidget *widget, gpointer infor );
void empresa_inicio_registrar( GtkWidget *widget, gpointer info );
void empresa_inicio_imagen( GtkWidget *widget, gpointer info );
void empresa_inicio_imagen_escribir( GtkWidget *widget, gpointer info );
void empresa_imagen_escribir( GtkWidget *widget, gpointer info );
void empresa_leer_informacion( void );

void empresa_inicio_imagen_omitir( GtkWidget *widget, gpointer info );
void empresa_inicio_imagen_omitir_confirmacion( GtkWidget *widget, gpointer info );
void empresa_inicio_domicilio_agregar( GtkWidget *widget, gpointer info );
void empresa_inicio_domicilio_cancelar( GtkWidget *widget, gpointer info );

void empresa_nuevo( GtkWidget *widget, gpointer info );
void empresa_editar( GtkWidget *widget, gpointer info );
void empresa_eliminar( GtkWidget *widget, gpointer info );
void empresa_descartar_cambios( GtkWidget *widget, gpointer info );
bool empresa_existen_cambios( void );
void empresa_cancelar_edicion();
void empresa_guardar_edicion( GtkWidget *widget, gpointer info );
void empresa_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data );

void empresa_inicio_senales_conectar();
void empresa_inicio_senales_desconectar();
void empresa_senales_conectar( void );

std::string empresa_buscar_existente( std::string empresa_nombre );

#endif