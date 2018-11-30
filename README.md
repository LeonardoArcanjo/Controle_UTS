# UTS_Module

This code is a .ino file for to control the UTS-CC module from Newport INC. That module works with a motor DC. 

A Arduino Mega 2560 was used with a Brigde H to control the motor DC. 

The encoder pins was coupled to interrupts pins from Arduino and when the IRQ are actived, a counter is increased or descreased, depends on the move direction of the body coupled to lead screw. 
