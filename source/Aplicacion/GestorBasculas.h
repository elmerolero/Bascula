#ifndef GESTOR_BASCULAS_H
#define GESTOR_BASCULAS_H

#include "Bascula.h"
#include "GestorRegistros.h"
#include "Aplicacion.h"
#include "Funciones.h"
#include <list>

// Lista de las básculas creadas
extern unsigned int codigoBasculaActual;
extern std::list< Bascula * > basculasRegistradas;

// Abre y cierra el lector
void basculaAbrirLector();
gboolean basculaCerrarLector();

// Inicia el lector
void basculaLecturaIniciar();
void basculaLecturaActualizar();
void basculaObtenerRegistros();
void basculaObtenerPuertosDisponibles();
void basculaGuardar();
void basculaNuevo( Bascula *bascula );
void basculaEditar( Bascula *bascula );
void basculaEliminar();
void basculaActualizarRegistros();

void basculaLimpiarFormulario( void );
void basculaRegistrarPeso( std::string etiquetaPeso, std::string etiquetaHora );
Bascula *basculaBuscarPorCodigo( unsigned int codigo );
void basculaEstablecerRenglon( Bascula *bascula, Row *row );

#endif
