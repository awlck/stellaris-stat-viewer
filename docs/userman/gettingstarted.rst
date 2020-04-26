================
Before you begin
================

Stellaris Stat Viewer reveals information about other empires that would not
normally be available to you during play, and its use may be considered
cheating. The author(s) of Stellaris Stat Viweer shall not be held liable for
any friendships ruined as a result of using the application.

Also, while the SSV is merely a viewer, not an editor, it *should* be safe to
use on your save files, but you may want to keep a backup just in case. Again,
the author(s) shall not be held liable for any harm caused by the use of the
application. See the license file for details about (the absence of) warranties.

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

If you are using a 64-bit version of **macOS** 10.12 "Sierra" or newer, we finally got
you covered: starting with version 0.3.1, macOS builds are also provided at the
`GitHub releases page`_. Note that macOS's *Gatekeeper* feature will likely stop you from
running Stellaris Stat Viewer the first time with every new release. To get around this,
it should be sufficient to right-click the `.app`, then selecting "Open" from the menu.

If you are using a 64-bit version of **Ubuntu**, then you *may* just be able to snatch a
current build from the `CI server`_. You will need to install **Qt 5.9** using `apt`.

On the site linked above, click the "Image: Ubuntu1804" entry, then switch to the
"Artifacts" page, and download the `zip` file listed there. Extract it somewhere safe, and
you should be good to go!

.. _CI server: https://ci.appveyor.com/project/ArdiMaster/stellaris-stat-viewer

On all **other systems**, I am afraid you're currently out of luck: you will have to build
from source. This is a bit outside the intended scope of this user's manual, but in short,
you will need a recent version of Qt5, CMake, and a C++11-capable compiler. The
:doc:`Technical Manual </techman/index>` should cover building from source in the future, but
until that is done, take a look at the (admittedly incomplete) `build instructions on GitHub
<https://gitlab.com/ArdiMaster/stellaris-stat-viewer/wikis/Building%20SSV%20from%20source>`_.

========
Overview
========

Stellaris Stat Viewer primarily consists of four tabs showing various statistics for your
empire: the **Overview** simply lists the Military, Economy, and Technology power ratings
for each empire. These are the numbers used by the game to produce the *Equivalent*,
*Inferior*, etc. ratings on the Contacts screen. The **Fleets** tab shows how many ships of
each type an empire posesses, together with the empire's total fleet power.

.. note::
   In the **Fleets** tab, check *Include stations in fleet power* to include starbases in
   the total fleet power figure. Leave it unchecked to include only mobile fleets.

The **Economy** tab lists each empire's  last month's incomes in
the various basic resources. Finally, the **Technologies** tab allows you to view the
technologies researched by each empire.

On top of that, the application menu provides options to load a save file, change some
settings, and to draw the game's tech tree. More on that later.

Loading a save file
-------------------

To load a save file, simply choose *Open Save File* from the *File* menu. Then select the
save file you want to inspect. Check `the Stellaris wiki`_ to find out where save files
are stored on your system.

As of version 0.3, SSV understands `.sav` compressed save files.

.. _the Stellaris wiki: https://stellaris.paradoxwikis.com/Save-game_editing
