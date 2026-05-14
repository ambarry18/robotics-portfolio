import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    map_file = os.path.join(
        os.path.expanduser('~'),
        'robotics-portfolio', 'ros2-service-robot',
        'results', 'maps', 'house_map.yaml'
    )

    nav2_params = os.path.join(
        get_package_share_directory('robot_navigation'),
        'config', 'nav2_params.yaml'
    )

    return LaunchDescription([

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(
                    get_package_share_directory('turtlebot3_navigation2'),
                    'launch', 'navigation2.launch.py'
                )
            ),
            launch_arguments={
                'use_sim_time': 'True',
                'map': map_file,
                'params_file': nav2_params
            }.items()
        ),
    ])