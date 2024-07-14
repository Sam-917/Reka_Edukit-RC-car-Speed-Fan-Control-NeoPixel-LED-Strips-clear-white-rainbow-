This file contain code and diagram for the Reka Edukit intergrated with rc car feature and ac cooler as well other features provide . 
This project involved in 2 sections , one on the Reka Edukit and other on ESP32 . 

One side controled by an Arduino Uno as the brain , controlling the movement and mobility of the rc car using bluetooth module Hc-05 as well other features like 8 bit Neo Pixel Led and cotroll the speed dc motor using the  l293n motor driver provide by Reka Edukit . 
For the main part that is the mobility for the rc car , we using l298n motor driver to control and attach the pin on to pin 8, 9 ,10 , 11, 12, 13 . 
In fact all digital pin on Arduino Uno is full ! except pin 0 & 1 for uploading purpose . 

On the other side is controlled by an ESP32 for IOT purposed . For IOT platform we will be using Blynk . We will be displaying values like TDs , Temperature , Electrical Conductivity & Water Quality Percentage . We also adding switch to control Servo 1 & Servo 2 for other purposed  . 

