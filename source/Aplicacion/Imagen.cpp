#include "Imagen.h"
#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cmath>
using namespace std;

ImagenCairo imagen;             // Imagen original
ImagenCairo imagen_escalada;    // Imagen transformada
string nombreTemporal;
cairo_surface_t *imagen_temporal = nullptr;

int imagen_posicion_x = 0;
int imagen_posicion_y = 0;
int imagen_ultimo_x = 0;
int imagen_ultimo_y = 0;

guint senal_presionado = 0;
guint senal_movimiento = 0;
guint senal_dibujado = 0;

Signal senal_imagen_guardar_edicion = { "BotonGuardarEdicionImagen", "clicked", 0 };
Signal senal_imagen_cancelar_edicion = { "BotonCancelarEdicionImagen", "clicked", 0 };
guint senal_imagen_descartar = 0;
guint senal_imagen_continuar = 0;
guint senal_imagen_omitir = 0;

void seleccionar_archivo( GtkWidget *widget, gpointer data ){
    GtkWindow *ventana = GTK_WINDOW( gtk_builder_get_object( builder, "VentanaPrincipal" ) );
    GtkWidget *dialogo = gtk_file_chooser_dialog_new( "Seleccionar imagen", ventana, GTK_FILE_CHOOSER_ACTION_OPEN,  ("_Cancelar"), GTK_RESPONSE_CANCEL, ("_Abrir"), GTK_RESPONSE_ACCEPT, NULL );
    gint res;

    try{
        // 
        res = gtk_dialog_run( GTK_DIALOG( dialogo ) );
        if( res == GTK_RESPONSE_ACCEPT ){
            char *nombreArchivo;
            GtkFileChooser *chooser = GTK_FILE_CHOOSER( dialogo );
            nombreArchivo = gtk_file_chooser_get_filename( chooser );
            imagen_abrir( &imagen, nombreArchivo );
            g_free( nombreArchivo );
        }
        else{
            // Oculta la ventana
            GObject *objeto = gtk_builder_get_object( builder, "VentanaEdicionImagen" );
            gtk_widget_hide( GTK_WIDGET( objeto ) );
            gtk_widget_destroy( dialogo );
            return;
        }
        gtk_widget_destroy( dialogo );

        imagen_escalada = imagen;

        // Obtiene el área de dibujo, establece su tamaño y la función de dibujo
        GObject *objeto = gtk_builder_get_object( builder, "AreaDibujo" );
        gtk_widget_set_size_request( GTK_WIDGET( objeto ), TAMANIO_FOTO, TAMANIO_FOTO );
        senal_dibujado = g_signal_connect( objeto, "draw", G_CALLBACK( imagen_mostrar ), nullptr );

        // Obtiene la ventana
        objeto = gtk_builder_get_object( builder, ( "AreaEdicion" ) );
        if( objeto == nullptr ){
            throw runtime_error( "Ocurrió un error al abrir la ventana de edición de imagen." );
        }

        // Obtiene el área de dibujo
        objeto = gtk_builder_get_object( builder, ( "VentanaEdicionImagen" ) );
        if( objeto == nullptr ){
            throw runtime_error( "Ocurrió un error al abrir la ventana de edición de imagen." );
        }
        
        gtk_widget_show( GTK_WIDGET( objeto ) );
        g_signal_connect( objeto, "delete-event", G_CALLBACK( imagen_cancelar ), nullptr );
        
        objeto = gtk_builder_get_object( builder, ( "BarraEscaladoImagen" ) );
        if( objeto == nullptr ){
            throw runtime_error( "Ocurrió un error al abrir la ventana de edición de imagen." );
        }
        gtk_range_set_value( GTK_RANGE( objeto ), 100 );
    }
    catch( invalid_argument &ia ){
        cout << ia.what() << endl;
         gtk_widget_destroy( dialogo );
    }
}

void imagen_abrir( ImagenCairo *imagen, string nombreArchivo ){
    cout << "imagen_abrir" << endl;
    // Verifica que imagen apunte hacia un apuntador válido
    if( imagen == nullptr ){
        throw invalid_argument( "Referencia a un elemento de imagen nulo." );
    }

    // Carga la imagen
    GError *error = nullptr;
    imagen -> superficie = gdk_pixbuf_new_from_file( nombreArchivo.c_str(), &error );
    if( imagen -> superficie == nullptr ){
        string errorStr = "Error al cargar la imagen. ";
        throw runtime_error( errorStr + error -> message );
    }

    // Obtiene las dimensiones de la imagen
    imagen -> ancho  = gdk_pixbuf_get_width( imagen -> superficie );
    imagen -> alto = gdk_pixbuf_get_height( imagen -> superficie );
    if( imagen -> ancho <= 250 || imagen -> alto <= 250 ){
        imagen_destruir( imagen );
        throw invalid_argument( " El tamaño mínimo de la imagen debe ser de 250 pixeles." );
    }
}

gboolean imagen_mostrar( GtkWidget *widget, cairo_t *cr, gpointer data ){
    gdk_cairo_set_source_pixbuf(cr, imagen_escalada.superficie, imagen_posicion_x, imagen_posicion_y );
    cairo_paint(cr); 
    
    return FALSE;
}

void leer_mouse( GtkWidget *widget, GdkEventButton evento, gpointer data ){
    cout << "leer_mouse" << endl;
    imagen_ultimo_x = evento.x;
    imagen_ultimo_y = evento.y;
}

gboolean imagen_mover( GtkWidget* widget, GdkEventButton evento, gpointer data ){
    cout << "imagen_mover" << endl;
    // Obtiene el cambio entre la posición original del cursor y su posición actual
    int cambioX = imagen_ultimo_x - evento.x;
    int cambioY = imagen_ultimo_y - evento.y;

    // Registramos la nueva última posición del cursor
    imagen_ultimo_x = imagen_ultimo_x - cambioX;
    imagen_ultimo_y = imagen_ultimo_y - cambioY;

    // Aseguramos que la vista muestre algo más allá de lo que es la imagen
    imagen_posicion_x = imagen_posicion_x - cambioX;
    if( imagen_posicion_x - TAMANIO_FOTO < ( imagen_escalada.ancho * -1) ){
        imagen_posicion_x = ( imagen_escalada.ancho * -1) + TAMANIO_FOTO;
    }
    else if( imagen_posicion_x > 0 ){
        imagen_posicion_x = 0;
    }
    
    // Aseguramos que la vista muestre algo más allá de lo que es la imagen
    imagen_posicion_y = imagen_posicion_y - cambioY;
    if( imagen_posicion_y - TAMANIO_FOTO < imagen_escalada.alto * -1 ){
        imagen_posicion_y = ( imagen_escalada.alto * -1) + TAMANIO_FOTO;
    }
    else if( imagen_posicion_y > 0 ){
       imagen_posicion_y = 0;
    }

    // Redibuja la pantalla
    gtk_widget_queue_draw( GTK_WIDGET( gtk_builder_get_object( builder, "AreaDibujo" ) ) );

    return FALSE;
}

//
void imagen_temporal_guardar( string nombre ){
    // Libera la superficie anteriormente dada
    if( imagen_temporal != nullptr ){
        nombreTemporal.clear();
        cairo_surface_destroy( imagen_temporal );
        imagen_temporal = nullptr;
    }

    // Obtiene el formato de la imagen
    cairo_format_t formato = ( gdk_pixbuf_get_has_alpha( imagen.superficie ) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24 );

    // Crea la superficie cairo
    imagen_temporal = cairo_image_surface_create( formato, TAMANIO_FOTO, TAMANIO_FOTO );
    if( imagen_temporal == nullptr ){
        throw runtime_error( "Error de conversión de imagen." );
    }

    // Crea un contexto a partir de la superficie creada
    cairo_t *cr = cairo_create( imagen_temporal );
    if( cr == nullptr ){
        throw runtime_error( "Error de conversión de imagen." );
    }

    //Dibuja la parte de la superficie indicada de la imagen transformada
    gdk_cairo_set_source_pixbuf(cr, imagen_escalada.superficie, imagen_posicion_x, imagen_posicion_y );
    cairo_paint( cr );

    // Destruye el contexto
    cairo_destroy( cr );
    cr = nullptr;

    // Elimina el resto de las cosas
    imagen_cancelar();

    //
    nombreTemporal = nombre;
}

void imagen_guardar( string archivo ){
    if( imagen_temporal != nullptr ){
        // Guarda el archivo
        cairo_surface_write_to_png( imagen_temporal, archivo.c_str() );         

        // Destruye el contexto y la superficie
        cairo_surface_destroy( imagen_temporal );
        imagen_temporal = nullptr;
    }
}

gboolean imagen_cancelar(){
    cout << "imagen_cancelar" << endl;

    // Destruye el nombre dado
    nombreTemporal.clear();

    // Destruye la imagen
    imagen_destruir( &imagen );
    imagen_destruir( &imagen_escalada );

    // Reinicia las posiciones
    imagen_posicion_x = 0;
    imagen_posicion_y = 0;
    imagen_ultimo_x = 0;
    imagen_ultimo_y = 0;

    // Oculta la ventana
    GObject *objeto = gtk_builder_get_object( builder, "VentanaEdicionImagen" );
    gtk_widget_hide( GTK_WIDGET( objeto ) );

    return TRUE;
}

void imagen_escalar( ImagenCairo* imagen, ImagenCairo *imagen_destino, int ancho, int alto ){
    cout << "imagen_escalar" << endl;
    // Libera la imagen anterior
    cout << imagen << " " << imagen_destino << endl;
    if( imagen -> superficie != imagen_destino -> superficie ){
        imagen_destruir( imagen_destino );
    }

    // Escala la imagen nueva
    imagen_destino -> superficie = gdk_pixbuf_scale_simple( imagen -> superficie, ancho, alto, GDK_INTERP_BILINEAR );
    imagen_destino -> ancho  = gdk_pixbuf_get_width( imagen_destino -> superficie );
    imagen_destino -> alto = gdk_pixbuf_get_height( imagen_destino -> superficie );
}

void cambiar_tamanio( GtkWidget *widget, gpointer data ){
    cout << "cambiar_tamanio" << endl;
    // Se asegura de que no exista una división entre cero
    imagen.alto = (imagen.alto <= 0 ? 1 : imagen.alto );

    // Obtiene la relacion de aspecto
    gdouble relacion_aspecto = (double)imagen.ancho / (double)imagen.alto;
    
    // Lee el valor de la barra de escala
    gdouble valor = gtk_range_get_value( GTK_RANGE( widget ) );

    // Obtiene el tamaño del porcentaje de escala
    gdouble alto = (double)imagen.alto * (valor / 100 );
    gdouble ancho = (double)imagen.ancho * (valor / 100 );
    if( (int)floor( ancho ) < TAMANIO_FOTO || (int)floor( alto ) < TAMANIO_FOTO ){
        alto = TAMANIO_FOTO;
        ancho = alto * relacion_aspecto;
    }
    
    // Redondea los valores resultantes
    imagen_escalar( &imagen, &imagen_escalada, (int)floor( ancho ), (int)floor( alto ) );

    // Actualiza la posición de la imagen
    imagen_posicion_x = ( imagen_posicion_x - TAMANIO_FOTO < ( imagen_escalada.ancho * -1 ) ? ( imagen_escalada.ancho * -1) + TAMANIO_FOTO : imagen_posicion_x );
    imagen_posicion_y = ( imagen_posicion_y - TAMANIO_FOTO < ( imagen_escalada.alto * -1 ) ? ( imagen_escalada.alto * -1) + TAMANIO_FOTO : imagen_posicion_y );

    // Actualiza la vista
    gtk_widget_queue_draw( GTK_WIDGET( gtk_builder_get_object( builder, "AreaDibujo" ) ) );
}

GdkPixbuf *imagen_cargar_escalar( std::string directorio, int width, int height ){
    cout << "imagen_cargar_escalar" << endl;
    // Imagen que se carga
    GdkPixbuf *archivo;
    GError *error = NULL;
    GdkPixbuf *imagen = nullptr;

    // Carga el archivo correspondiente
    archivo = gdk_pixbuf_new_from_file( directorio.c_str(), &error );
    if(  archivo != nullptr ){
        imagen = gdk_pixbuf_scale_simple( archivo, width, height, GDK_INTERP_BILINEAR );
        //g_object_unref( archivo );
    }

    // Retorna la nueva 
    return imagen;
}

void imagen_destruir( ImagenCairo *imagen ){
    cout << "imagen_destruir" << endl;
    // No hace referencia hacia un elemento nulo
    if( imagen -> superficie != nullptr ){
        // Libera la superficie
        g_object_unref( imagen -> superficie );
        imagen -> superficie = nullptr;

        // Establece las dimensiones en cero
        imagen -> ancho = 0;
        imagen -> alto = 0;
    }
}