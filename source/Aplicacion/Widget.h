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

        void establecerBuildable( GtkBuilder *buildable );

        // Establece el icon de la aplicacion
        void establecerIconoVentana( std::string idVentana, std::string archivo );

        // Establece el nombre de un widget
        void establecerNombreWidget( std::string idWidget, std::string nombreWidget );

        // Obtiene el nombre de un widget 
        std::string obtenerNombreWidget( std::string idWidget );

        // Conecta un evento al elemento e indica una función que ejecutará
        void conectarSenal( Signal &senal, GCallback funcion, gpointer data );

        // Desconecta la señal del elemento indicado
        void desconectarSenal( Signal &senal );

        // Permite obtener el objeto de un widget indicando su ID
        GObject * obtenerObjeto( std::string id ) const;

        // Inserta un Widget dentro de un Grid que se encuentre en el Widget
        void insertarElementoAGrid( const Widget *elemento, std::string idFuente, std::string idDestino, gint left, gint top, gint width, gint height );

        // Inserta un widget a una lista de cajas
        void insertarElementoListBox( const Widget *elemento, std::string idFuente, std::string idDestino, gint position );
        
        // Obtiene el nombre del Widget seleccionado en un ListBox
        std::string obtenerWidgetSeleccionadoListBox( std::string idListBox );

        // Elimina el Widget dado
        void eliminarElemento( const Widget *elemento, std::string idElemento, std::string idContenedor );
        
        // Establece el texto en la etiqueta indicada
        void establecerTextoEtiqueta( std::string idEtiqueta, std::string texto );
        
        // Obtiene el texto de la etiqueta indicada
        const char *obtenerTextoEtiqueta( std::string idEtiqueta );
        
        // Establece el tamaño máximo de la entrada (cuantos caracteres puede introducir)
        void establecerTamanoEntrada( std::string idEntrada, int tamano );
        
        // Habilita la posibilidad de poder editar la entrada
        void habilitarEdicionEntrada( std::string idEntrada );
        
        // Deshabilita la posibilidad de poder editar la entrada
        void deshabilitarEdicionEntrada( std::string idEntrada );
        
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
        
        // Establece el boton activo
        void establecerActivoBotonToggle( std::string idBoton );
        
        // Obtiene el estado de un toggle button (si esta activo o inactivo)
        bool obtenerEstadoBotonToggle( std::string id );
        
        // Agrega una opcion al combobox text indicado
        void agregarOpcionComboBoxText( std::string idComboBox, std::string opcion, const char *idOpcion );
        
        // Establece la opcion con el id indicado como el objeto activo
        void establecerActivoComboBoxText( std::string idComboBox, std::string idOpcion );
        
        // Obtiene la opcion seleccionada
        const char *obtenerOpcionComboBoxText( std::string idComboBox );
        
        // Establece un opcion activa en el comboBox
        void establecerOpcionComboBox( std::string idComboBox, unsigned int indice );

        // Limpia el combobox text indicado removiendo todas las opciones
        void limpiarComboBoxText( std::string idComboBox );

        // Selecciona una fecha en el calendario
        void establecerFechaCalendario( std::string idCalendario, unsigned int dia, unsigned int mes, unsigned int anio );

        // Obtiene la fecha seleccionada en el calendario
        void obtenerFechaCalendario( std::string idCalendario, unsigned int *dia, unsigned int *mes, unsigned int *anio );
        
        // Permite cargar una imagen desde archivo para un GtkImage
        void establecerImagen( std::string idImagen, std::string archivo );
        
        // Obtiene el ancho de un Widget
        unsigned int obtenerAnchoWidget( std::string idWidget );

        //  Obtiene el alto de un Widget
        unsigned int obtenerAltoWidget( std::string idWidget );

        // Permite obtener el builder de un Widget
        const GtkBuilder *obtenerBuilder() const;
        
        // Remueve los elementos de un contenedor
        void removerElementosHijos( std::string idContenedor );

        void establecerModeloVistaArbol( std::string idVistaArbol, GtkListStore *lista );
        
        // Destruye el widget
        void destruir();

    private:
        GtkBuilder *builder;
};

#endif
