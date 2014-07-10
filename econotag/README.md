REQUIREMENTS
===========

For the code to compile you need to first install ANCIENT 2008 toolchain:
https://github.com/malvira/libmc1322x/wiki/toolchain

To make sure everything is fine:

* compile hello-world:

```cd contiki/examples/hello-world
make TARGET=econotag hello-world```
    
* try loading it onto the chip
    
```contiki/cpu/mc1322x/tools/mc1322x-load.pl -f contiki/examples/hello-world/hello-world_econotag.bin -t DEVICE```

where device is something like /dev/ttyUSB0. After issuing that command press reset button on the chip.
    

MISC
===========

Email szymon.sidor@gmail.com if you have any problems.

