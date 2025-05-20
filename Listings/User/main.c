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

// �����Լ���Lcd_Fill������ʹ�����еĺ���ʵ��
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


// �ַ������Һ���
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
    uint8_t esp_status = 0;                // ESPģ��״̬
    uint32_t timestamp = 0;                // ʱ���
    const char *http_response;             // HTTP��Ӧ
    uint32_t response_length = 0;          // ��Ӧ����
    char buffer[64];                       // ��ʾ������
    char temp_buffer[8];                  // �¶Ȼ�����
    char weather_buffer[16];              // ����������
    
    // ��ʼ��LCD��ʾ��
    Lcd_Init();                            // ��ʼ��LCD
    LCD_LED_SET;                           // �򿪱���
    Lcd_Clear(BLACK);                      // ����Ϊ��ɫ����
    
    // ��ʾ����Ͳ�����Ϣ
    Gui_DrawFont_GBK16(10, 10, WHITE, BLACK, (uint8_t*)"ESP-AT����");
    
    // ��ʼ��ESP-ATģ��
    esp_status = esp_at_init();
    
    Delay_ms(1000);                        // �ȴ�1��
    
    // ��ʼ��WiFi
    Gui_DrawFont_GBK16(10, 30, BLUE, BLACK, (uint8_t*)"Init WiFi...");
    esp_status = esp_at_wifi_init();
//    if(esp_status)
//    {
//        Gui_DrawFont_GBK16(10, 30, GREEN, BLACK, (uint8_t*)"WiFi init success");
//    }
//    else
//    {
//        Gui_DrawFont_GBK16(10, 30, RED, BLACK, (uint8_t*)"WiFi init fail!  ");
//        while(1);                          // ���WiFi��ʼ��ʧ�ܣ�����ֹͣ
//    }
    
    Delay_ms(1000);                        // �ȴ�1��
    
    // �滻Ϊ���Լ���WiFi���ƺ�����
    esp_status = esp_at_wifi_connect("AB", "123456789");
    if(esp_status)
    {
        Gui_DrawFont_GBK16(10, 30, GREEN, BLACK, (uint8_t*)"WiFi connect");
    }
    else
    {
        Gui_DrawFont_GBK16(10,30, RED, BLACK, (uint8_t*)"WiFi fail ");
        while(1);                          // ���WiFi����ʧ�ܣ�����ֹͣ
    }
    
    Delay_ms(1000);                        // �ȴ�1��
    
    // ��ѭ��
    while(1)
    {
        // ��ȡʱ���
        esp_status = esp_at_time_get(&timestamp);
        if(esp_status)
        {
            // ��ʱ���ת��Ϊ�ַ�������ʾ
            sprintf(buffer, "time: %lu", timestamp);
            Gui_DrawFont_GBK16(10, 50, GREEN, BLACK, (uint8_t*)buffer);
        }
        
//        // ����HTTP GET������֪����API
//        Gui_DrawFont_GBK16(10, 90, BLUE, BLACK, (uint8_t*)"Getting weather");
        esp_status = esp_at_http_get("https://api.seniverse.com/v3/weather/now.json?key=SfesfR_4aLtHMcNwv&location=beijing&language=en&unit=c", &http_response, &response_length, 8000);
        
        if(esp_status)
        {
//            Gui_DrawFont_GBK16(10, 70, GREEN, BLACK, (uint8_t*)"Weather OK    ");
            
            // �����¶� - ���� "temperature":"
            char *temp_pos = strstr_simple(http_response, "\"temperature\":\"");
            if(temp_pos) {
                temp_pos += 15; // �ƶ����¶�ֵ�Ŀ�ʼ
                int i = 0;
                while(temp_pos[i] != '"' && i < 5) {
                    temp_buffer[i] = temp_pos[i];
                    i++;
                }
                temp_buffer[i] = '\0';
                
                // ��ʾ�¶�
                sprintf(buffer, "Temp: %s C", temp_buffer);
                Gui_DrawFont_GBK16(10, 70, WHITE, BLACK, (uint8_t*)buffer);
            }
            
            // ��������״�� - ���� "text":"
            char *text_pos = strstr_simple(http_response, "\"text\":\"");
            if(text_pos) {
                text_pos += 8; // �ƶ��������ı��Ŀ�ʼ
                int i = 0;
                while(text_pos[i] != '"' && i < 15) {
                    weather_buffer[i] = text_pos[i];
                    i++;
                }
                weather_buffer[i] = '\0';
                
                // ��ʾ����״��
                sprintf(buffer, "Weather: %s", weather_buffer);
                Gui_DrawFont_GBK16(10, 90, YELLOW, BLACK, (uint8_t*)buffer);
            }
        }
        else
        {
//            Gui_DrawFont_GBK16(10, 90, RED, BLACK, (uint8_t*)"Weather fail  ");
            // ���� - ��ʾ����������Ϣ
            if(response_length > 0 && response_length < 30) {
                strncpy(buffer, http_response, response_length);
                buffer[response_length] = '\0';
                Gui_DrawFont_Small(10, 90, RED, BLACK, (uint8_t*)buffer);
            }
        }
        
        // �ȴ�10���ٴ�ˢ��
        Delay_ms(10000);
        
        // �������������
        Lcd_Fill(10, 110, 118, 128, BLACK);
    }
}

