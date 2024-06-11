.. _posix-custom-option-group-sample:

Custom POSIX Option Group Implementation
########################################

Overview
********

This sample application demonstrates how one may override Zephyr's implementation of a
POSIX Option Group. The mechanism through which this is achieved is via selection of Kconfig
options in :file:`lib/posix/options/Kconfig.toolchain`.

Zephyr's POSIX API allows for third-parties to supply their own implementations of POSIX features
at Option Group granularity. All symbols within a particular Option Group should be implemented in
order to ensure that the application can link and that things run as expected. For example, if a
third-party C library wishes to use its own implementation of the
:ref:`POSIX_FILE_SYSTEM <posix_option_group_file_system>` Option Group, then one would
``select TC_PROVIDES_POSIX_FILE_SYSTEM`` in a Kconfig file. Then, the custom implementation of
*all* symbols within the ``POSIX_FILE_SYSTEM`` Option Group should be provided.

.. note::

      The ``CONFIG_TC_PROVIDES_POSIX_FILE_SYSTEM`` and similar Kconfig options are not user
      configurable and cannot be added to an application via ``prj.conf`` or ``overlay.conf``.
      These symbols must be selected by another Kconfig option, typically added by a maintainer
      to leverage the POSIX API of an external C library.

For the purposes of simplicity, the :ref:`POSIX_PIPE <posix_option_group_pipe>` Option Group is
overridden in this sample, which means only a single function needs to be implemented - ``pipe()``.

The ``pipe()`` function creates an unnamed pipe - a unidirectional data channel that can be used
for interprocess (or inter-thread) communication.

Building and Running
********************

Build and run this sample with Zephyr's ``pipe()`` implementation.

.. zephyr-app-commands::
   :zephyr-app: samples/posix/custom_option_group
   :board: qemu_riscv64
   :goals: run
   :compact:

Observe the following message on the console:

.. code-block:: console

    write: 42
    read: 42

Build and run this sample with a custom ``pipe()`` implementation.

.. zephyr-app-commands::
   :zephyr-app: samples/posix/custom_option_group
   :board: qemu_riscv64
   :gen-args: -DOVERLAY_CONFIG="custom.conf"
   :goals: run
   :compact:

Observe the following message on the console, which shows that the custom implementation of
``pipe()`` was used:

.. code-block:: console

    write: 42
    this pipe is custom
    read: 42
