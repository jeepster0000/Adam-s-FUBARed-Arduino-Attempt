#include <Encoder.h>
 
/*
This code links a rotary encoder to the NAV1 OBS. If you turn the
encoder quickly, the bug changes quickly and you have fast control; if
you turn the encoder slowly, the bug changes slowly and you have fine
control.
 
This is an extension (and substantial rewrite) of the plain encoder-
reading code from before.
*/
 
 
// Input from hardware
Encoder Nav1HzEnc(24, 25); // encoder on pins 26 and 27
short Nav1HzEncPrevious = 0;
 
elapsedMillis Nav1HzEncClickInterval = 0;
 
// Input/Output with simulator
FlightSimFloat Nav1HzDataRef;
 
 
void setup() {
  Nav1HzDataRef = XPlaneRef("sim/cockpit2/radios/actuators/nav1_frequency_Mhz");
}
 
 
void loop() {
  FlightSim.update();
 
  // divide by 4 to find how many clicks the encoder's been turned
  // (+ve clockwise, -ve anticlockwise, normally)
  short Nav1HzClicks = (Nav1HzEnc.read() - Nav1HzEncPrevious) / 10;
 
  // when encoder 'clicks' into a new detent:
  if (Nav1HzClicks != 0) {
 
    // change in degrees from current stored value (can be <0 )
    float Nav1HzChange = 0;
 
    // Threshold between FINE and FAST control: 30ms per click
    if (Nav1HzEncClickInterval > 30) {
      // FINE/slow mode. Change by 0.25 degrees per click.
      Nav1HzChange = Nav1HzClicks * 0.1;
    } else {
      // FAST/coarse mode. Change by 5 degrees per click.
      Nav1HzChange = Nav1HzClicks * 5.0;
    }
 
    // new value = current value plus changes
    float Nav1HzNewValue = Nav1HzDataRef + Nav1HzChange;
 
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Nav1HzNewValue  <   108.00) Nav1HzNewValue += 118.00;
    while (Nav1HzNewValue >= 119.00) Nav1HzNewValue -= 108.00;
 
    // write validated new heading back to dataref
    Nav1HzDataRef = Nav1HzNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    Nav1HzEncPrevious = 0;
    Nav1HzEnc.write(0);
    Nav1HzEncClickInterval = 0;
  }
}
