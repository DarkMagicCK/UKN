//
//  Sensor.h
//  Project Unknown
//
//  Created by Robert Bu on 12/10/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Sensor_h
#define Project_Unknown_Sensor_h

#include "UKN/Platform.h"
#include "UKN/Basic.h"
#include "UKN/Event.h"

#include <list>

namespace ukn {
	
	class ISensor: public virtual Interface {
	public:
		virtual bool isAvailable() const = 0;
		virtual bool isActive() const = 0;
		
		virtual void activate() = 0;
		virtual void deactivate() = 0;
		
		virtual ISensor* clone() = 0;
	};
	
	template<typename T>
	class Sensor: public virtual Object, public ISensor {
	public:
		typedef Event<T> SensorEventType;
		typedef Function<void(void*/*original sender*/, Sensor*/*triggered sensor*/, T> SensorTriggerFuncType;
		
		Sensor();
		Sensor(const SensorTriggerFuncType& func);
		Sensor(const Sensor& rhs);
		virtual ~Sensor();
		
		bool isAvailable() const;
		bool isActive() const;
		
		void activate();
		void deactivate();
		
		ISensor* clone();
		
		void onSensorEvent(void* sender, T& eventType);
		
	private:
		bool mActive;
		bool mAvailable;
		
		SensorTriggerFuncType mFunc;
	};
    
    class SensorManager: private Uncopyable {
    protected:
    	SensorManager();
    	~SensorManager();
    	
    public:
    	void addSensor(ISensor* sensor);
    	void removeSensor(ISensor* sensor);
    	
    	
    	
    private:
    	typedef std::list<SharedPtr<ISensor> > SensorList;
    	SensorList mSensors;
    };
    
} // namespace ukn

#endif
