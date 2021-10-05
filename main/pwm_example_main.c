/* pwm example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"

#include "esp8266/gpio_register.h"
#include "esp8266/pin_mux_register.h"

#include "driver/pwm.h"

// GPIO pin for the servo control signal
#define PWM_OUTPUT_IO_NUM       2

// PWM period 20,000us (equivalent to 20ms or a frequency of 50Hz)
#define PWM_PERIOD              (20000)

// Servo has an operating range between 1.0ms to 2.0ms duty period 
#define PWM_DUTY_N90_DEGREES    (1000)
#define PWM_DUTY_N45_DEGREES    (1250)
#define PWM_DUTY_0_DEGREES      (1500)
#define PWM_DUTY_P45_DEGREES    (1750)
#define PWM_DUTY_P90_DEGREES    (2000)

static const char *TAG = "servo_pwm_control";

// Table of each pin munber
const uint32_t pin_num[1] = {
    PWM_OUTPUT_IO_NUM
};

// Table of initial duty values for each pin
uint32_t duties[1] = {
    1000
};

// Table of initial phase values for each pin
float phase[1] = {
    0
};

void taskServoControlContinuous(void * pvParameters) {
    ESP_LOGI(TAG, "Controlling the servo for continuous motion");
    
    // Initialize the PWM driver with intial duties and phases for each pin
    pwm_init(PWM_PERIOD, duties, 1, pin_num);
    pwm_set_phases(phase);
    pwm_start();

    // Iterate through the servo duty range starting from the minimum value
    int count = 0;
    while (1) {
        int servo_position = PWM_DUTY_N90_DEGREES + count;
        pwm_set_duty(0, servo_position);
        pwm_start();
        ESP_LOGI(TAG, "PWM changed duty to %dms", servo_position);

        count += 5;
        count %= 1000;

        // Block the task during the 10ms sleep duration
        vTaskDelay(10 / portTICK_RATE_MS);
    }
} 

void taskServoControlSteps(void * pvParameters) {
    ESP_LOGI(TAG, "Controlling the servo for fixed position steps");
    
    // Initialize the PWM driver with intial duties and phases for each pin
    pwm_init(PWM_PERIOD, duties, 1, pin_num);
    pwm_set_phases(phase);
    pwm_start();
    
    // Iterate between hardcoded servo positions defined by their duty value
    int16_t count = 0;
    while (1) {
        if (count == 0) {
            pwm_set_duty(0, PWM_DUTY_N90_DEGREES);
            pwm_start();
            ESP_LOGI(TAG, "PWM changed duty to 1.00ms (-90 degrees)\n");
        } else if (count == 5) {
            pwm_set_duty(0, PWM_DUTY_N45_DEGREES);
            pwm_start();
            ESP_LOGI(TAG, "PWM changed duty to 1.25ms (-45 degrees)\n");
        } else if (count == 10) {
            pwm_set_duty(0, PWM_DUTY_0_DEGREES);
            pwm_start();
            ESP_LOGI(TAG, "PWM changed duty to 1.5ms (0 degrees)\n");
        } else if (count == 15) {
            pwm_set_duty(0, PWM_DUTY_P45_DEGREES);
            pwm_start();
            ESP_LOGI(TAG, "PWM changed duty to 1.75ms (45 degrees)\n");
        } else if (count == 20) {
            pwm_set_duty(0, PWM_DUTY_P90_DEGREES);
            pwm_start();
            ESP_LOGI(TAG, "PWM changed duty to 2.0ms (90 degrees)\n");
        }

        count++;
        count = count % 25;

        // Block the task during the 1000ms sleep duration
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
} 

void app_main()
{
    // Create one of the two possible servo control tasks defined above
    xTaskCreate(taskServoControlSteps, "Task Servo Control", 8192, NULL, 5, NULL);
}
