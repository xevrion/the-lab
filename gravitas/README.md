# Gravitas

A 2D physics sandbox and solar system simulator built in C++ with raylib.

## Modules

**Gravitas** — bouncing balls with realistic physics
- Gravity, wall collision, restitution, friction
- Ball-to-ball elastic/inelastic collision with mass weighting
- Trail effects, speed cap, sleep threshold

**Solar System** — n-body gravity simulator
- Newton's law with softening factor (no singularity)
- Stable circular orbital initialization
- Planet merging with momentum conservation
- Elliptical orbit trails
- Live tunable G, epsilon, speed cap via custom sliders

## TODO
- Circular buffer for orbit trails (currently O(n) erase)
- Click to spawn planets with velocity
- Pause/resume

## Built with
- C++
- raylib
- raygui (removed, replaced with custom slider)
