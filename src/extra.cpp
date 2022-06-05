#include <common_setup.h>

/*

    LED

*/
Led::Led(uint8_t pin)
{
    pin_ = pin;
    inverted_ = false;
    pinMode(pin, OUTPUT);
    off();
}

Led::Led(uint8_t pin, bool invertedOutput)
{
    pin_ = pin;
    pinMode(pin, OUTPUT);
    inverted_ = invertedOutput;
    off();
}

bool Led::status()
{
    return led_on_;
}

void Led::on()
{
    digitalWrite(pin_, inverted_ ? LOW : HIGH);
    led_on_ = true;
}

void Led::off()
{
    digitalWrite(pin_, inverted_ ? HIGH : LOW);
    led_on_ = false;
}

void Led::toggle()
{
    // If LED was on; turn it off, vice versa
    led_on_ ? off() : on();
    // {{condition}} ? {{do if true}} : {{do if false}};
}

void Led::blink(uint8_t blinkCount, uint32_t delayTime)
{
    Serial.printf("Blinking LED %d times in %.2f seconds\n", blinkCount, blinkCount * delayTime * 2 / 1000.0);
    for (size_t i = 0; i < blinkCount; i++)
    {
        on();
        delay(delayTime);
        off();
        delay(delayTime);
    }
}

/*

    awakeTime

*/
uint16_t stayAwakeTime;
void setStayAwakeTime(uint16_t time)
{
    stayAwakeTime = time;
}

uint16_t getStayAwakeTime()
{
    return stayAwakeTime;
}

/*

    Battery level

*/
uint16_t batteryLevel;
void measureBatteryLevel()
{
    uint8_t switch_pin = 13;
    uint8_t adc_pin = 14;

    pinMode(switch_pin, OUTPUT);
    pinMode(adc_pin, INPUT);

    digitalWrite(switch_pin, HIGH);
    delay(1000);
    batteryLevel = analogRead(adc_pin);
    delay(1000);
    digitalWrite(switch_pin, LOW);
}

uint16_t getBatteryLevel()
{
    return batteryLevel;
}