#include "stm32f10x.h"
#include "Delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "AD.h"
#include "MyDMA.h"
#include "MPU6050.h"
#include "W25Q32.h"
#include "esp_at.h"
#include "stdio.h"
#include "string.h"

// 定义自己的Lcd_Fill函数，使用已有的函数实现
void Lcd_Fill(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color)
{
    u16 i, j;
    Lcd_SetRegion(x_start, y_start, x_end, y_end);
    Lcd_WriteIndex(0x2C);
    for(i = x_start; i <= x_end; i++)
    {
        for(j = y_start; j <= y_end; j++)
        {
            LCD_WriteData_16Bit(color);
        }
    }
}


// 字符串查找函数
char* strstr_simple(const char* haystack, const char* needle) {
    while (*haystack) {
        const char* h = haystack;
        const char* n = needle;
        
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        
        if (!*n) {
            return (char*)haystack;
        }
        
        haystack++;
    }
    return NULL;
}

int main(void)
{
    uint8_t esp_status = 0;                // ESP模块状态
    uint32_t timestamp = 0;                // 时间戳
    const char *http_response;             // HTTP响应
    uint32_t response_length = 0;          // 响应长度
    char buffer[64];                       // 显示缓冲区
    char temp_buffer[8];                  // 温度缓冲区
    char weather_buffer[16];              // 天气缓冲区
    
    // 初始化LCD显示屏
    Lcd_Init();                            // 初始化LCD
    LCD_LED_SET;                           // 打开背光
    Lcd_Clear(BLACK);                      // 清屏为黑色背景
    
    // 显示标题和测试信息
    Gui_DrawFont_GBK16(10, 10, WHITE, BLACK, (uint8_t*)"ESP-AT测试");
    
    // 初始化ESP-AT模块
    esp_status = esp_at_init();
    
    Delay_ms(1000);                        // 等待1秒
    
    // 初始化WiFi
    Gui_DrawFont_GBK16(10, 30, BLUE, BLACK, (uint8_t*)"Init WiFi...");
    esp_status = esp_at_wifi_init();
//    if(esp_status)
//    {
//        Gui_DrawFont_GBK16(10, 30, GREEN, BLACK, (uint8_t*)"WiFi init success");
//    }
//    else
//    {
//        Gui_DrawFont_GBK16(10, 30, RED, BLACK, (uint8_t*)"WiFi init fail!  ");
//        while(1);                          // 如果WiFi初始化失败，程序停止
//    }
    
    Delay_ms(1000);                        // 等待1秒
    
    // 替换为你自己的WiFi名称和密码
    esp_status = esp_at_wifi_connect("AB", "123456789");
    if(esp_status)
    {
        Gui_DrawFont_GBK16(10, 30, GREEN, BLACK, (uint8_t*)"WiFi connect");
    }
    else
    {
        Gui_DrawFont_GBK16(10,30, RED, BLACK, (uint8_t*)"WiFi fail ");
        while(1);                          // 如果WiFi连接失败，程序停止
    }
    
    Delay_ms(1000);                        // 等待1秒
    
    // 主循环
    while(1)
    {
        // 获取时间戳
        esp_status = esp_at_time_get(&timestamp);
        if(esp_status)
        {
            // 将时间戳转换为字符串并显示
            sprintf(buffer, "time: %lu", timestamp);
            Gui_DrawFont_GBK16(10, 50, GREEN, BLACK, (uint8_t*)buffer);
        }
        
//        // 发起HTTP GET请求到心知天气API
//        Gui_DrawFont_GBK16(10, 90, BLUE, BLACK, (uint8_t*)"Getting weather");
        esp_status = esp_at_http_get("https://api.seniverse.com/v3/weather/now.json?key=SfesfR_4aLtHMcNwv&location=beijing&language=en&unit=c", &http_response, &response_length, 8000);
        
        if(esp_status)
        {
//            Gui_DrawFont_GBK16(10, 70, GREEN, BLACK, (uint8_t*)"Weather OK    ");
            
            // 解析温度 - 查找 "temperature":"
            char *temp_pos = strstr_simple(http_response, "\"temperature\":\"");
            if(temp_pos) {
                temp_pos += 15; // 移动到温度值的开始
                int i = 0;
                while(temp_pos[i] != '"' && i < 5) {
                    temp_buffer[i] = temp_pos[i];
                    i++;
                }
                temp_buffer[i] = '\0';
                
                // 显示温度
                sprintf(buffer, "Temp: %s C", temp_buffer);
                Gui_DrawFont_GBK16(10, 70, WHITE, BLACK, (uint8_t*)buffer);
            }
            
            // 解析天气状况 - 查找 "text":"
            char *text_pos = strstr_simple(http_response, "\"text\":\"");
            if(text_pos) {
                text_pos += 8; // 移动到天气文本的开始
                int i = 0;
                while(text_pos[i] != '"' && i < 15) {
                    weather_buffer[i] = text_pos[i];
                    i++;
                }
                weather_buffer[i] = '\0';
                
                // 显示天气状况
                sprintf(buffer, "Weather: %s", weather_buffer);
                Gui_DrawFont_GBK16(10, 90, YELLOW, BLACK, (uint8_t*)buffer);
            }
        }
        else
        {
//            Gui_DrawFont_GBK16(10, 90, RED, BLACK, (uint8_t*)"Weather fail  ");
            // 调试 - 显示错误代码或信息
            if(response_length > 0 && response_length < 30) {
                strncpy(buffer, http_response, response_length);
                buffer[response_length] = '\0';
                Gui_DrawFont_Small(10, 90, RED, BLACK, (uint8_t*)buffer);
            }
        }
        
        // 等待10秒再次刷新
        Delay_ms(10000);
        
        // 清除旧数据区域
        Lcd_Fill(10, 110, 118, 128, BLACK);
    }
}

