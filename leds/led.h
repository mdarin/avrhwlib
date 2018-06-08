#ifndef LEDS_H_
#define LEDS_H_

#ifdef __cplusplus
extern "C" {
#endif

// atmega32
#if defined(__AVR_ATmega168__)
#define GREEN_LED_PIN    PD6
#define RED_LED_PIN      PD7       
#endif

#define LED_DDR DDRD
#define LED_PORT PORTD

#define leds_init() LED_DDR |= (1 << GREEN_LED_PIN) | (1 << RED_LED_PIN)

#define green_led_on() {LED_PORT |= (1<<GREEN_LED_PIN);} // �������� ������� ����.
#define green_led_off() {LED_PORT &= ~(1<<GREEN_LED_PIN);} // ��������� ������� ����.

#define red_led_on() {LED_PORT |= (1<<RED_LED_PIN);} // �������� ������� ����.
#define red_led_off() {LED_PORT &= ~(1<<RED_LED_PIN);} // ��������� ������� ����.

#ifdef __cplusplus
}
#endif

#endif// LEDS_H_
