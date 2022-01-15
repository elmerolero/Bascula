#ifndef WIDGET_H
#define WIDGET_H
#include <string>
#include <vector>
#include <gtk/gtk.h> 

struct Signal
{
    std::string object;
    std::string type;
    guint id;
};

extern GtkBuilder *builder;

//void leer_Widget_seleccionado_listbox( std::string id );
void conectar_senal( Signal &senal, GCallback callback, gpointer data );
void desconectar_senal( Signal &senal );
void mostrar_elemento( std::string id );
void ocultar_elemento( std::string id );
void establecer_texto_entrada( std::string id, std::string texto );
const char *obtener_texto_entrada( std::string id );
void establecer_texto_etiqueta( std::string id, std::string texto );
std::string obtener_nombre_elemento( std::string id );
GObject *buscar_objeto( std::string id );
void limpiar_contenedor( std::string idContendor );

/*************************************************************
 * Guardar los apuntadores GdkPixbuf de los registros para   *
 * liberarlos cuando cuando se remuevan del contenedor dado. *
 *************************************************************/
extern std::vector< GdkPixbuf * > imagenesRegistros;


#endif
