#ifndef __MATRIXSTATE_H
#define __MATRIXSTATE_H

#include "board.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>

#define MAX_FILAS 8

typedef struct
{
    int rows;       //Cantidad de filas (x)
    int columns;    //Cantidad de columnas (y)

    uint16_t output[MAX_FILAS]; //Matriz de visualizacion del display
    uint16_t buffer[MAX_FILAS];

    //Puertos de filas y columnas
    GPIO_TypeDef * rows_port;   
    GPIO_TypeDef * columns_port;

    //Primeros pines de fila y columnas (deben ser consecutivos)
    uint16_t row_pin;           
    uint16_t col_pin;

    //Flag de inicializacion correcta
    uint8_t initialized;

    //valores para limitar la matriz de salida a los valores marcados antes
    int x_mask;
    int y_mask;

    uint8_t rotate;
}Matrix_t;

/**
 * @brief Constructor de la estructura de la matriz led
 * 
 */
void Matrix_Init(
    Matrix_t *new_matrix,
    uint8_t rows,\
    uint8_t columns,\
    GPIO_TypeDef *row_port,\
    GPIO_TypeDef *col_port,\
    uint16_t first_row_pin,\
    uint16_t first_col_pin,\
    uint8_t rotate
    );

/**
 * @brief Limpia el vector de salida de la matriz
 * 
 * @param matrix 
 */
void Matrix_Clear(Matrix_t *matrix);


/**
 * @brief Funcion de desplazamiento de la matriz
 * 
 * @param matrix  matriz donde se guarda el vector a desplazar
 * @param y indica si es desplazamiento vertical u horizontal
 */
void shift_matrix(Matrix_t *matrix,uint8_t y);

/**
 * @brief Rota la matriz 90 grados
 * 
 * @param matrix 
 */
void matrix_rotate(Matrix_t *matrix);

/**
 * @brief Recorta el vector de salida para asegurarse que coincida
 * con el tamaño definido en la estructura
 * 
 * @param matrix matriz que se truncara
 */
void crop_input (Matrix_t *matrix);

/**
 * @brief Carga un vector al vector de salida
 * 
 * @param matrix matriz a modificar
 * @param data vector a cargar
 */
void load_output(Matrix_t *matrix, uint8_t data[]);

/**
 * @brief aplica espejado horizontal en la matriz de salida
 * 
 * @param matrix 
 */
void flip_x(Matrix_t *matrix);

/**
 * @brief aplica espejado vertical en la matriz de salida
 * 
 * @param matrix 
 */
void flip_y(Matrix_t *matrix);

/**
 * @brief efecto de negativo en la matriz de salida
 * 
 * @param matrix 
 */
void negate_output(Matrix_t *matrix);

#endif
