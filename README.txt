km:
Compile gum_km.c using the included Makefile and
load the file gum_km.ko onto the gumstix board using
minicom or another method of your choice.

On the Gumstix, install the kernel module with the
following command, replacing /path/to/ with the appropriate path to the file:
> insmod /path/to/gum_km.ko

To perform simple tests on the kernel module, use "echo [command] > /proc/robokart"
where [command] is an integer corresponding to the desired command.
The mapping of integers to commands is as follows: 

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

For example,
> echo 1 > /proc/robokart

should turn on GPIO pins to make the kart go forward. 
When the kart is not connected, LEDs or other indicators can be connected
to the GPIO pins and used to test the status of the GPIO pins.
Note that the PWM settings that move the kart do not work to change
the brightness of an LED when speed is set to slow.

ul:
Compile l2cap-server using the include Makefile and load the application l2cap-server
into the gumstix. Start the application by typing ./l2cap-server & to run in the 
background. No other input is needed by the user. The l2cap server waits for the 
Bluetooth messages and passes them to kernel space using procfs.

jasper:
Copy the files in this folder to your installation folder of jasper
for example /home/pi/jasper/client/modules - Make sure that this files are part of the 
"modules folder" for Jasper to load it

