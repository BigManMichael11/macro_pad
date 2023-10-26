/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * 
 * 
 * 
 * https://devzone.nordicsemi.com/guides/nrf-connect-sdk-guides/b/peripherals/posts/twi-ic2-implementation-with-nrfx-twis-driver
 * https://academy.nordicsemi.com/topic/i2c-driver/
 * 
 * 
 */


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>


#define LED0_NODE DT_ALIAS(led0)
const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define BUTTON0_NODE DT_ALIAS(sw0)
const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET(BUTTON0_NODE, gpios);

static struct gpio_callback button0_cb_data;
k_tid_t main_thread_id;


#define I2C1_NODE DT_NODELABEL(io_expander1)
//static const struct i2c_dt_spec ioExpander1 = I2C_DT_SPEC_GET(I2C1_NODE);
//ret = i2c_write_dt(&ioExpander1, writeData, sizeof(writeData));







void button0_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    k_wakeup(main_thread_id);
}


typedef struct
{
    const struct gpio_dt_spec   *dt;
    gpio_flags_t                flags;
} gpio_config_t;

static const gpio_config_t allGpio[] = {
    {&led0,             GPIO_OUTPUT_ACTIVE},
    {&button0,          GPIO_INPUT | GPIO_ACTIVE_LOW | GPIO_PULL_UP}
};


typedef struct
{
    const struct i2c_dt_spec *dev;
} i2c_config_t;

static const i2c_config_t allI2C[] = {
    //{&ioExpander1}
};

bool initGpio(){
    for(int idx = 0; idx < sizeof(allGpio) / sizeof(gpio_config_t); idx++){
        if(!device_is_ready(allGpio[idx].dt->port))
        {
            return false;
        }
        int ret = gpio_pin_configure_dt(allGpio[idx].dt, allGpio[idx].flags);
        if (ret < 0)
        {
            return false;
        }
    }
    return true;
}

bool initI2C(){
    for(int idx = 0; idx < sizeof(allI2C) / sizeof(i2c_config_t); idx++){
        if(!device_is_ready(allI2C[idx].dev->bus)){
            printk("I2C not ready\n");
            return false;
        }
    }
    return true;
}




int main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
    main_thread_id = k_current_get();
    

    while(initGpio() != true)
    {
        k_msleep(50);
    }
    initI2C();

    gpio_pin_interrupt_configure_dt(&button0, GPIO_INT_EDGE_TO_ACTIVE );
    gpio_init_callback(&button0_cb_data, button0_pressed, BIT(button0.pin));
    gpio_add_callback(button0.port, &button0_cb_data);


    while (1){//led testing
        if(gpio_pin_get_dt(&button0)){
            gpio_pin_toggle_dt(&led0);
            k_msleep(500);
        }
    }
   return 1;
}
