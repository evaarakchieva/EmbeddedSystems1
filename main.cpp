#include "hal.h"
//константы, маски тех битов которые мы хотим изменить в порте вывода. показывают какой пин вывода D надо зажигать
int lamps[8] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};

int animation[12][8] = {{1, 1, 1, 0, 0, 0, 1, 1},
                        {0, 1, 1, 1, 0, 1, 1, 0},
                        {0, 0, 1, 1, 1, 1, 0, 0},
                        {0, 0, 0, 1, 1, 1, 0, 0},
                        {0, 0, 1, 1, 1, 1, 1, 0},
                        {0, 1, 1, 0, 0, 1, 1, 1},
                        {1, 1, 0, 0, 1, 1, 1, 0},
                        {0, 1, 1, 1, 1, 1, 0, 0},
                        {0, 0, 1, 1, 1, 0, 0, 0},
                        {0, 1, 1, 1, 1, 0, 0, 0},
                        {0, 0, 0, 0, 1, 1, 0, 0},
                        {0, 0, 0, 0, 0, 1, 1, 0}};
const int ANIMATION_DELAY = 500;
unsigned int switches[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12}; //переключатели стенда с состояниями

bool check_code() {
    return (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == GPIO_PIN_RESET && //подключены к E порту
            HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_RESET &&
            HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET &&
            HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET);
}

void match_lamps_to_sw() {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], HAL_GPIO_ReadPin(GPIOE, switches[i])); //установим константы
    }
    for (int i = 4; i < 8; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], GPIO_PIN_RESET);
    }
}

void set_animation_state(int state) {
    for (int i = 0; i < 12; i++) {
        HAL_GPIO_WritePin(GPIOD, lamps[i], animation[state][i] ? GPIO_PIN_SET : GPIO_PIN_RESET); //меняем состояния, задержка 500мс
    }
}

void green() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET); //пин PD13 сигнал GLC
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);//PD14 YARC
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET); //PD15 YCRA
}

void yellow() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void red() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

bool check_btn() {
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET; //логический ноль на линии (сигнал), когда нажимаем кнопку
}

int umain() {
    int state = 0;
    bool button = false;
    while (true) {
        if (check_code()) {
            button = check_btn() ^ button;
            if (!button) {
                green();
                set_animation_state(state);
                state = (state + 1) % 12;
                HAL_Delay(ANIMATION_DELAY);
            } else {
                red();
                HAL_Delay(500);
            }
        } else {
            match_lamps_to_sw();
            yellow();
            state = 0;
            button = false;
        }
    }

    return 0; //больше ничего не происходит после завершения



}

