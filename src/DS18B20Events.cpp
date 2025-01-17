#include "DS18B20Events.h"

static unsigned long intervalMillis;
static unsigned long intervalMillisMax;
static unsigned long lastMillisRequest;
static DallasTemperature* _sensors;

DS18B20Events::DS18B20Events(uint8_t index)
{
	_index = index;
}

DS18B20Events::~DS18B20Events() {

}

float DS18B20Events::getTempC(void)
{
	return currentTempC;
}
unsigned long DS18B20Events::getMillis()
{
	return lastMillisGet;
}
uint8_t DS18B20Events::getIndex()
{
	return _index;
}
void DS18B20Events::setInterval(unsigned long value, unsigned long valueMax)
{
	if (value < minInterval) { intervalMillis = minInterval; }
	else { intervalMillis = value; }
	
	if (valueMax > value) { intervalMillisMax = valueMax; }
	else { intervalMillisMax = value + minInterval * 1000; }
};

void DS18B20Events::setup(DallasTemperature* sensors)
{
	#if _DEBUG
		Serial.println("Setup Thermometer");
	#endif
	_sensors = sensors;
	_sensors->begin();
	intervalMillis = 10000; // default interval
	intervalMillisMax = 10000000; // default max interval
};

void DS18B20Events::loop(void) {
	currentMillis = millis();

	if (currentMillis - lastMillisRequest >= intervalMillis || lastMillisRequest == 0)
	{
		#if _DEBUG
			Serial.println("Requesting temperature ");
		#endif
		_sensors->requestTemperatures();
		lastMillisRequest = currentMillis;
	}

	if (currentMillis - lastMillisGet >= intervalMillis || lastMillisGet == 0)
	{
		#if _DEBUG
			Serial.print("Get temperature at ");
			Serial.println(_index);
		#endif
		currentTempC = _sensors->getTempCByIndex(_index);

		if (currentTempC != lastTempC || currentMillis - lastMillisCB >= intervalMillisMax)
		{
			#if _DEBUG
				Serial.print("DEBUG ");
				Serial.print(currentTempC);
				Serial.print(" new TempC at ");
				Serial.println(_index);
			#endif	

			if (onChange != nullptr) { onChange(_index, currentTempC); }
			lastMillisCB = currentMillis;
		}
		lastMillisGet = currentMillis;
		lastTempC = currentTempC;
	}	
}
