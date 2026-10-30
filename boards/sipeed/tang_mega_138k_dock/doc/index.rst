.. zephyr:board:: tang_mega_138k_dock

Overview
********

The `Tang Mega 138K Dock`_ is an FPGA development board from Sipeed built around the Tang Mega
138K system-on-module, a Gowin GW5AST-LV138PG484 device. The FPGA integrates a 32-bit RISC-V
`AndesTech AE350`_ hard-core SoC, which is what Zephyr runs on. The FPGA fabric around it is
described by a bitstream that must be programmed before Zephyr can be used.

Hardware
********

The Tang Mega 138K Dock provides the following hardware features:

- Andes AE350 32-bit RISC-V processor (Andes A25 core, up to 800 MHz)
- 64 KiB ILM and 64 KiB DLM local memories, 32 KiB instruction and data caches
- 1 GB DDR3 memory
- 64 Mbit (early boards) or 128 Mbit (boards shipped from October 2025) SPI flash, shared by the
  FPGA bitstream and the firmware
- PCIe x1 edge connector
- SFP+ cage (early boards) or USB 3.0 port (boards shipped from October 2025)
- Gigabit Ethernet
- HDMI output and input
- MIPI CSI connector
- MicroSD card slot
- 2 x PMOD connectors
- 40-pin expansion header
- USB JTAG and UART debugger
- User LEDs and buttons

Supported Features
==================

.. zephyr:board-supported-hw::

The baseline bitstream configures the RiscV_AE350_SOC IP as follows:

- SPI flash instruction memory
- 256 KiB DTCM (block RAM) data memory, no DDR3
- UART2 as the console, wired to the onboard USB debugger
- Core clock 800 MHz, AHB and APB clocks 50 MHz

Zephyr uses the RISC-V machine timer as system clock and UART2 as console. The other hard
peripherals of the AE350 (GPIO, PIT, watchdog, RTC, I2C, SPI, DMA) are described in the devicetree
and can be enabled once the bitstream routes them to pins.

Programming and debugging
*************************

.. zephyr:board-supported-runners::

FPGA provisioning
=================

Before using the board, write the FPGA bitstream to offset zero of the SPI flash and set the
boot-mode DIP switches to boot from flash. The ``DBG_EN`` switch must be on for the onboard
debugger to be detected.

.. code-block:: console

   openFPGALoader --board tangmega138k --external-flash --write-flash --offset 0x0 ae350_baseline.fs

The bitstream instantiates the AE350 hard core and defines which peripherals and memories are
available. Repeat this step after the FPGA image has been erased or replaced.

Building and flashing Zephyr
============================

Zephyr runs in place from the SPI flash. ``west flash`` writes ``zephyr.bin`` at flash offset
``0x600000``, which the AE350 maps at ``0x80000000``, using the onboard USB debugger; it does not
touch the bitstream. The FPGA reloads after programming, which resets the processor into the new
firmware.

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: tang_mega_138k_dock/ae350
   :goals: build flash

The console is available on the UART of the onboard USB debugger at 115200 baud.

Debugging
=========

The AE350 debug port is a separate 4-wire JTAG interface that the bitstream routes to FPGA pins; it
is not reachable through the onboard FPGA debugger. Connect an OpenOCD-compatible JTAG adapter to
those pins. The board configuration assumes an FTDI FT232H based adapter; pass another interface
configuration with ``west debug --config``.

The default configuration uses XIP, so flash the application first and attach without loading it
again:

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: tang_mega_138k_dock/ae350
   :maybe-skip-config:
   :goals: attach

For early-boot debugging, build with ``CONFIG_XIP=n``. Non-XIP images are linked into the data
memory and ``west debug`` loads the ELF directly into RAM:

.. zephyr-app-commands::
   :zephyr-app: samples/hello_world
   :board: tang_mega_138k_dock/ae350
   :maybe-skip-config:
   :goals: build debug

References
**********

.. target-notes::

.. _Tang Mega 138K Dock: https://wiki.sipeed.com/hardware/en/tang/tang-mega-138k/mega-138k.html

.. _AndesTech AE350: http://www.andestech.com/en/products-solutions/andeshape-platforms/ae350-axi-based-platform-pre-integrated-with-n25f-nx25f-a25-ax25/
