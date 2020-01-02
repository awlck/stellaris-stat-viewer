========
Settings
========

Stellaris Stat Viewer currently knows three settings, accessible via *Tools* >
*Settings* on most systems, and via *Stellaris Stat Viewer* > *Preferences* on macOS.

Game Folder
  This field allows you to input the folder in which the game itself is installed. You
  can either type the path in yourself, or you can click the *Select* button and browse
  for it. Setting this enables you to use the :doc:`tech tree feature <techtree>`, and
  also allows displaying proper names in the Technologies view.
  
  On Windows, with a default Steam install, the path would be ``C:/Program Files (x86)/Steam/steamapps/common/Stellaris``.

Dot
  This field similarly allows you to input the location of the *dot* utility, also used
  in drawing tech trees. Dot is part of `GraphViz <https://www.graphviz.org>`_.

Game Language
  This list will be populated with interesting options once you set the game folder.
  The language chosen here will be used to display the names of technologies both in
  the tech tree output and the Technologies view. Choosing *(None)* causes SSV to display
  the internal identifiers used within the save files and the game's code.
