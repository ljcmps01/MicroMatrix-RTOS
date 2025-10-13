#ifndef __MATRIXSTATE_H
#define __MATRIXSTATE_H

#include "board.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>

#define MAX_FILAS 8

typedef struct Matrix Matrix_t;

Matrix_t *GetMatrix(void);

/**
 * @brief Constructor de la estructura de la matriz led
 * 
 */
void Matrix_Init(
    Matrix_t *matrix,
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
void load_output(Matrix_t *matrix, const uint8_t data[]);

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
