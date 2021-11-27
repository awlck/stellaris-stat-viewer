==============================
Building Stellaris Stat Viewer
==============================

Stellaris Stat Viewer is written in C++11, using the Qt library, with CMake as its build system.
As such, all these components need to be present in order to build SSV yourself.

The following configurations are used by the developer and should thus work properly:

* Windows 10 x64, Visual Studio 2019
* macOS 11 x64, clang

It is assumed that the reader has a passing knowledge of how to compile a program, so the
general setup will be gone over quickly.
  
Windows
-------

Install Visual Studio, if you haven't already. Be sure to enable the relevant C++ desktop and
CMake features in the installer. Alternatively, MinGW should also be possible, but that has not
been tested. Next, install Qt (at least version 6.0.0 is required). Be sure to select the
appropriate (Visual Studio/ MinGW) variant for your development system.

To build, open the *Native Tools Command Prompt* (preferably x64, if your system supports it).
Naviagate to the folder in which you stored the SSV source code. It is generally recommended
to build in a subfolder::

    mkdir cmake-build-debug
    cd cmake-build-debug

Then, set CMake up generating::

    cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..

Finally, start the build::

    ninja

The ``ninja`` utility is included with Visual Studio's CMake option, and builds on all
available cores by default.

In order for the resulting executable to run, you will need to either add the directory
containing Qt's binaries to *PATH*, or manually copy the relevant `.dll` files from there to
your build directory. Before distributing your build, copy the executables to a blank
directory and run ``windeployqt`` to ensure all dependencies are present.

macOS
-----

If you haven't done so already, install XCode from the App Store. Next, install Qt (version 6.0
or higher required). Also install CMake (available via Homebrew or from
`here <https://cmake.org>`_).

To build, open a terminal and navigate to the folder into which you have downloaded the source
code. It is generally recommended to build in a subfolder::

  mkdir cmake-build-debug
  cd cmake-build-debug

Then, CMake is used to generate the Makefiles::

  cmake -DCMAKE_BUILD_TYPE=Debug ..

Finally, start the actual build using ``make`` on all cores::

  make -j

The build process automatically creates a `.app` bundle, but this will be tied to the Qt
libraries on your system: you will need to apply the ``macdeployqt`` tool if you want to
distribute the result.

Linux
-----

Again, you will need a C++11-compatible compiler (recommended: ``gcc``/ ``g++`` or ``clang``),
the Qt development files, and CMake. The exact process for installing these will depend on the
distribution you are using, but on Ubuntu, the process would look something like this::

  sudo apt install g++ cmake make qtbase6-dev

To build, open a terminal and navigate to the folder in which you have downloaded the source
code. It is generally recommended to build in a subfolder::

  mkdir cmake-build-debug
  cd cmake-build-debug

Then, use CMake to generate the Makefiles::

  cmake -DCMAKE_BUILD_TYPE=Debug ..

Finally, start the actual build using ``make`` on all cores::

  make -j

Build options
-------------

When generating build files, CMake allows setting special variables to be set that will affect
the build process. They must all be specified in the form ``-DVARIABLE=VALUE``

``CMAKE_BUILD_TYPE``
  Choose the type of build. Common values are ``Debug`` (few optimizations, good for
  development), ``Release`` (plenty of optimization, no debug info), and ``RelWithDebInfo``
  (reasonably many optimizations, with debug info). The official builds published on GitHub
  currently use the ``RelWithDebInfo`` setting.

``CMAKE_C_COMPILER``
  Explicitly set the compiler to be used for C code. Useful if you have several compilers
  installed and don't want to use the default one (e.g. ``/usr/bin/cc`` on Linux).

``CMAKE_CXX_COMPILER``
  Explicitly set the compiler to be used for C++ code (default on Linux: ``/usr/bin/c++``).
  Not   required to be the same as the chosen C compiler. It is however recommended to
  choose   compilers from the same family (e.g. ``gcc`` and ``g++`` or ``clang`` and
  ``clang++``) to prevent any ABI issues.
  
``SSV_BUILD_TESTS``
  Set ``ON`` to build unit tests for the parser, ``OFF`` to skip this.

``SSV_BUILD_WIDGETS``
  Set ``ON`` to include the QtWidgets GUI frontend in the resulting application. More on the
  concept of frontends later. Set ``OFF`` to build a console-only application.

``SSV_BUILD_JSON``
  Set ``ON`` to include a console-based `JSON` output. Enabling this also allows exporting
  `JSON` data from the graphical interface.

``SSV_BUILD_VERSION``
  Set the version displayed in the *About* dialog. Used for release builds.

Troubleshooting
---------------

If CMake complains about being unable to find Qt, you may need to specify its path manually::

  -DCMAKE_PREFIX_PATH="D:/path/to/Qt/6.2.1/msvc2019_64"
