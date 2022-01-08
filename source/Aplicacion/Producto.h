#ifndef PRODUCTO_H
#define PRODUCTO_H

#include "Widget.h"
#include "Imagen.h"
#include <string>

void producto_validar_nombre( std::string nombre );
void producto_listar_registros( GtkWidget *widget, gpointer info );
void producto_nuevo( GtkWidget *widget, gpointer info );
void producto_editar( GtkWidget *widget, gpointer info );
void producto_eliminar( GtkWidget *widget, gpointer info );
void producto_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void producto_escribir_imagen( GtkWidget *widget, gpointer info );
void producto_descartar_cambios( GtkWidget *widget, gpointer info );
void producto_guardar_cambios( GtkWidget *widget, gpointer info );
void producto_cancelar_nuevo( GtkWidget *widget, gpointer info );
void producto_cancelar_edicion();
bool producto_existen_cambios( void );
void producto_actualizar_registros( void );
void producto_conectar_senales( void );
void producto_desconectar_senales( GtkWidget *widget, gpointer info );

#endif
