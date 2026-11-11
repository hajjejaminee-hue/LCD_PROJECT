#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "lcd.h"
#include <stdio.h>

void Rcc_Config(void) {

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

void GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LCD_Config(void) {
    LCD_InitTypeDef LCD_InitStruct;
    LCD_InitStruct.CharacterFont = LCD_CharacterFont_5x10Dots;
    LCD_InitStruct.DataLength = LCD_DataLength_4Bit;
    LCD_InitStruct.LineNumber = LCD_LineNumber_2Lines;
    LCD_Init(&LCD_InitStruct);
}

void ADC_Config(void) {
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonStructure;

    ADC_CommonStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 3;
    ADC_Init(ADC1, &ADC_InitStructure);


    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_84Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_84Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_84Cycles);

    ADC_Cmd(ADC1, ENABLE);
}

void delay(int N) {
    volatile int i;
    for (i = 0; i < N; i++);
}

char* dtostrf(double val, signed char width, unsigned char prec, char* s) {
    char fmt[10];
    sprintf(fmt, "%%%d.%df", width, prec); // creates format like "%4.2f"
    sprintf(s, fmt, val);
    return s;
}


int main(void) {
    Rcc_Config();
    GPIO_Config();
    ADC_Config();
    LCD_Config();


    double value_ch8, value_ch1, value_ch2;
    double tension_ch8, tension_ch1, tension_ch2;
    char msg1[20], msg2[20], msg3[20];

    while (1) {
        ADC_SoftwareStartConv(ADC1);
        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        value_ch8 = ADC_GetConversionValue(ADC1);

        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        value_ch1 = ADC_GetConversionValue(ADC1);

        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        value_ch2 = ADC_GetConversionValue(ADC1);

        tension_ch8 = (value_ch8 * 3.3) / 4096;
        tension_ch1 = (value_ch1 * 3.3) / 4096;
        tension_ch2 = (value_ch2 * 3.3) / 4096;


        dtostrf(tension_ch8, 4, 2, msg1);
        dtostrf(tension_ch1, 4, 2, msg2);
        dtostrf(tension_ch2, 4, 2, msg3);

        LCD_Goto(0, 0);
        LCD_Puts(msg1);
        delay(100);
        LCD_Goto(1, 0);
        LCD_Puts(msg2);
        delay(100);
        LCD_Goto(2, 0);
        LCD_Puts(msg3);
        delay(100);
    }
}
