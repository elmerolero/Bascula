#ifndef PRODUCTO_H
#define PRODUCTO_H

#include "Widget.h"
#include "Imagen.h"
#include <string>

extern Signal senal_producto_imagen_seleccionar;

extern Signal senal_producto_nuevo;
extern Signal senal_producto_guardar_nuevo;
extern Signal senal_producto_cancelar_nuevo;

extern Signal senal_producto_editar;
extern Signal senal_producto_guardar_edicion;
extern Signal senal_producto_cancelar_edicion;
extern Signal senal_producto_eliminar;
extern Signal senal_producto_seleccionar;

extern Signal senal_producto_buscar;

void producto_validar_nombre( std::string nombre );
void producto_listar_registros( GtkWidget *widget, gpointer info );
void producto_nuevo( GtkWidget *widget, gpointer info );
void producto_editar( GtkWidget *widget, gpointer info );
void producto_eliminar( GtkWidget *widget, gpointer info );
void producto_seleccionar( GtkListBox *box, GtkListBoxRow *row, gpointer data );
void producto_escribir_imagen( GtkWidget *widget, gpointer info );
void producto_descartar_cambios( GtkWidget *widget, gpointer info );
void producto_guardar_edicion( GtkWidget *widget, gpointer info );
void producto_cancelar_nuevo( GtkWidget *widget, gpointer info );
void producto_cancelar_edicion();
bool producto_existen_cambios( void );
void producto_actualizar_registros( void );
void producto_conectar_senales( void );
void producto_desconectar_senales( GtkWidget *widget, gpointer info );
void producto_buscar( GtkSearchEntry* entry, GdkEvent* event );

#endif
