===================
Drawing a Tech Tree
===================

Stellaris Stat Viewer currently has one "extra" feature: generating a *Tech
Tree*, which attempts to visualize the somewhat complex relations between the
various technologies in the game. In short, it reads in the game's technology
files, and then creates a graph where the relation "technology *b* requires
technology *a* to be researched first" is visualized by an arrow pointing
from *a* to *b*.

Setup
-----

As mentioned above, the game's files are partially read in order to make this
entire stunt work. So, obviously, you will need to have access to the game
files for this to work. For generating the graph, Stellaris Stat Viewer relies
on the `dot` utility from the GraphViz suite, so you'll need that as well.
Set the paths for both of these in the settings dialog before proceeding.

Drawing the Tree
----------------

Access the tech tree drawing utility by clicking `Tools` > `Draw Tech Tree...`.
You are presented with two options for drawing the tree: `Reduced` only draws
the "prerequisite" relation, whereas `Full` attempts to visualize how having
researched certain technologies affects the likelihood of other technologies
appearing as a choice.

.. note::
    You will notice the `Full` option is greyed out. This is because I haven't
    gotten around to implementing this yet, I'm merely getting you excited.

Click the `Go` button, and wait a second or two for the entire thing to run.
You will then be prompted for where to save the resulting file. Once you do
that, the result will be opened in your default application for `.svg` vector
graphics files (usually a web browser).

.. note::
    The ability to change the output file format planned for a future
    version of Stellaris Stat Viewer.

