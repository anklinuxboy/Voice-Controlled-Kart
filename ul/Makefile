CC = $(EC535)/gumstix/oe/cross/bin/arm-linux-gcc

#CC = $(EC535)/arm-linux/bin/arm-linux-gcc

IPATH = -I/ad/eng/courses/ec/ec535/gumstix/bluez/include
LPATH = -L/ad/eng/courses/ec/ec535/gumstix/bluez/lib

objects = l2cap-server.o

l2cap-server : $(objects) 
	$(CC) -o l2cap-server $(objects) -lbluetooth $(IPATH) $(LPATH)

.PHONY : clean

clean : 
		rm l2cap-server $(objects)
