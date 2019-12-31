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
You are presented with two options for drawing the tree: `Standard` only draws
the "prerequisite" relation, whereas `Advanced` attempts to visualize how having
researched certain technologies affects the likelihood of other technologies
appearing as a choice.

Click the `Go` button, and wait a second or two for the entire thing to run.
You will then be prompted for where to save the resulting file. Once you do
that, the result will be opened in your default application for `.pdf` files.

.. note::
    The ability to change the output file format planned for a future
    version of Stellaris Stat Viewer.

Interpreting the output
-----------------------

Upon opening the resulting file, you will be presented with a somewhat intimidating mess
of colored boxes and arrows. But not to worry, it's not too hard to step through once you
know what you're looking at.

Except for some outliers (not sure how to fix those yet), a technology being shown towards
the right of the page *usually* appears later in the game, as it requires more other
technologies to be researched first.

The color of each box represents the research field the technology falls into: just like in
the game, blue border means physics, a green border means society, and an orange border
means engineering. A box filled in green means a starting technology (i.e. a technology
already researched by default when you begin a new game). A box filled in purple means that
the game considers this technology "rare". A box filled in grey means a technology with
default weight 0 (i.e. it will never appear as a research option unless something happens,
usually a special project).

An arrow with a black, solid line pointing from some technology *a* to another technology
*b* means that *b* will not appear as an option until *a* is researched (a
*is a prerequisite of* b). If several such arrows point at *b*, then *all* those
technologies must be researched first. An arrow with a dashed green line pointing from *a*
to *b* means that having researched *a* increases the chance of *b* appearing as an option.
Analogously, an arrow with a dashed red line would imply that having researched *a*
decreases the chances of *b* appearing, but as of game version 2.5, that never happens.

Note that weights are often affected by other factors, like leader traits, which are not
shown in the tree.
