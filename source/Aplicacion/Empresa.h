#ifndef EMPRESA_H
#define EMPRESA_H

#include <gtk/gtk.h>
#include <string>

//
void empresa_inicio_senales_conectar();

std::string empresa_validar_razon_social( std::string razon_social );
void empresa_domicilio_agregar( GtkWidget *widget, gpointer info );
void empresa_escribir_imagen( GtkWidget *widget, gpointer info );
void empresa_inicio_registrar( GtkWidget *widget, gpointer info );
void empresa_inicio_imagen( GtkWidget *widget, gpointer info );
void empresa_leer_informacion( void );

void empresa_inicio_imagen_omitir( GtkWidget *widget, gpointer info );
void empresa_inicio_imagen_omitir_confirmacion( GtkWidget *widget, gpointer info );

void empresa_inicio_senales_conectar();
void empresa_senales_desconectar();

void domicilio_validar_formulario( void );
std::string domicilio_validar_lugar( std::string objeto, bool obligatorio );
std::string domicilio_validar_numero( std::string numero, bool obligatorio );
std::string domicilio_validar_descripcion( std::string descripcion );

#endif