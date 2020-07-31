#ifndef WIDGET_H
#define WIDGET_H
#include <string>
#include <gtk/gtk.h>

class Widget
{
    public:
        // Constructor sin argumentos
        Widget();

        // Argumento con la ubicación del archivo de interfaz
        Widget( std::string directorio );

        // Carga la interfaz indicando donde se encuentra el archivo
        void cargarInterfaz( std::string directorio );

        // Asigna conecta un evento al elemento indicado
        void conectarSenal( std::string id, std::string tipoSenal, GCallback funcion, gpointer data );

        // Permite obtener el objeto de un widget indicando su ID
        GObject * obtenerObjeto( std::string id ) const;

        // Busca la etiqueta que esta dentro del Widget y establece el texto
        void establecerTextoEtiqueta( std::string idEtiqueta, std::string texto );

        // Permite insertar un Widget dentre de un grid que se encuentre en el Widget
        void insertarElementoAGrid( const Widget *elemento, std::string idFuente, std::string idDestino, gint left, gint top, gint width, gint height );
        
        // 
        GtkBuilder *obtenerBuilder() const;

        // Destruye el Widget
        void destruir();

    private:
        GtkBuilder *builder;
        GObject *object;
        GError *error;
};

#endif

