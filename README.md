# Control Chain Arduino Library

## Installation

This library uses:

* [TimerOne](https://code.google.com/p/arduino-timerone/downloads/list)
* [DueTimer](https://github.com/ivanseidel/DueTimer/releases)

which are already included in this repository.

## QuickStart:

To make an Arduino ControlChain device, we recommend you to checkout our examples first, being config.h and *.ino files the most relevant files to the example.

Basically, what you have to do is:

1. create your config.h file (based on the examples) which will describe your device limitations.

2. create your project.ino file.

#### Glossary:

* Device: It's what holds and manages the actuators.
* Actuator: Something that generates 1 value per read. That value can be an integer or not. Also, it's a generic class in our lib which is in charge of common necessities of the actuators.
* Impl-Actuator: These are types of actuators we already implemented, they are less generic representations where you already have an idea about how the actuator works.(e.g. Linear Sensor is one of them and it works for any sensor that has a linear scale, like a potentiometer or a touch screen axis.)
* Assignment: At mod-ui, when you have a plugin parameter associated to an actuator (like a knob or a footswitch), thats an assignment.
* Mode: This is related to the lv2 port properties. In case you are writing your own impl-actuator class, you'll have to declare how many modes your kind of actuator will support (e.g. Our button class supports a toggle, a trigger and a tap tempo mode, so when a parameter needs one of these modes to be assigned, the actuator you created inheriting the button class will appear on the assingment list.) For most cases, our Impl-Actuators probably will do the job. Modes are shared by different actuators in our library.

### Configuring config.h:

* You must declare how many actuators your device will have (e.g. Accelerometer example works with 3 axis, each axis works like an actuator.)
* how many assignments your device will be able to hold (This pre-defined aiming to have only static alocations, since arduino can have a really small memory and using dinamic allocation could crash the device after using it for a while due to memory fragmentation.) Each actuator must have at least 1 assignment, but it can hold more than one. The number of assignments that an actuator hold is declared at the Impl-Actuator's constructor.
* This will be changed, but now you have to declare the sum of modes your actuators share. so if you have 2 actuators inheriting LinearSensor and a actuator inheriting Button, you'll have 4 modes: 3 from Button and 1 from LinearSensor (they share the same mode array).
(Only have to configure the defines below if you have a visual output in your device, otherwise it doesn't make sense to have values different from 0.)
* The max number of scalepoints is a define because, again, arduino can have a small memory and, since theres no forecast of how many scalepoints an assignment will have, we limited the number of possible scalepoints (which are basically string + float) so you don't have to take the risk of fragmentating arduino's memory during use or causing a crash between heap and stack.
* The max number of strings works similarly, but this number of strings will supply the scalepoints and both assignment's label and unit.

### The .ino file:

* First thing, including controlchain.h and the header of the Impl-actuators (e.g. button.h)
* Create a class that inherit an already existent impl-actuator (or one that you made)
    - When creating the Constructor of your class, you have to fulfill the Impl-actuator's constructor in a initialization list.
    - Inside the constructor, you may have to assign values to "maximum" and "minimum" attributes, which are already declared in the father class.(These values correspond to your actuator's raw limits.)
    - Also, you'll have to make a "float getValue()" method, which will return a float value so that the Impl-Actuator class can prepare the value according to the assignment made during execution.

    *Example:*

```c++
#include "controlchain.h"
#include "button.h"
#define BUTTON_PIN      A1

class Butt: public Button{
public:

        Butt(char* name, uint8_t id):Button(name, id, 1 /*num_assignments*/, 50/*debounce delay, if 0, no debounce.*/){
            maximum = 1;
            minimum = 0;
            pinMode(BUTTON_PIN, INPUT);
        }

        float getValue( ){
            return digitalRead(BUTTON_PIN);
        }

    };
```

* Instantiate the Device, ControlChain and your actuators classes.

    *Example:*

```c++
Device dev("http://portalmod.com/devices/accel" /*Device URL*/, "Accelerino" /*Device Label*/, 1 /*Device channel, in case there's more than one of the same.*/);
ControlChain arduinodev; // This class holds the device class and the communication system together.
Accel act1("Sensor X", 1, &ax); // these are Accel class instances we implemented in our case_accel.
Accel act2("Sensor Y", 2, &ay);
Accel act3("Sensor Z", 3, &az);
```

* Implement "void setup()" function, which will run once before loop(). In this function you must add the actuators you created to the device (device class has an actuator pointer array which will point to the added actuators.) , init the ControlChain instance and the Device instance.

    *Example:*
```c++
arduinodev.init(&dev); // it receives a device address as argument.
dev.addActuator(&act1); // receives a actuator address as argument.
dev.addActuator(&act2);
dev.addActuator(&act3);
dev.init(); // after the actuators are added to the device, you can initialize it.
```

* Implement "void loop()" function. In this function you should call Device::run() method, which will take care of the required tasks.

    *Example:*

```c++
void loop(){
    accelgyro.getAcceleration(&ax, &ay, &az); // this line is used in the case_accel, but is not obligatory.
    dev.run();
};
```

After these steps, you should have a ControlChain device ready to use with MOD =)!