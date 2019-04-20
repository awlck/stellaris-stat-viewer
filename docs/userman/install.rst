======================================
Getting ahold of Stellaris Stat Viewer
======================================

If you are using a 64-bit version of **Windows** (like I assume most people do these days),
then you have it simple: just head over to the `GitHub releases page`_ and download the latest
version! You have a choice between a traditional installer `exe`, and a simple `zip` file.

.. note::
   The `zip` version contains the exact same files that would be installed by the `exe`
   installer: it is not truly "portable" software, settings will still be stored in the
   registry.

.. _GitHub releases page: https://github.com/ArdiMaster/stellaris-stat-viewer/releases

If you are using a 64-bit version of **Ubuntu**, then you *may* just be able to snatch a
current build from the `CI server`_. You will need to install **Qt 5.9** using `apt`.

On the site linked above, click the "Image: Ubuntu1804" entry, then switch to the
"Artifacts" page, and download the `zip` file listed there. Extract it somewhere safe, and
you should be good to go!

.. _CI server: https://ci.appveyor.com/project/ArdiMaster/stellaris-stat-viewer

On all **other** systems, I am afraid you're currently out of luck: you will have to build
from source. This is a bit outside the intended scope of this user's manual, but in short,
you will need a recent version of Qt5, CMake, and a C++11-capable compiler. The
:doc:`Technical Manual </techman/index>` should cover building from source in the future, but
until that is done, take a look at the (admittedly incomplete) `build instructions on GitHub
<https://github.com/ArdiMaster/stellaris-stat-viewer/wiki/Building-SSV-from-Source>`_.
