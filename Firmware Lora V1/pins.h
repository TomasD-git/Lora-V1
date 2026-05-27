#pragma once

#define LORA_SCK      17  
#define LORA_MISO     16   
#define LORA_MOSI     15 
#define LORA_NSS      11   
#define LORA_RST      14   
#define LORA_DIO1     12 
#define LORA_BUSY     13  

// W25Q12
#define FLASH_CS      33  
#define FLASH_RST     39  

// SH1106
#define SCREEN_SDA    21   
#define SCREEN_SCL    26   
#define SCREEN_MOSFET  7  

// PCF85063A
#define RTC_SDA       34  
#define RTC_SCL       32   
#define RTC_INT       33  

// BQ25895
#define PMU_SCL       26   
#define PMU_SDA       21  
#define PMU_INT        5  

// RF control 
#define RF_MOSFET_CTRL 27 

// push buttons
#define BTN_SW6        6  
#define BTN_SW7        0   
#define BTN_SW8        8
#define BTN_SW9        9 
#define BTN_SW10      10  

// Rotary Encoder 1
#define ROT1_A         1   
#define ROT1_B         2   
#define ROT1_BTN       3  

// Rotary Encoder 2 
#define ROT2_A         4  
#define ROT2_B         5   
#define ROT2_BTN      18   

#define OLED_ADDR     0x3C
#define RTC_ADDR      0x51
#define PMU_ADDR      0x6A
