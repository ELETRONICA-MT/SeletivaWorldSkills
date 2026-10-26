#include "keypad.h"

// Envia 8 bits para o registrador de deslocamento 74HC595
void Send_74hc595(uint8_t dado) {
    HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);

    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(DADO_GPIO_Port, DADO_Pin, (dado >> i) & 0x01);

        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LATCH2_GPIO_Port, LATCH2_Pin, GPIO_PIN_RESET);
}

// Lê os estados dos 16 botões do teclado e preenche o vetor Button[16]
void Matriz_teclado(bool Button[16]) {
    bool Linha1, Linha2, Linha3, Linha4;

    // Coluna 1
    Send_74hc595(0x88);
    Linha1 = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
    Linha2 = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
    Linha3 = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
    Linha4 = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);
    Button[0] = Linha1; Button[1] = Linha2; Button[2] = Linha3; Button[3] = Linha4;

    // Coluna 2
    Send_74hc595(0x48);
    Linha1 = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
    Linha2 = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
    Linha3 = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
    Linha4 = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);
    Button[4] = Linha1; Button[5] = Linha2; Button[6] = Linha3; Button[7] = Linha4;

    // Coluna 3
    Send_74hc595(0x28);
    Linha1 = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
    Linha2 = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
    Linha3 = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
    Linha4 = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);
    Button[8] = Linha1; Button[9] = Linha2; Button[10] = Linha3; Button[11] = Linha4;

    // Coluna 4
    Send_74hc595(0x18);
    Linha1 = HAL_GPIO_ReadPin(LINHA1_GPIO_Port, LINHA1_Pin);
    Linha2 = HAL_GPIO_ReadPin(LINHA2_GPIO_Port, LINHA2_Pin);
    Linha3 = HAL_GPIO_ReadPin(LINHA3_GPIO_Port, LINHA3_Pin);
    Linha4 = HAL_GPIO_ReadPin(LINHA4_GPIO_Port, LINHA4_Pin);
    Button[12] = Linha1; Button[13] = Linha2; Button[14] = Linha3; Button[15] = Linha4;
}
