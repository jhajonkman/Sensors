//
//  Sensors 
//  Library C++ code
//  ----------------------------------
//  Developed with embedXcode
//
//  Sensors
//  Created by jeroenjonkman on 22-06-15
// 


#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Sensors.h>

#ifdef Sensors_enableTSL
TSL2561 tsl(TSL2561_ADDR_FLOAT);
#endif

#ifdef Sensors_enableDHT
DHT dht(DHTPIN, DHTTYPE);
#endif

#ifdef Sensors_enableBMP
BMP180 bmp;
#endif

void   Sensors::setup(uint8_t id)
{
    _id = id;
#ifdef Sensors_enableRTC
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet) {
        Serial.println("Unable to sync with the RTC");
    } else {
#ifdef Sensors_temperatureRTC
        bitWrite(_status,SENSORS_TEMPERATURE_RTC_SETUP_BIT,true);
#endif
        bitWrite(_status,SENSORS_TIME_SETUP_BIT,true);
    }
#endif
#ifdef Sensors_enableTSL
    if (tsl.begin()) {
        uint32_t lum = tsl.getFullLuminosity();
        if (lum != 0xffffffff) {
            bitWrite(_status,SENSORS_LIGHT_SETUP_BIT,true);
        }
    }
#endif
#ifdef Sensors_enableDHT
    dht.begin();
    delay(400); // Cold start delay (Uno)
    _temperatureDHT = dht.readTemperature();
    if (!isnan(_temperatureDHT)) {
        bitWrite(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT,true);
    }
    _humidityDHT = dht.readHumidity();
    if (!isnan(_humidityDHT) && !isnan(_temperatureDHT)) {
        bitWrite(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT,true);
    }
#endif
#ifdef Sensors_enableTSL
    if (bitRead(_status,SENSORS_LIGHT_SETUP_BIT)) {
        tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
    }
#endif
#ifdef Sensors_enableBMP
    bmp.begin(BMP180_Mode_HighResolution,false);
#ifdef Sensors_temperatureBMP
    _temperatureBMP = bmp.getTemperature();
    if (!isnan(_temperatureBMP)) {
        bitWrite(_status,SENSORS_BMP_SETUP_BIT,true);
        bitWrite(_status,SENSORS_TEMPERATURE_BMP_SETUP_BIT,true);
    }
#else
    if (!isnan(bmp.getTemperature())) {
        bitWrite(_status,SENSORS_BMP_SETUP_BIT,true);
    }
#endif
#endif
    bitWrite(_status,SENSORS_STATUS_SETUP_BIT,true);
}

bool Sensors::isSetup()
{
    return bitRead(_status,SENSORS_STATUS_SETUP_BIT);
}

#ifdef Sensors_Relays
void Sensors::loop(Relays *relays)
{
    loop();
    if (relays->isSetup()) {
        if (bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
            relays->setTemperature(_temperatureDHT);
        }
#ifdef Sensors_enableTSL
        if (bitRead(_status,SENSORS_LIGHT_SETUP_BIT)) {
            relays->setLight(_lux);
        }
#endif
    }
}
#endif

void Sensors::loop()
{
    unsigned long m_seconds = millis();
    if (_last_run < m_seconds) {
#ifdef Sensors_debug
        Serial.println("S:l");
#endif
#ifdef Sensors_enableRTC
        if ( bitRead(_status,SENSORS_TIME_SETUP_BIT)) {
            loopTime();
        }
#ifdef Sensors_temperatureRTC
        if (_looper%8==1 && bitRead(_status,SENSORS_TEMPERATURE_RTC_SETUP_BIT)) {
            loopTemperatureRTC();
        }
#endif
#endif
#ifdef Sensors_enableDHT
        if (_looper%8==2 && bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
            loopTemperatureDHT();
        }
        if (_looper%8==4 && bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT)) {
            loopHumidityDHT();
        }
#endif
#ifdef Sensors_enableTSL
        if (_looper%8==3 && bitRead(_status,SENSORS_LIGHT_SETUP_BIT)) {
            loopLight();
        }
#endif
#ifdef Sensors_dewPoint
        if (_looper%8==5 && bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT) && bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
            loopDewPoint();
        }
#endif
#ifdef Sensors_enableBMP
        if (_looper%8==6 && bitRead(_status,SENSORS_BMP_SETUP_BIT)) {
            loopBMP();
        }
#endif
#ifdef Sensors_print
        if (_looper%15==0 && _looper > 10) {
            printStatus();
        }
#endif
        _last_run += SENSORS_LOOP_CHECK;
        _looper++;
    }
}

#ifdef Sensors_enableRTC
    time_t Sensors::getTime()
    {
        return _lastTime;
    }
#endif

#ifdef Sensors_enableDHT
    float Sensors::getTemperature()
    {
        return _temperatureDHT;
    }

    float Sensors::getHumidity()
    {
        return _humidityDHT;
    }
#endif

#ifdef Sensors_enableBMP
    long Sensors::getPressure()
    {
        return _pressure;
    }

#endif

#ifdef Sensors_enableTSL
    uint16_t Sensors::getLux()
    {
        return _lux;
    }
    uint16_t Sensors::getIr()
    {
        return _ir;
    }
    uint16_t Sensors::getVisible()
    {
        return _visible;
    }
    uint16_t Sensors::getFull()
    {
        return _full;
    }
#endif


#ifdef Sensors_xbee

uint8_t Sensors::putXBeeData(ByteBuffer *buffer)
{
#ifdef Sensors_enableRTC
    if ( bitRead(_status,SENSORS_TIME_SETUP_BIT)) {
        putXBeeTime(buffer);
    }
#ifdef Sensors_temperatureRTC
    if (bitRead(_status,SENSORS_TEMPERATURE_RTC_SETUP_BIT)) {
        putXBeeTemperatureRTC(buffer);
    }
#endif
#endif
#ifdef Sensors_enableDHT
    if (bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
        putXBeeTemperatureDHT(buffer);
    }
    if (bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT)) {
        putXBeeHumidityDHT(buffer);
    }
#endif
#ifdef Sensors_enableTSL
    if (bitRead(_status,SENSORS_LIGHT_SETUP_BIT)) {
        putXBeeLux(buffer);
        putXBeeIr(buffer);
        putXBeeVisible(buffer);
        putXBeeFull(buffer);
    }
#endif
#ifdef Sensors_enableBMP
    if (bitRead(_status,SENSORS_BMP_SETUP_BIT)) {
#ifdef Sensors_temperatureBMP
        if (bitRead(_status,SENSORS_TEMPERATURE_BMP_SETUP_BIT)) {
            putXBeeTemperatureBMP(buffer);
        }
#endif
        putXBeePressure(buffer);
    }
#endif
#ifdef Sensors_dewPoint
    if (bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT) && bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
        putXBeeDewPoint(buffer);
    }
#endif
    return 0;
}

#ifdef Sensors_enableRTC
uint8_t Sensors::putXBeeTime(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= 5) {
        if(!buffer->put(XBEE_TIME_HEADER)) {
            return 0;
        }
        return buffer->putTime(_lastTime);
    }
    return 0;
}

#ifdef Sensors_temperatureRTC
uint8_t Sensors::putXBeeTemperatureRTC(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_TEMPERATURE_HEADER | 0x01, (int)_temperatureRTC*SENSORS_FLOAT_TO_INT_MULTIPLY);
}
#endif
#endif

#ifdef Sensors_enableDHT
uint8_t Sensors::putXBeeTemperatureDHT(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_TEMPERATURE_HEADER | 0x02, _temperatureDHT*SENSORS_FLOAT_TO_INT_MULTIPLY);
}

uint8_t Sensors::putXBeeHumidityDHT(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_HUMIDITY_HEADER | 0x01, _humidityDHT*SENSORS_FLOAT_TO_INT_MULTIPLY);
}
#endif

#ifdef Sensors_enableTSL
uint8_t Sensors::putXBeeLux(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_LUX_HEADER | 0x01, _lux*SENSORS_FLOAT_TO_INT_MULTIPLY);
}

uint8_t Sensors::putXBeeIr(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_IR_HEADER | 0x01, _ir*SENSORS_FLOAT_TO_INT_MULTIPLY);
}

uint8_t Sensors::putXBeeVisible(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_VISIBLE_HEADER | 0x01, _visible*SENSORS_FLOAT_TO_INT_MULTIPLY);
}

uint8_t Sensors::putXBeeFull(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_FULL_HEADER | 0x01, _full*SENSORS_FLOAT_TO_INT_MULTIPLY);
}
#endif

#ifdef Sensors_enableBMP
#ifdef Sensors_temperatureBMP
uint8_t Sensors::putXBeeTemperatureBMP(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_TEMPERATURE_HEADER | 0x03, _temperatureBMP*SENSORS_FLOAT_TO_INT_MULTIPLY);
}
#endif
uint8_t Sensors::putXBeePressure(ByteBuffer *buffer)
{
    return putXBeeLong(buffer, XBEE_PRESSURE_HEADER | 0x01, (long)_pressure);
}
#endif

#ifdef Sensors_dewPoint
uint8_t Sensors::putXBeeDewPoint(ByteBuffer *buffer)
{
    return putXBeeInt(buffer, XBEE_DEWPOINT_HEADER | 0x01, _dewpoint*SENSORS_FLOAT_TO_INT_MULTIPLY);
}
#endif

#endif  //Sensors_xbee

#ifdef Sendors_status
String Sensors::getStatus()
{
    String status = "";
#ifdef Sensors_enableRTC
    status += stringTime();
    status += "\n";
#ifdef Sensors_temperatureRTC
    status += stringTemperatureRTC();
    status += "\n";
#endif
#endif
#ifdef Sensors_enableDHT
    status += stringTemperatureDHT();
    status += "\n";
    status += stringHumidityDHT();
    status += "\n";
#endif
#ifdef Sensors_enableTSL
#endif
    return status;
}
#endif

#ifdef Sensors_xbee

uint8_t Sensors::putXBeeInt(ByteBuffer *buffer, uint8_t sensor, int value)
{
    if (buffer->getFreeSize() >= 5) {
        if(buffer->put(XBEE_SENSOR_HEADER)) {
            if(buffer->put(sensor)) {
                return buffer->putInt(value);
            }
        }
    }
    return 0;
}

/*
uint8_t Sensors::putXBeeFloat(ByteBuffer *buffer, uint8_t sensor, float value)
{
    if (buffer->getFreeSize() >= 5) {
        if(buffer->put(XBEE_SENSOR_HEADER)) {
            if(buffer->put(sensor)) {
                return buffer->putFloat(value);
            }
        }
    }
    return 0;
}
*/

uint8_t Sensors::putXBeeLong(ByteBuffer *buffer, uint8_t sensor, long value)
{
    if (buffer->getFreeSize() >= 5) {
        if(buffer->put(XBEE_SENSOR_HEADER)) {
            if(buffer->put(sensor)) {
                return buffer->putLong(value);
            }
        }
    }
    return 0;
}

#endif

#ifdef Sensors_enableRTC
void Sensors::loopTime()
{
    _lastTime = now();
}

#ifdef Sensors_temperatureRTC
void Sensors::loopTemperatureRTC()
{
    int t = RTC.temperature();
    float celsius = t / 4.0;
    _temperatureRTC = celsius;
}
#endif
#endif

#ifdef Sensors_enableDHT
void Sensors::loopTemperatureDHT()
{
    _temperatureDHT = dht.readTemperature();
#ifdef Sensors_dewPoint
    if(!isnan(_humidityDHT))
        _dewpoint = dewPoint(_temperatureDHT,_humidityDHT);
#endif
}

void Sensors::loopHumidityDHT()
{
    float humidity = dht.readHumidity();
    if( !isnan(humidity) )
        _humidityDHT = humidity;
#ifdef Sensors_dewPoint
    if(!isnan(_temperatureDHT))
        _dewpoint = dewPoint(_temperatureDHT,_humidityDHT);
#endif

}
#endif

#ifdef Sensors_dewPoint
void Sensors::loopDewPoint()
{
}
#endif

#ifdef Sensors_enableTSL
void Sensors::loopLight()
{
    uint32_t lum = tsl.getFullLuminosity();
    if (lum == 0xffffffff) {
        return;
    }
    _ir = lum >> 16;
    _full = lum & 0xFFFF;
    _lux = tsl.calculateLux(_full, _ir);
    if (_ir<_full) {
        _visible = _full-_ir;
    } else {
        _visible = 0;
    }
}
#endif

#ifdef Sensors_enableBMP
void Sensors::loopBMP()
{
    //bmp.PrintCalibrationData();
    _pressure = bmp.getPressure();
#ifdef Sensors_temperatureBMP
    _temperatureBMP = bmp.getTemperature();
#endif
}
#endif

#ifdef Sensors_enableRTC
String  Sensors::stringTime()
{
    String timeString = "";
    switch (weekday(_lastTime)) {
        case 1:
            timeString += "Zo ";
            break;
        case 2:
            timeString += "Ma ";
            break;
        case 3:
            timeString += "Di ";
            break;
        case 4:
            timeString += "Wo ";
            break;
        case 5:
            timeString += "Do ";
            break;
        case 6:
            timeString += "Vr ";
            break;
        case 7:
            timeString += "Za ";
            break;
        default:
            timeString += "No ";
            break;
    }
    timeString += stringDigits(hour(_lastTime));
    timeString += ":";
    timeString += stringDigits(minute(_lastTime));
    timeString += ":";
    timeString += stringDigits(second(_lastTime));
    timeString += " ";
    timeString += stringDigits(day(_lastTime));
    timeString += "-";
    timeString += stringDigits(month(_lastTime));
    timeString += "-";
    timeString += stringDigits((year(_lastTime) - 2000));
    return timeString;
}

String Sensors::stringDigits(int digits){
    String digitsString = "";
    if(digits < 10) {
        digitsString += "0";
    }
    digitsString.concat(digits);
    return digitsString;
}

#ifdef Sensors_temperatureRTC
String Sensors::stringTemperatureRTC()
{
    String text = "";
    if (!isnan(_temperatureRTC)) {
        text += "Temp:";
        text += _temperatureRTC;
        text += "C";
    }
    return text;
}
#endif
#endif

#ifdef Sensors_enableDHT
String Sensors::stringTemperatureDHT()
{
    String text = "";
    if (!isnan(_temperatureDHT)) {
        text += "Temp:";
        text += _temperatureDHT;
        text += "C";
    }
    return text;
}

String Sensors::stringHumidityDHT()
{
    String text = "";
    if (!isnan(_humidityDHT)) {
        text += "Humi:";
        text += _humidityDHT;
        text += "%";
    }
    return text;
}
#endif

#ifdef Sensors_dewPoint
string Sensors::stringDewpoint()
{
    String text = "";
    if (!isnan(_dewpoint ) ) {
        text += ", DewP:";
        text += _dewpoint;
        text += "°";
    }
    return text;
}
#endif

#ifdef Sensors_enableTSL
String Sensors::stringLight()
{
    String text = "";
    text += ", Lux:";
    text += _lux;
    text += ", IR:";
    text += _ir;
    text += ", VIS:";
    text += _visible;
    text += ", Lum:";
    text += _full;
    return text;
}
#endif

#ifdef Sensors_enableBMP
String Sensors::stringBMP()
{
    String text = "";
#ifdef Sensors_temperatureBMP
    if (!isnan(_temperatureBMP)) {
        text += "Temp:";
        text += _temperatureBMP;
        text += "C (BMP)";
    }
#endif
    text += ", Press:";
    text += (float)_pressure/100;
    return text;
}
#endif

#ifdef Sensors_print
void Sensors::printStatus()
{
#ifdef Sensors_enableRTC
    if (bitRead(_status,SENSORS_TIME_SETUP_BIT)) {
        Serial.print(stringTime());
    }
#ifdef Sensors_temperatureRTC
    if (bitRead(_status,SENSORS_TEMPERATURE_RTC_SETUP_BIT)) {
        Serial.print(", ");
        Serial.print(stringTemperatureRTC());
        Serial.print(" (RTC)");
    }
#endif
#endif
#ifdef Sensors_enableDHT
    if (bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
        Serial.print(", ");
        Serial.print(stringTemperatureDHT());
        Serial.print(" (DHT)");
    }
    if (bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT)) {
        Serial.print(", ");
        Serial.print(stringHumidityDHT());
    }
#endif
#ifdef Sensors_dewPoint
    if (bitRead(_status,SENSORS_HUMIDITY_DHT_SETUP_BIT) && bitRead(_status,SENSORS_TEMPERATURE_DHT_SETUP_BIT)) {
        Serial.print(stringDewpoint());
    }
#endif
#ifdef Sensors_enableTSL
    if (bitRead(_status,SENSORS_LIGHT_SETUP_BIT)) {
        Serial.print(stringLight());
    }
#endif
#ifdef Sensors_enableBMP
    if (bitRead(_status,SENSORS_BMP_SETUP_BIT)) {
        Serial.print(stringBMP());
    }
#endif
    Serial.println();
#ifdef __AVR_ATmega32U4__
    if (Serial1) {
        Serial1.println();
    }
#endif
}
#endif

#ifdef Sensors_dewPoint
// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
#ifndef Sensors_dewPointFast
double Sensors::dewPoint(double celsius, double humidity)
{
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
    RHS += log10(1013.246);
    
    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;
    
    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558 - T);
}
#else
// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double Sensors::dewPoint(double celsius, double humidity)
{
    double a = 17.271;
    double b = 237.7;
    double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
    double Td = (b * temp) / (a - temp);
    return Td;
}
#endif
#endif

