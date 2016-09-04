##本项目包含##
- tx_rx_serial.c : code that transmits and receives serial data. Use this as the basis of your own code.
- serial_toggle_DTR.c : toggles the DTR pin at about 2 Hz.

##tx_rx_serial.c##

###运行前准备###
1. 确保RS232端口已连接于电脑。如果是用VM，请确保VM中已勾选设备。
2. 确认RS232端口的2号针脚和3号针脚已连通（针脚位置参考[维基百科](https://zh.wikipedia.org/wiki/RS-232)）。
3. 输入以下命令：

```bash
$ dmesg | grep tty
# => [  691.641435] usb 1-2: pl2303 converter now attached to ttyUSB0
```

根据以上输出结果，修改源代码 [tx_rx_serial.c 约56行处](/tx_rx_serial.c#L56)，改为对应的设备值（本例中为 **ttyUSB0**）。

```c
#define SERIAL_PORT "/dev/ttyUSB0" // 该行原为 #define SERIAL_PORT "/dev/tty0"
```

最后参见[此处修改](https://github.com/ethanyoung/serial_port/pull/2/files)做一些其它的代码改动。

###编译###
```bash
$ gcc -o tx_rx_serial.out -g tx_rx_serial.c # 生成可执行文件 tx_rx_serial.out
```

###运行###
```bash
$ ./tx_rx_serial.out
# =>
#  Transmitted "1234567890" on serial port.
#  Received 10 bytes: 1234567890
#  DTR low.
```

`Received 10 bytes: 1234567890` - 该行表示成功接收到数据，运行成功。

###注意 ⚠️###
每次修改了`tx_rx_serial.c`，都需要重新[编译](#编译)然后运行。

##serial_toggle_DTR.c##
作用目前未知。运行后会一直循环，也不会改变`tx_rx_serial.c`的结果。
