obj-m:=virtko.o

KERS :=/lib/modules/$(shell uname -r)/build                                                                                                                                                                                                                                             

all:
	make -C $(KERS) M=$(shell pwd) modules

clean:
	make -C $(KERS) M=$(shell pwd) clean