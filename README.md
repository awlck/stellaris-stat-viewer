# Stellaris Stat Viewer
... for all your galactic stat needs.

[![gitlab-ci](https://gitlab.com/ArdiMaster/stellaris-stat-viewer/badges/master/pipeline.svg)](https://gitlab.com/ArdiMaster/stellaris-stat-viewer/pipelines)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/ArdiMaster/stellaris-stat-viewer)](https://ci.appveyor.com/project/ArdiMaster/stellaris-stat-viewer)

(work in progress)

Have you ever played Stellaris and just wondered where the heck you stand compared to the other empires?
Well, some friends of mine certainly have, and since Stellaris doesn't provide EU4-style ingame statistics,
I came up with this instead. Stellaris Stat Viewer reads in a Stellaris savegame file and gives you copious
statistics and rankings.

**Note**: Stellaris Stat Viewer has recently [moved to GitLab](https://gitlab.com/ArdiMaster/stellaris-stat-viewer).
The GitHub page is now merely a mirror of that repository, but releases will continue to be published on GitHub.

## Features
The user interface looks a lot like a spreadsheet, because that's what it is. Stellaris Stat Viewer reads in
a save file and displays each empire's power ratings, ships per class, and monthly incomes. For a more
complete listing, see the [user manual](https://stellaris-stat-viewer.readthedocs.io/en/latest/userman/index.html).

If you have ideas for some stat/ranking you'd like to see, [open an Issue](https://gitlab.com/ArdiMaster/stellaris-stat-viewer/issues) and I'll see what I can do.

## Building
Stellaris Stat Viewer (or SSV, for short) is built using Qt5 and C++11. It should be possible to compile it
wherever Qt5 and CMake are available (that includes Windows, Mac, and Linux). For instructions, see
[the manual](https://stellaris-stat-viewer.readthedocs.io/en/latest/techman/building.html).

**Dependencies**: SSV uses Qt5 (tested with 5.9.5, 5.12, 5.13, 5.14), and CMake (version 3.0 or newer).