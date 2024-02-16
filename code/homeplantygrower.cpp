#include "homeplantygrower.hpp"

namespace homeplantygrower
{
    volatile int light = 0;
    float temperature = 0;
    float humidity = 0;

    grower::grower()
    {
        // Initializes the serial monitor
        Serial.begin(9600);

        ADMUX |= (1 << REFS0); // Sets the reference voltage to AVCC
        ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

        ADCSRA |= (1 << ADEN) // Enable ADC

            // Enable Timer interrupt for the light sensor
            TCCR1B |= (1 << WGM12); // Configure timer for CTC mode
        OCR1A = 18749;              // Set compare math value for 5 minutes at 16Mhz
        TIMSK1 |= (1 << OCIE1A);    // Enable CTC interrupt

        // Enable interrupts
        sei();
    }

    grower::void readLight()
    {
        ADCSRA |= (1 << ADIE); // Enable ADC interrupt
        ADCSRA |= (1 << ADSC); // Start A2D conversions

        // Wait for the conversion to complete
        while (ADCSRA & (1 << ADSC))
        {
        }

        // Read the ADC value
        byte low = ADCL;
        byte high = ADCH;
        int light = (high << 8) | low;

        // Prints the light value on the serial monitor
        Serial.print("Light: ");
        Serial.println(light);
    }

    ISR(TIMER1_COMPA_vect)
    {
        readLight(); // Trigger light reading every 5 minutes
    }

    ISR(ADC_vect)
    {
        // Nothing to do here as ADC reading is handled in readLight()
    }

grower::void readTemperature()
{
    // Define the dht sensor
    #define DHTPIN 10
    #define DHTTYPE DHT11
    DHT dht(DHTPIN, DHTTYPE);
    dht.begin();
    // Read the temperature and humidity
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    // Check if any reads failed
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read Data from DHT sensor!");
        return;
    }
    // Prints the temperature and humidity on the serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C ");

}

grower::void readWaterLevel
{
    ADCSRA |= (1 << ADIE); // Enable ADC interrupt
    ADCSRA |= (1 << ADSC); // Start A2D conversions

    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC))
    {
    }

    // Read the ADC value
    byte low = ADCL;
    byte high = ADCH;
    water_level = (high << 8) | low;
}
} // namespace homeplantygrower