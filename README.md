# Crowd Modelling Tool
GUI Qt app for crowd dynamics modelling. Visualization and numeric metrics of people crowd moving in bounded space.

# Theory and math
Model is based on Newtonian mechanics and using pathfinding algorithms (A*, Lee). Mathematical model is based on following work:
> Modeling of behavior of panicked crowd in multi-floor branched space 
> A. M. Aptukov, D. A. Bratsun, A. V. Lyushnin<br>
> Perm Pedagogical University, Center for the Study of Complex Systems, 24 Sibirskaya st., Perm, 614990, Russia<br>
> Citation: Computer Research and Modeling, 2013, vol. 5, no. 3, pp. 491–508 (Russian).

Link to original paper: http://crm-en.ics.org.ru/uploads/crmissues/crm_2013_3/13313.pdf

# Libs
Project is using or used in previous versions following external libs :
* lee-pathfinder (https://github.com/dibadov/lee-pathfinder);
* astar-algorithm-cpp (https://github.com/justinhj/astar-algorithm-cpp);
* self-implemented A* algo `a_star_step.h` using following implementation guide (https://www.redblobgames.com/pathfinding/a-star/implementation.html);
* pia (https://github.com/smuelpeng/pia) - polygon intersection area algo.

All of them are included directly to project as source files. Some of them are slightly modified to fit our goals.

# Build and run
Project can be built both by native Qt tools (Qt Creator, qmake) and by cmake.
## Qmake
```
qmake crowd.pro
make
./crowd
```

## CMake
```
mkdir build && cd build
cmake ..
make
src/crowd
```

# Usage
Build and run app. Choose path to config file 'Settings->Path to config' (standard config is in project directory 'config/config.json'). Choose scheme file 'File->Open scheme' (2 files in 'schemes/'):
* scheme.json - just a casual room, actually very strange:);
* torine.json - scaled model of Piazza San Carlo in Turin with predefined params for modelling this accident (https://www.bbc.com/news/world-europe-40147813).

Press 'Start', watch, learn, analyze, contribute:)
