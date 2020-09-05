#ifndef WIDGET_H
#define WIDGET_H
#include <string>
#include <gtk/gtk.h>

class Widget
{
    public:
        // Constructor sin argumentos
        Widget();
        
        // Destructor
        ~Widget();

        // Argumento con la ubicación del archivo de interfaz
        Widget( std::string directorio );

        // Carga la interfaz indicando donde se encuentra el archivo
        void cargarWidget( std::string archivo );

        // Establece el icon de la aplicacion
        void establecerIconoVentana( std::string idVentana, std::string archivo );

        // Conecta un evento al elemento e indica una función que ejecutará
        guint conectarSenal( std::string id, std::string tipoSenal, GCallback funcion, gpointer data );

        // Desconecta la señal del elemento indicado
        void desconectarSenal( std::string idElemento, guint idSenal );

        // Permite obtener el objeto de un widget indicando su ID
        GObject * obtenerObjeto( std::string id ) const;

        // Inserta un Widget dentro de un Grid que se encuentre en el Widget
        void insertarElementoAGrid( const Widget *elemento, std::string idFuente, std::string idDestino, gint left, gint top, gint width, gint height );
        
        // Elimina el Widget dado
        void eliminarElemento( const Widget *elemento, std::string idElemento, std::string idContenedor );
        
        // Establece el texto en la etiqueta indicada
        void establecerTextoEtiqueta( std::string idEtiqueta, std::string texto );
        
        // Obtiene el texto de la etiqueta indicada
        const char *obtenerTextoEtiqueta( std::string idEtiqueta );
        
        // Establece el tamaño máximo de la entrada (cuantos caracteres puede introducir)
        void establecerTamanoEntrada( std::string idEntrada, int tamano );
        
        // Establece un completador para la entrada dada (osea, para sugerencia dependiendo la entrada)
        void establecerCompletadorEntrada( std::string idEntrada, GtkEntryCompletion *completador );
        
        // Establece el texto de la entrada dada
        void establecerTextoEntrada( std::string idEntrada, std::string texto );
        
        // Obtiene el texto de la entrada dada
        const char *obtenerTextoEntrada( std::string idEntrada );
        
        // Establece una etiqueta en el elemento indicado
        void establecerBotonEtiqueta( std::string idBoton, std::string texto );
        
        // Muestra el widget con el ID indicado
        void mostrarElemento( std::string id ) const;
        
        // Oculta el widget con el ID indicado
        void ocultarElemento( std::string id ) const;
        
        // Permite obtener el builder de un Widget
        const GtkBuilder *obtenerBuilder() const;
        
        // Remueve los elementos de un contenedor
        void removerElementosHijos( std::string idContenedor );
        
        // Destruye el widget
        void destruir();

    private:
        GtkBuilder *builder;
};

#endif
