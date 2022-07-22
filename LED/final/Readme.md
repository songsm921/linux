# How to activate this driver?
> Just enter command 'make' will makes driver .ko file. However, you might modify Makefile.
# What is app.c?
> This app will work intentionally in ZCU106 + Petalinux + AXI GPIO. <br>
> petalinux-build -t modules -n <module name> --enable <br>
> <petalinux-project directory>/project-spec/recipes-modules/.../<module name>/<module name.c> -> modify this source code. <br>
> petalinux-build -c <module name> <br>
> Move .ko file into petalinux SD card -> insmod *.ko -> gcc app.c -> ./a.out. 

  
## issue 1 (2022.07.22)
> 두 프로세스를 켜놓고 시작하다가 한 프로세스를 terminate시키고 다른 프로세스를 진행하면 segmetation fault 발생. -> 이유는 ioremap을 두번째 하면서 첫번째 한 ioremap 주소는 unmap되지 않고 살아있으며, 이미 두번째 iounmap으로 해제하였기 때문에 segmentation fault 발생 -> driver를 열 때 mapping하도록 init, exit 단계로 옮김.
