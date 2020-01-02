========================
Structure of the Program
========================

Stellaris Stat Viewer currently consists of three semi-separate components: *core*, the
*widgets frontend*, and the *json frontend*.

The Significance of Core
------------------------

*core*, as the name implies, does most of the heavy lifting: it contains the parser, as
well as the logic needed to create objects from the parse tree.

.. note::
   *core* only creates object representations of the save file contents, e.g. a vector of
   ``Ship`` objects. Turning that into sensible numbers is left as an exercise for the
   frontend.

The Concept of Frontends
------------------------

Early on in Stellaris Stat Viewer's development, a friend wanted to build a web application
for it, which necessitated exporting numbers in a machine-readable format without needing
to navigate the GUI. The web application never came to pass, but the ability to bypass the
graphical interface altogether and use SSV from the console lives on. Initially, frontends
were dynamic libraries exporting a special function, loaded at runtime based on the
``--frontend=`` command line setting. Ultimately, dynamic linking proved to be too much of
a pain to deal with, the entire thing never worked reliably (unless fully installed to the
standard locations), and the idea was scrapped without ever being used in a release.

Instead, frontends are now static libraries. Of course, that means that the choice of
frontends is fixed at compile time, but that's a concession I'm willing to make. It's not
like either of them is particularly huge anyways, so just enabling and distributing all of
them should be just fine in most cases. Based on the CMake settings, the file
``frontends.h.in`` is configured to reflect the frontends that will be built.

The chosen frontend takes full control over the execution of the program: the ``main``
function only figures out which frontend to use, then hands over to the relevant
``frontend_*_begin`` function.
