# STM32F4 Firmware for a swashplateless cyclic blade pitch controller

This STM32F4 firmware generates a Dshot2400 throttle signal with an appropirate sine wave component given the following input parameters (via i2c packets): base throttle, angle offset and scaler (see image below) 

![](https://github.com/MacroMelon/swashplateless-cyclic-blade-pitch-controller/blob/master/throttle_explanation.png)

The throttle output update rate is around 14 kHz with the current firmware, utilising a simplified sine function or potentially some form of lookup table should allow for much higher loop rates

### todo - finish readme
