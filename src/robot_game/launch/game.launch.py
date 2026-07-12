import os
import yaml
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_share   = get_package_share_directory('robot_game')
    rviz_config = os.path.join(pkg_share, 'rviz', 'rviz_config.rviz')
    config_file = os.path.join(pkg_share, 'config', 'game_config.yaml')


    with open(config_file, 'r') as f:
        cfg = yaml.safe_load(f)
    game_mode = cfg.get('game_mode', 'duel')
    print(f'[LAUNCH] Herny mod: {game_mode}')

    nodes = [
        Node(
            package='robot_game',
            executable='robot_node',
            name='robot_node_0',
            arguments=['0'],
            output='screen'
        ),
        Node(
            package='robot_game',
            executable='robot_node',
            name='robot_node_1',
            arguments=['1'],
            output='screen'
        ),
        Node(
            package='robot_game',
            executable='game_node',
            name='game_node',
            output='screen'
        ),
        Node(
            package='robot_game',
            executable='lidar_node',
            name='lidar_node_0',
            arguments=['0'],
            output='screen'
        ),
        Node(
            package='robot_game',
            executable='lidar_node',
            name='lidar_node_1',
            arguments=['1'],
            output='screen'
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config],
            output='screen'
        ),
        Node(
            package='robot_game',
            executable='keyboard_node',
            name='keyboard_node',
            output='screen'
        ),
    ]


    if game_mode == 'duel_bot':
        nodes.append(Node(
            package='robot_game',
            executable='bot_node',
            name='bot_node',
            output='screen'
        ))
        print('[LAUNCH] Bot node spusteny')

    return LaunchDescription(nodes)
