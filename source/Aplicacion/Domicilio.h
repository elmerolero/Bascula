#ifndef DOMICILIO_H
#define DOMICILIO_H
#include "Senales.h"
#include <string>


extern Signal senal_domicilio_guardar_nuevo;
extern Signal senal_domicilio_cancelar_nuevo;

extern Signal senal_domicilio_empresa_propia_agregar;
extern Signal senal_domicilio_empresa_propia_editar;
extern Signal senal_domicilio_empresa_propia_eliminar;

void domicilio_empresa_propia_eliminar( GtkWidget *widget, gpointer info );
void domicilio_empresa_registros_actualizar( std::string tabla, std::string contenedor, unsigned int id );

void domicilio_limpiar_formulario( void );

std::string domicilio_validar_lugar( std::string objeto, bool obligatorio );
std::string domicilio_validar_numero( std::string numero, bool obligatorio );
std::string domicilio_validar_descripcion( std::string descripcion );

#endif