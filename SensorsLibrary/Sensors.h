//
//  Sensors 
//  Header
//  ----------------------------------
//  Developed with embedXcode
//
//  Sensors
//  Created by jeroenjonkman on 22-06-15
//

#ifndef Sensors_h
#define Sensors_h
#include <Time.h>
#include <Wire.h>
#include <DS3232RTC.h>
#include <DHT.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif



#define SENSORS_LOOP_CHECK 1000

//#define Sensors_debug
//#define Sensors_dewPoint
//#define Sensors_dewPointFast
//#define Sensors_print
//#define Sendors_status
#define Sensors_xbee
#define Sensors_Relays
#define Sensors_enableRTC
#define Sensors_enableTSL
#define Sensors_enableDHT
#define Sensors_enableBMP
#define Sensors_temperatureRTC
#define Sensors_temperatureBMP

#ifdef Sensors_enableTSL
#include <TSL2561.h>
#endif

#ifdef Sensors_enableBMP
#include <BMP180.h>
#endif

#ifdef Sensors_xbee
#include <ByteBuffer.h>
#endif

#ifdef Sensors_Relays
#include <Relays.h>
#endif

#define SENSORS_STATUS_SETUP_BIT            0
#define SENSORS_TIME_SETUP_BIT              1
#ifdef Sensors_temperatureRTC
#define SENSORS_TEMPERATURE_RTC_SETUP_BIT   2
#endif
#define SENSORS_TEMPERATURE_DHT_SETUP_BIT   3
#define SENSORS_HUMIDITY_DHT_SETUP_BIT      4
#define SENSORS_LIGHT_SETUP_BIT             5
#define SENSORS_BMP_SETUP_BIT               6
#ifdef Sensors_temperatureBMP
#define SENSORS_TEMPERATURE_BMP_SETUP_BIT   7
#endif

#define SENSORS_FLOAT_TO_INT_MULTIPLY       100

#define DHTPIN 7
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define XBEE_TIME_HEADER            0x10
#define XBEE_SENSOR_HEADER          0x40
#define XBEE_POWER_HEADER           0x80

#define XBEE_TEMPERATURE_HEADER     0x01 << 3   // T
#define XBEE_HUMIDITY_HEADER        0x03 << 3   // H
#define XBEE_DEWPOINT_HEADER        0x04 << 3   // D
#define XBEE_LUX_HEADER             0x08 << 3   // L
#define XBEE_IR_HEADER              0x09 << 3   // I
#define XBEE_VISIBLE_HEADER         0x0A << 3   // V
#define XBEE_FULL_HEADER            0x0B << 3   // F
#define XBEE_PRESSURE_HEADER        0x0C << 3   // P

class Sensors
{
public:

    void setup(uint8_t id = 0);
    
    bool isSetup();
    
    void loop();
#ifdef Sensors_Relays
    void loop(Relays *relays);
#endif
    
#ifdef Sensors_enableRTC
    time_t getTime();
#endif
#ifdef Sensors_enableDHT
    float getTemperature();
    float getHumidity();
#endif
#ifdef Sensors_enableBMP
    long getPressure();
#endif
#ifdef Sensors_enableTSL
    uint16_t getLux();
    uint16_t getIr();
    uint16_t getVisible();
    uint16_t getFull();
#endif
    
#ifdef Sensors_xbee
    uint8_t putXBeeData(ByteBuffer *buffer);
#ifdef Sensors_enableRTC
    uint8_t putXBeeTime(ByteBuffer *buffer);
#ifdef Sensors_temperatureRTC
    uint8_t putXBeeTemperatureRTC(ByteBuffer *buffer);
#endif
#endif
#ifdef Sensors_enableDHT
    uint8_t putXBeeTemperatureDHT(ByteBuffer *buffer);
#endif
#ifdef Sensors_temperatureBMP
    uint8_t putXBeeTemperatureBMP(ByteBuffer *buffer);
#endif
    uint8_t putXBeeHumidityDHT(ByteBuffer *buffer);
#ifdef Sensors_enableTSL
    uint8_t putXBeeLux(ByteBuffer *buffer);
    uint8_t putXBeeIr(ByteBuffer *buffer);
    uint8_t putXBeeVisible(ByteBuffer *buffer);
    uint8_t putXBeeFull(ByteBuffer *buffer);
#endif
#ifdef Sensors_enableBMP
    uint8_t putXBeePressure(ByteBuffer *buffer);
#endif
#ifdef Sensors_dewPoint
    uint8_t putXBeeDewPoint(ByteBuffer *buffer);
#endif
#endif //Sensors_xbee
#ifdef Sendors_status
    String getStatus();
#endif
    

private:
    uint8_t         _id             =   0;
    uint16_t        _status         =   0x0000;         // SENSORS_STATUS_*
    uint8_t         _looper         =   0;

    unsigned long   _last_run       =   0;
#ifdef Sensors_enableRTC
#ifdef Sensors_temperatureRTC
    float           _temperatureRTC =   NAN;
#endif
#endif
    float           _temperatureDHT =   NAN;
#ifdef Sensors_temperatureBMP
    float           _temperatureBMP =   NAN;
#endif
    float           _humidityDHT    =   NAN;
#ifdef Sensors_dewPoint
    int             _dewpoint       =   0;
#endif
#ifdef Sensors_enableBMP
    long            _pressure       =   0;
#endif
#ifdef Sensors_enableTSL
    uint16_t        _lux            =   0;
    uint16_t        _ir             =   0;
    uint16_t        _visible        =   0;
    uint16_t        _full           =   0;
#endif
#ifdef Sensors_enableRTC
    time_t          _lastTime       =   0x0;
#endif
    
#ifdef Sensors_enableRTC
    void        loopTime();
#ifdef Sensors_temperatureRTC
    void        loopTemperatureRTC();
#endif
#endif
#ifdef Sensors_enableDHT
    void        loopTemperatureDHT();
    void        loopHumidityDHT();
#endif
#ifdef Sensors_enableTSL
    void        loopLight();
#endif
#ifdef Sensors_enableBMP
    void        loopBMP();
#endif
#ifdef Sensors_dewPoint
    void        loopDewPoint();
#endif
    
#ifdef Sensors_xbee
    uint8_t     putXBeeInt(ByteBuffer *buffer, uint8_t sensor, int value);
    //uint8_t     putXBeeFloat(ByteBuffer *buffer, uint8_t sensor, float value);
    uint8_t     putXBeeLong(ByteBuffer *buffer, uint8_t sensor, long value);
#endif
    
#ifdef Sensors_enableRTC
    String      stringTime();
    String      stringDigits(int digits);
#ifdef Sensors_temperatureRTC
    String      stringTemperatureRTC();
#endif
#endif
#ifdef Sensors_enableDHT
    String      stringTemperatureDHT();
    String      stringHumidityDHT();
#endif
#ifdef Sensors_dewPoint
    String      stringDewpoint();
#endif
#ifdef Sensors_enableTSL
    String      stringLight();
#endif
#ifdef Sensors_enableBMP
    String      stringBMP();
#endif
#ifdef Sensors_print
    void        printStatus();
#endif

#ifdef Sensors_dewPoint
    double      dewPoint(double celsius, double humidity);
    double      dewPointFast(double celsius, double humidity);
#endif
};

#endif
