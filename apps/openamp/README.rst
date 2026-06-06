.. zephyr:code-sample:: openamp
   :name: OpenAMP
   :relevant-api: ipm_interface

Send messages between two cores using OpenAMP.

Overview
********

This application demonstrates how to use OpenAMP with Zephyr for
inter-core communication on multicore platforms. It shows how to
exchange messages between a primary core and a remote core using
RPMsg over OpenAMP.

This application is maintained in the project repository and is not
located under the Zephyr `samples` directory. Therefore, build
commands and board support may differ from the upstream Zephyr
OpenAMP sample.

Prerequisites
***********************************************

Before building the application, run the following command from the
`<zephyr_workspace>/modules` directory using Git Bash:

.. code-block:: bash

   sed -i 's/VERSION 3.0.2/VERSION 3.20.0/' hal/libmetal/libmetal/CMakeLists.txt lib/open-amp/open-amp/CMakeLists.txt

This updates the minimum CMake version required by the OpenAMP and
Libmetal modules.


Building the Application for maax_imxrt1176_cm7
***********************************************

From the application root directory, build the project using sysbuild:

.. code-block:: bash

   west build -b maax_imxrt1176/mimxrt1176/cm7 --sysbuild calixto-zephyr-app/apps/openamp


Running the Application
***********************************************

Open a serial terminal and connect to the board using the appropriate
UART settings for your platform.

After programming and resetting the board, the primary and remote cores
will exchange messages through OpenAMP.
Open a serial terminal (minicom, putty, etc.) and connect the board with the
following settings:

- Speed: 115200
- Data: 8 bits
- Parity: None
- Stop bits: 1

Reset the board and the following message will appear on the corresponding
serial port, one is master another is remote:

.. code-block:: console

   **** Booting Zephyr OS build zephyr-v3.7.2 ****
   Starting application thread!

   OpenAMP[master] demo started
   Master core received a message: 1
   Master core received a message: 3
   Master core received a message: 5
   ...
   Master core received a message: 99
   OpenAMP demo ended.


.. code-block:: console

   **** Booting Zephyr OS build zephyr-v3.7.2  ****
   Starting application thread!

   OpenAMP[remote] demo started
   Remote core received a message: 0
   Remote core received a message: 2
   Remote core received a message: 4
   ...
   Remote core received a message: 98
   OpenAMP demo ended.