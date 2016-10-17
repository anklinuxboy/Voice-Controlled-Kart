# Voice Controlled Robot Kart
#### Final Project for EC500 Embedded Systems taught by [Dr. Ayse Coskun](http://www.bu.edu/eng/profile/ayse-coskun) at Boston University College of Engineering

#### Team BlueGum
*Ankit Sharma Laura Kamfonik Juan Jhong Chung*

#### Project Components
##### Hardware:
1. Raspberry Pi B+
2. Gumstix Verdex Pro
3. Speaker and Microphone
4. Robot Kart from Sparkfun

##### Software:
1. Kernel Module in linux C
2. [Jasper](https://jasperproject.github.io) Open Source Voice Processing Software
3. Kart control code in C
4. L2cap Bluetooth Python and C client server module

#### Instructions
-----------------
##### Kernel Module:
Compile gum_km.c using the included Makefile and
load the file gum_km.ko onto the gumstix board using
minicom or another method of your choice.

On the Gumstix, install the kernel module with the
following command, replacing /path/to/ with the appropriate path to the file:
```
> insmod /path/to/gum_km.ko
```
To perform simple tests on the kernel module, use 
```
> echo [command] > /proc/robokart
```
where [command] is an integer corresponding to the desired command.

The mapping of integers to commands is as follows:
```
0 - manual stop
1 - go forward
2 - go backward
3 - turn right
4 - turn left
5 - set speed to fast (default, PWM disabled)
6 - set speed to slow (enable PWM)
7 - dance
8 - set move_time to 5 seconds
9 - set move_time to 10 seconds
```
For example,
```
> echo 1 > /proc/robokart
```
should turn on GPIO pins to make the kart go forward. 
When the kart is not connected, LEDs or other indicators can be connected
to the GPIO pins and used to test the status of the GPIO pins.
Note that the PWM settings that move the kart do not work to change
the brightness of an LED when speed is set to slow.

##### User Application Module:
Compile l2cap-server using the include Makefile and load the application l2cap-server
into the gumstix. Start the application by typing 
```
./l2cap-server &
```
to run in the background. No other input is needed by the user. The l2cap server waits for the 
Bluetooth messages and passes them to kernel space using procfs.

##### Jasper:
Copy the files in this folder to your installation folder of jasper
for example /home/pi/jasper/client/modules - Make sure that this files are part of the 
"modules folder" for Jasper to load it

#### YouTube Video Link
<a href="http://www.youtube.com/watch?feature=player_embedded&v=4uqO_HJrflc
" target="_blank"><img src="http://img.youtube.com/vi/4uqO_HJrflc/0.jpg" 
alt="Robot Kart" width="240" height="180" border="10" /></a>
