# How to activate this driver?
> Just enter command 'make' will makes driver .ko file. However, you might modify Makefile.
# What is app.c?
> This app will work intentionally in ZCU106 + Petalinux + AXI GPIO. <br>
> petalinux-build -t modules -n <module name> --enable <br>
> <petalinux-project directory>/project-spec/recipes-modules/.../<module name>/<module name.c> -> modify this source code. <br>
> petalinux-build -c <module name> <br>
> Move .ko file into petalinux SD card -> insmod *.ko -> gcc app.c -> ./a.out. 
