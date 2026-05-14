import os
from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    world = os.path.join(
        get_package_share_directory('turtlebot3_gazebo'),
        'worlds', 'turtlebot3_house.world'
    )

    sdf = os.path.join(
        get_package_share_directory('turtlebot3_gazebo'),
        'models', 'turtlebot3_waffle_pi', 'model.sdf'
    )

    urdf_path = os.path.join(
        get_package_share_directory('turtlebot3_description'),
        'urdf', 'turtlebot3_waffle_pi.urdf'
    )
 
    with open(urdf_path, 'r') as f:
        robot_desc = f.read().replace('${namespace}', '')

    return LaunchDescription([

        ExecuteProcess(
            cmd=['gzserver', '--verbose', world,
                 '-s', 'libgazebo_ros_init.so',
                 '-s', 'libgazebo_ros_factory.so'],
            output='screen'
        ),

        TimerAction(
            period=5.0,
            actions=[
                ExecuteProcess(cmd=['gzclient'], output='screen')
            ]
        ),

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{'robot_description': robot_desc, 'use_sim_time': True}]
        ),

        TimerAction(
            period=3.0,
            actions=[
                ExecuteProcess(
                    cmd=['ros2', 'run', 'gazebo_ros', 'spawn_entity.py',
                         '-entity', 'waffle_pi', '-file', sdf,
                         '-x', '0.0', '-y', '0.5', '-z', '0.01'],
                    output='screen'
                )
            ]
        ),
    ])