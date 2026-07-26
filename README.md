# RobotGame

A waste-collection robot simulation built as a university assignment, implemented in two versions:

- **`robot_old`** — the original, plain C++ implementation (no ROS dependency).
- **`robot_game`** + **`robot_msgs`** — the redesigned version, built on ROS 2 with multiple communicating nodes, RViz visualization, and a YAML-configurable game setup.

Two robots compete to collect waste items (paper, plastic, glass) scattered across a map and drop them off at a central station, either against another human player or a simple autonomous bot.

## Repository structure

```
RobotGame/
├── src/
│   ├── robot_game/      # ROS 2 package: game logic, robots, sensors, launch/config
│   ├── robot_msgs/      # ROS 2 package: custom RobotState message definition
│   └── robot_old/       # Plain C++ version of the assignment (no ROS)
└── README.md
```

## `robot_game` — ROS 2 version

Built as a set of independent ROS 2 nodes communicating over topics:

| Node | Responsibility |
|---|---|
| `game_node` | Tracks waste and station state, checks pickup/drop-off, publishes RViz markers |
| `robot_node` | Simulates a single robot's physics/state, collision checking, publishes `RobotState` and an occupancy map |
| `lidar_node` | Simulates a 180-ray laser scanner per robot for obstacle sensing |
| `bot_node` | Simple autonomous controller for PvE (`duel_bot`) mode — seeks nearest waste, returns to station |
| `keyboard_node` | Reads keyboard input (via X11) to drive robots manually in PvP (`duel`) mode |

Custom message: `robot_msgs/msg/RobotState` — position, heading, velocities, and current/max load.

Waste items (`Paper`, `Plastic`, `Glass`) are created through a factory pattern (`WasteFactory`) and placed on the map by `WasteGenerator`, avoiding obstacles.

### Dependencies

- ROS 2 (Humble or later recommended)
- `yaml-cpp` (for config parsing)
- `libx11-dev` (for `keyboard_node`, which reads keyboard state via Xlib)

### Build

```bash
cd RobotGame
colcon build --symlink-install
source install/setup.bash
```

### Run

```bash
ros2 launch robot_game game.launch.py
```

This starts the full simulation (game logic, robots, sensors) and opens RViz using the bundled `rviz/rviz_config.rviz`.

### Configuration

Game parameters (mode, map, robot start positions, waste count/types, station location) live in `src/robot_game/config/game_config.yaml`.

> **⚠️ Known issue:** `game_config.yaml` currently hardcodes an absolute path to the map image. This will not resolve on another machine or after cloning the repo — update it to a path relative to the package before running on a new setup.

