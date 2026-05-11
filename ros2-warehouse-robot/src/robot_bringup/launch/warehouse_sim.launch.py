import os
from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    world = os.path.join(
        get_package_share_directory('robot_simulation'),
        'worlds', 'warehouse.world'
    )

    urdf = os.path.join(
        get_package_share_directory('turtlebot3_gazebo'),
        'models', 'turtlebot3_waffle_pi', 'model.sdf'
    )

    return LaunchDescription([

        # gzserver avec le monde entrepôt
        ExecuteProcess(
            cmd=['gzserver', '--verbose', world,
                 '-s', 'libgazebo_ros_init.so',
                 '-s', 'libgazebo_ros_factory.so'],
            output='screen'
        ),

        # gzclient délayé de 5 secondes (évite le crash de timing)
        TimerAction(
            period=5.0,
            actions=[
                ExecuteProcess(
                    cmd=['gzclient'],
                    output='screen'
                )
            ]
        ),

        # robot_state_publisher
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[{'use_sim_time': True}],
            arguments=[urdf]
        ),

        # Spawn du Waffle Pi délayé de 3 secondes
        TimerAction(
            period=3.0,
            actions=[
                ExecuteProcess(
                    cmd=['ros2', 'run', 'gazebo_ros', 'spawn_entity.py',
                         '-entity', 'waffle_pi',
                         '-file', urdf,
                         '-x', '0.0', '-y', '0.0', '-z', '0.01'],
                    output='screen'
                )
            ]
        ),
    ])