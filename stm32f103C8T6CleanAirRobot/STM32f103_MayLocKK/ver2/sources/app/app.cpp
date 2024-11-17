
/*
ADC 				A0+A8 A1

USART				A9 A10	A2 A3

PWM                 B8 B9

LINE_IO_PIN         B3 B4 B5 B6 A15

PWM_Fan             B7

DHT11               A7
*/

/* driver include */
#include "../driver/led/led.h"
#include "../driver/flash/flash.h"
#include "../driver/Arduino-LiquidCrystal-I2C-library/LiquidCrystal_I2C.h"
#include "../driver/modbus_rtu/modbus_rtu.h"
#include "../driver/EmonLib/EmonLib.h"
#include "../driver/sys/sys.h"
#include "../driver/delay/delay.h"
#include "../driver/DHT11/dht11.h"
#include "../driver/ADC/adc.h"

/* app include */
#include "app.h"
#include "app_dbg.h"

/* task include */

/* sys include */
#include "../sys/sys_irq.h"
#include "../sys/sys_io.h"
#include "../sys/sys_ctrl.h"
#include "../sys/sys_dbg.h"
#include "../sys/sys_arduino.h"
#include "io_cfg.h"
/* common include */
#include "../common/utils.h"


led_t led_life;
vitri_t object_A;
DHT11_t object_b;

//  A la trai, B la phai

int16_t Kp = 90, Ki  = 5,  Kd  = 60; //Điều chỉnh
int16_t error = 0,  P = 0,  I = 0,  D = 0,  PID_value =  0;
int16_t previous_error  = 0;
int16_t PV  = 200;
uint8_t PID_right,  PID_left;


////////////////////

//Khai báo chân tín hiệu motor A
//#define enA   10
//#define in1   13
//#define in2   12

//Khai báo chân tín hiệu cho motor B
//#define in3   11
//#define in4    8
//#define enB    9
uint8_t j, count_straight = 0;
uint8_t temp, humi, roomnow = 0, dir = 0, train = 5;
uint8_t line_val[5] = {0};
uint8_t tim_reset = 0;
uint8_t buffer_string[25] = "";
uint16_t buff_sharp[6];
uint16_t buff_pin[6];
////////////////////
//cmd_t cmd1;


void vitri_init(vitri_t * object);
void DHT11_init(DHT11_t * object);
void delay_check(uint16_t tim_while);
void read_sensor();
uint16_t Get_adc_sensor();
uint16_t Get_adc_PIN();
int16_t constrain(int16_t value, int16_t value_min, int16_t value_max);
void  calculate_pid();  //int8_t * error
void GoMotor();//int16_t  PV, int16_t PID_value
void GoMotor_trans_dir();
void display_serial();
void Usart2_send_data(uint8_t * data, uint8_t len);
void Send_data_string(uint8_t char_array[], uint8_t len, uint8_t value_data);
/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app() {
    APP_PRINT("main_app() entry OK\n");

    /******************************************************************************
    * init applications
    *******************************************************************************/
    /*********************
    * hardware configure *
    **********************/
    /* init watch dog timer */
    sys_ctrl_independent_watchdog_init();	/* 32s */
    sys_ctrl_soft_watchdog_init(200);		/* 20s */

    /*********************
    * software configure *
    **********************/
    /* life led init */
    led_init(&led_life, led_life_init, led_life_on, led_life_off);
    led_off(&led_life);


    //vitri_init(&object_A);
    //DHT11_init(&object_b);
    //vitri_init(&cmd1.ojA);
    //led_test();

    delay_init();       //ok
    GPIO_line_init();
    DHT11_Init();
    TIM3_Int_Init(999, 7199);    // config timer 100ms  ok
    //sys_cfg_console();// usart1 debug ok
    Usart2_Init();
    Adc_Init(ADC1, ADC_Channel_0);      //ok
    IO_LED_control();   //  ok
    Adc_Init(ADC2, ADC_Channel_1);
    PWM_Init();     //ok
    buff_sharp[train] = 0;
    buff_pin[train] = 0;
    while(1) {

        /*app_interface_ui(SL_SENSOR_TEMP, (uint8_t*)&object_A, sizeof (vitri_t));
        led_off(&led_life);
        delay_check(1000);
        led_on(&led_life);
        delay_check(1000);*/
        /*

         ADC_Read( ADCx,channel);*/
        //Get_adc_sensor();
        //delay(1000);
        //Get_adc_sensor();
        //DHT11_Read_Data(&temp,&humi);

        if (tim_reset < tim3_print)
        {
            read_sensor();
            if(error == 0)
                count_straight ++;
            else if (count_straight <= 2) {
                count_straight = 2;
            }
            else
            {
                count_straight --;
            }
            if (count_straight >= 9) {
                 count_straight = 9;
             }
            /*if (roomnow == roomset)
            {
                PWM_1(0);
                PWM_2(0);
            }
            else if (roomnow < roomset && dir == 1) {
                calculate_pid();
                GoMotor();
            }
            else if (roomnow > roomset && dir == 0) {
                calculate_pid();
                GoMotor();
            }
            else if (roomnow < roomset && dir == 0){
                GoMotor_trans_dir();
            }
            else if (roomnow > roomset && dir == 1){
                GoMotor_trans_dir();
            }/
            else{}*/
            GoMotor();

            //display_serial();

            tim_reset = tim3_print;
        }
        if (flag_read == 1)
        {
            for (uint8_t k = 0; k < train; k++)
            {
                buff_sharp[k] = Get_adc_sensor();
                buff_sharp[train] += buff_sharp[k];
                delay_ms(2);
            }
            buff_sharp[train] = buff_sharp[train]/(train+1);

            /*memcpy(&buffer_string[0], "sharp", sizeof ("sharp"));
            buffer_string[sizeof ("sharp")] = uint8_t(buff_sharp[train] / 10 + 48);
            buffer_string[sizeof ("sharp") + 1] = uint8_t(buff_sharp[train] % 10 + 48);
            memcpy(&buffer_string[sizeof ("sharp") + 1 + 1], "\n", sizeof ("\n"));
            Usart2_send_data((uint8_t*)&buffer_string[0], sizeof (buffer_string));
            for (uint8_t k = 0;  k < sizeof (buffer_string); k++)
            {
                xprintf("%c",buffer_string[k]);
            }

            flag_read = 0;*/
            Send_data_string((uint8_t*)"s",sizeof("s"), (uint8_t) buff_sharp[train]);

        }
        else if (flag_read == 2)
        {
            DHT11_Read_Data(&temp,&humi);
            Send_data_string((uint8_t*)"h",sizeof("h"), (uint8_t) humi);
        }
        else if (flag_read == 3)
        {
            DHT11_Read_Data(&temp,&humi);
            Send_data_string((uint8_t*)"t",sizeof("t"), (uint8_t) temp);
        }
        else if (flag_read == 4)
        {
            for (uint8_t k = 0; k < train; k++)
            {
                buff_pin[k] = Get_adc_PIN();
                buff_pin[train] += buff_pin[k];
                delay_ms(2);
            }
            buff_pin[train] = buff_pin[train]/(train+1);
            Send_data_string((uint8_t*)"p",sizeof("p"), (uint8_t) buff_pin[train]);

        }
        else if (flag_read == 5)
        {
            Send_data_string((uint8_t*)"r",sizeof("r"), (uint8_t) roomnow);
        }
        else if (flag_read == 6)
        {
            Send_data_string((uint8_t*)"b",sizeof("b"), (uint8_t) int_datafeedback);
        }

    }
    return 0;
}

/* hardware timer interrupt 10ms
 * used for sensor polling
 */
void sys_irq_timer_10ms() {

}
void vitri_init(vitri_t * object) {
    object->cmd = 1;
    object->x = 44;
    object->y = 8;
    object->point = 90;
}
void DHT11_init(DHT11_t * object) {
    object->cmd = SL_SENSOR_HUM;
    object->temp = 2;
    object->humi = 2;
}
void delay_check(uint16_t tim_while)
{
    uint8_t temp=0,humi=0;
    while (tim_while)
    {


        if (state_printf  > 0)
        {
            rx_frame_parser( c , sizeof(c));
            xprintf("%ld\n",state_printf);
            delay_ms(10);
            state_printf --;
        }
        if (tim3_print  > 0)
        {
            DHT11_Read_Data(&temp,&humi);
            object_b.temp = temp;
            object_b.humi = humi;
            xprintf("%d--%d\r\n",temp,humi);
            app_interface_ui(SL_DHT11, (uint8_t*)&object_b, sizeof (DHT11_t));
            delay_ms(10);
            tim3_print --;
        }
        delay(1);
        tim_while--;
    }
}

uint16_t Get_adc_sensor()
{
    uint16_t samplingTime = 280;
    uint16_t deltaTime = 40;
    uint16_t sleepTime = 9680;

    uint32_t voMeasured = 0;
    uint16_t calcVoltage = 0;
    uint16_t dustDensity = 0;

    GPIO_ResetBits(GPIOA, GPIO_Pin_8); // Bật IR LED
    delay_us(samplingTime);  //Delay 0.28ms
    voMeasured = ADC_Read(ADC2, ADC_Channel_0)*100; // Đọc giá trị ADC V0
    delay_us(deltaTime); //Delay 0.04ms
    GPIO_SetBits(GPIOA, GPIO_Pin_8); // Tắt LED
    delay_us(sleepTime); //Delay 9.68ms

    // Tính điện áp từ giá trị ADC
    calcVoltage = voMeasured / 4095 * 3.3; //1024) 12 bit; //Điệp áp Vcc của cảm biến (5.0 hoặc 3.3)

    // Linear Equation http://www.howmuchsnow.com/arduino/airquality/
    // Chris Nafis (c) 2012
    dustDensity = (calcVoltage*0.26/100+0.01)*100;//(0.17 * (float)calcVoltage/100 - 0.1)*10000;


//    xprintf("Raw Signal Value (0-1023): ");
//    xprintf("%6d",voMeasured);
//    delay_ms(5);
//    xprintf(" - Voltage: ");
//    xprintf("%5d/100",calcVoltage);
//    delay_ms(5);
//    xprintf(" - Dust Density: ");
//    xprintf("%5d/100 mg/m^3",dustDensity);
//    xprintf("\n");
//    delay_ms(300);
    return dustDensity;
}
uint16_t Get_adc_PIN()
{
    uint32_t voMeasured = 0;
    uint16_t calcVoltage = 0;
    uint8_t dustDensity = 0;
    voMeasured = ADC_Read(ADC1, ADC_Channel_1)*100;
    delay_us(500);
    calcVoltage = voMeasured / 4095 * 3.3;
    dustDensity = (calcVoltage - 235 + 2) * 100 / (330-235);
//    xprintf("vol=%d\n",voMeasured);
//    xprintf("vol=%d\n",voMeasured);
//    xprintf("pin=%d\n",dustDensity);
    if (dustDensity > 99)
    {
        dustDensity = 99;
    }
    return dustDensity;
}
////////////////////
void read_sensor()
{
  for (j = 0; j < 5; j++)
  {
    if (j == 4)
    {
        line_val[j] = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);
    }
    else {
        line_val[j] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3<<j);
    }

    delay_ms(1);
  }

  if        ((line_val[0] ==  1)  &&  (line_val[1] ==  0)  && (line_val[2] ==  0) && (line_val[3] ==  0)  && (line_val[4] ==  0)) error = -4;
  else  if  ((line_val[0] ==  1)  &&  (line_val[1] ==  1)  && (line_val[2] ==  0) && (line_val[3] ==  0)  && (line_val[4] ==  0)) error = -3;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  1)  && (line_val[2] ==  0) && (line_val[3] ==  0)  && (line_val[4] ==  0)) error = -2;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  1)  && (line_val[2] ==  1) && (line_val[3] ==  0)  && (line_val[4] ==  0)) error = -1;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  1) && (line_val[3] ==  0)  && (line_val[4] ==  0)) error =  0;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  1) && (line_val[3] ==  1)  && (line_val[4] ==  0)) error =  1;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  0) && (line_val[3] ==  1)  && (line_val[4] ==  0)) error =  2;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  0) && (line_val[3] ==  1)  && (line_val[4] ==  1)) error =  3;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  0) && (line_val[3] ==  0)  && (line_val[4] ==  1)) error =  4;

  else  if  ((line_val[0] ==  1)  &&  (line_val[1] ==  1)  && (line_val[2] ==  1) && (line_val[3] ==  1)  && (line_val[4] ==  1))
  {
//    if (PID_value >  0)  error =  4;
//   else   if  (PID_value  < 0)  error = -4;
//   else   error = 0;
  }

  //
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  0)  && (line_val[2] ==  0) && (line_val[3] ==  0)  && (line_val[4] ==  0)) roomnow =  1;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  1)  && (line_val[2] ==  1) && (line_val[3] ==  1)  && (line_val[4] ==  0)) roomnow =  2;
  else  if  ((line_val[0] ==  0)  &&  (line_val[1] ==  1)  && (line_val[2] ==  0) && (line_val[3] ==  1)  && (line_val[4] ==  0)) roomnow =  3;
  else  if  ((line_val[0] ==  1)  &&  (line_val[1] ==  0)  && (line_val[2] ==  1) && (line_val[3] ==  0)  && (line_val[4] ==  1)) roomnow =  4;
  else {}
}
int16_t constrain(int16_t value, int16_t value_min, int16_t value_max)
{
    if      (value > value_max) return value_max;
    else if (value < value_min) return value_min;
    else                        return value;

}
void  calculate_pid()//Tinh PID
{
  P = error;
  I = I + error;// sai số trước đó cộng sai số hiện tại
  I =  constrain(I,-50,50);
  D = error - previous_error ;// sai số hiện tại trừ sai số trước đó
  D =  constrain(D,-50,50);
  PID_value = (Kp * P)  + (Ki * I)  + (Kd * D);
  PID_value =  constrain(PID_value,-180,180);
  previous_error  = error;

}

void GoMotor()
{
      calculate_pid();
      PID_right = PV  - PID_value;
      PID_left  = PV  + PID_value;
      PID_right = constrain(PID_right, 0, 200);
      PID_left  = constrain(PID_left, 0, 200);
      if (count_straight >= 7)
      {
          PID_left = 200;
          PID_right = 200;

      }
      PWM_1((uint8_t)PID_left);
      PWM_2((uint8_t)PID_right);

}
void GoMotor_trans_dir(){
    //xprintf("xoay\n");
    PWM_1(200);
    PWM_2(0);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    PWM_2(200);
    delay_ms(1000);
    read_sensor();
    /*while (error == 0||error == -1||error == 1) {
        read_sensor();
        delay_ms(10);
    }*/
    PWM_1(0);
    PWM_2(0);
    dir = (~dir)&0x01;
    //xprintf("%d\n",dir);
}
void display_serial()
  {
  for (j=0;j<5;j++)
  {
  xprintf("%d",line_val[j]);
  xprintf(" ");
  delay_ms(1);
  }
  xprintf("  ");
  xprintf("%3d",PID_left);
  delay_ms(2);
  xprintf("  ");
  xprintf("%3d",PID_right);
  xprintf("  ");
  xprintf("%3d",error);
  xprintf("  ");
  xprintf("%3d",PID_value);
  xprintf("\n");
  delay_ms(2);
  //delay_ms(100);
  }
void Usart2_send_data(uint8_t * data, uint8_t len)
{
    uint8_t buffer[100];
    memcpy(&buffer[0], &data[0], len);
    buffer[len] = '\0';
    for (uint8_t i = 0; i <= (len); i++)
        {
        USART_SendData(USART2, (char)buffer[i]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}
void Send_data_string(uint8_t char_array[], uint8_t len, uint8_t value_data)
{
    uint8_t char_c[25] = "";
    memcpy(&char_c[0], &(char_array[0]), len);
    memcpy(&buffer_string[0], &char_c[0], sizeof (char_c));
    buffer_string[len] = uint8_t(value_data / 10 + 48);
    buffer_string[len + 1] = uint8_t(value_data % 10 + 48);
    memcpy(&buffer_string[len + 1 + 1], "\n", sizeof ("\n"));
    //memcpy(&buffer_string[len + 1 + 1 + 1], "\r", sizeof ("\r"));
    Usart2_send_data((uint8_t*)&buffer_string[0], sizeof (buffer_string));
    for (uint8_t k = 0;  k < sizeof (buffer_string); k++)
    {
        xprintf("%c",buffer_string[k]);
    }

    flag_read = 0;
}
////////////////////

