.. zephyr:code-sample:: spi_flash_programmer
    :name: SPI Flash Programmer
    :relevant-api: flash_interface

    Characterize and program generic SPI NOR flash devices.

Overview
********

This sample demonstrates how to use SPI, Flash, and JESD216 APIs in Zephyr to manage SPI NOR flash
devices with a cache of images ready. Only single-mode SPI is supported at this time.

Building and Running
********************

.. note::
    This sample is best to run with hardware that supports "hats", "shields", "clicks", and
    other swappable component boards. Alternatively, hook-up wire works fine as well.

.. zephyr-app-commands::
    :zephyr-app: samples/spi_flash_programmer
    :board: nucleo_g071rb
    :goals: build flash
    :compact:

Sample Output
=============

.. code-block:: console
    
    SPI Flash Programmer
    Type 'help' for a list of commands
    sfp> reset
    SPI Flash Programmer
    Type 'help' for a list of commands
    sfp> help
    ...
    sfp> cache ls
    sfp> get vio
    1.8
    sfp> get freq
    0
    sfp> identify
    Found [ef, 40, 15], 16 Mbit, vio: 3.3 V, cpol:1, cpha:1, cspol:1, sck: 32 MHz
    sfp> get freq
    31999999
    sfp> cache add --as image42 --from uart0:1152008n1:zmodem --progress bar
    |█████████████████---------------------------------| 34.3%  read 18.7 kiB in 158.0 ms (921.6 kbps)
    ...
    |██████████████████████████████████████████████████| 100.0% read 53.1 kiB in 460.9 ms (921.6 kbps)
    sfp> cache ls
    Name        Size        Hits
    ----------------------------
    image42     53.1 kiB    0
    sfp> reset
    sfp> burn --image image42
    |█████████████████████████████████████████████-----| 90.1%  write 31.99 Mbps (3.99 MiB/s)
    ...
    |██████████████████████████████████████████████████| 100.0% wrote 424.8 kbit (53.1 kiB) in 13.2 ms
    sfp> get vio
    3.3
    sfp> get freq
    31999999
    sfp> set cache image42
    sfp> burn
    sfp> burn
    sfp> burn
    sfp> cache ls --short-hash
    Name        Size        Hits  Sha256
    -----------------------------------------
    image42     53.1 kiB    4     27270fa14d0
    sfp> cache rm image42
    sfp> cache ls
    sfp>

Configuration
*************

Firmware can be built to support numerous options, for example:

* Support for 1.8V I/O
* Support for 3.3V I/O
* I/O Voltage Switching (either manual or software-controlled)
* Minimum SPI clock frequency supported by the SPI transceiver
* Maximum SPI clock frequency supported by the SPI transceiver
* Shell support
* Auto-detection of SPI NOR flash devices

Several transports are supported for control and transfer

* Bluetooth Serial (Xmodem, ZModem)
* Bluetooth IPSP (HTTP(s), Websockets)
* Ethernet (HTTP(s), Websockets)
* UART Serial (Xmodem, Zmodem)
* UART PPP (HTTP(s), Websockets)
* USB Serial (Xmodem, Zmodem)
* USB CDC-ACM (HTTP(s), Websockets)
* WiFi (HTTP(s), Websockets)

Usage
*****

Shell Commands
==============

The shell command-set is listed below

.. csv-table:: SPI Flash Programmer Shell Commands
    :header: Command, Help
    :widths: 25 75
    :header-rows: 1

    burn, Program the SPI NOR flash device.
    cache, Manage cached images (see ``cache --help`` for details).
    get, Get a parameter (e.g. ``get vio``)
    help, Display the list of commands.
    identify, Automatically identify a SPI NOR flash device and set I/O accordingly
    reset, Reset the board. Default settings and state are applied. I/O lines are in HiZ.
    set, Set a parameter (e.g. ``set vio 1.8``)

.. csv-table:: SPI Flash Programmer Parameters
    :header: Setting, Description
    :widths: 25 75
    :header-rows: 1

    cache, Get or set the cached image name. This sets the default for the 'burn' command.
    cpha, Get or set the current SPI clock phase; 0 := sample data on the first clock edge, 1 := sample data on the second clock edge.
    cpol, Get or set the SPI clock idle state (polarity); 0 := logic low. 1 := logic high.
    cspol, Get or set the SPI chip-select idle state (polarity); 0 := logic low. 1 := logic high.
    freq, Get or set the current SPI clock frequency.
    max-freq, Get or set the maximum SPI clock frequency supported by the device.
    min-freq, Get or set the minimum SPI clock frequency supported by the device.
    sfdp, Get the JESD216 serial flash discoverable parameters (Get only)
    vio, Get or set the I/O voltage.
