#include "menu.h"
#include "common.h"

int main_menu() {
    
    scroll_text("Menu: L/Bitris ; R/Snake");
    
    return 0;
}

void vMenuTask(void *pvParameters)
{
    scroll_text("HyF"); // Welcome message

    int a = main_menu();

    //Selection for the menu
    /*switch (main_menu()){
        case Left botton:
            Run Bitris
            break;
        case Right botton:
            Run Snake
            break;
        default:
            SEGGER_RTT_WriteString(0, "No valid option selected\n");
            scroll_text("No valid option selected.");
            break;
    }*/
}

void Runapp(void){
    xTaskCreate(vMenuTask, "MenuTask", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
}