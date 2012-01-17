#include "UKN/Sensor.h"

namespace ukn {

	Sensor::Sensor():
	mActive(false),
	mAvailable(true) {
	
	}
	
	Sensor(const SensorTriggerFuncType& func):
	mActive(false),
	mAvailable(true),
	mFunc(func) {
	
	}
	
	Sensor(const Sensor& rhs):
	mActive(rhs.mActive),
	mAvailable(rhs.mAvailable),
	mFunc(rhs.mFunc) {
	}
	
	Sensor::~Sensor() {
	
	}
		
	bool Sensor::isAvailable() const {
		return mAvailable;
	}
	
	bool Sensor::isActive() const {
		return mActive;
	}
	
	void Sensor::activate() {
		mActive = true;
	}
	
	void Sensor::deactivate() {
		mActive = false;
	}
		
	ISensor* Sensor::clone() {
		return new Sensor(*this);	
	}
		
	void Sensor::onSensorEvent(void* sender, T& eventType) {
		if(mAvailable && mActive) {
			mFunc(sender, this, eventType);
		}
	}

} // namespace ukn