# speed_module의 모든 노드를 실행
from launch import LaunchDescription
from launch_ros.actions import Node

# socketcan을 launch를 위해 가져옴
import os
from launch.actions import IncludeLaunchDescription
from launch_ros.substitutions import FindPackageShare

from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution


def generate_launch_description():

    # ros2_socketcan 패키지 경로 - 특수 객체
    socketcan_pkg_share = FindPackageShare('ros2_socketcan')

    # Receiver(.py) 런치 파일 - 경로 객체 생성
    socketcan_receiver_launch_path = PathJoinSubstitution(
        [
            socketcan_pkg_share,
            'launch',
            'socket_can_receiver.launch.py'
        ]
    )

    # Sender(.py) 런치 파일 - 경로 객체 생성
    socketcan_sender_launch_path = PathJoinSubstitution(
        [
            socketcan_pkg_share,
            'launch',
            'socket_can_sender.launch.py'
        ]
    )

    # Receiver 실행 (경로 객체 사용)
    socketcan_receiver_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(socketcan_receiver_launch_path),
        launch_arguments={'interface': 'can0'}.items()
    )

    # Sender 실행 (경로 객체 사용)
    socketcan_sender_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(socketcan_sender_launch_path),
        launch_arguments={'interface': 'can0'}.items()
    )


    return LaunchDescription([
        # socketcan 관련 노드 실행 (receiver, sender)
        socketcan_receiver_launch,
        socketcan_sender_launch,

        # ... (이하 speed_module_pkg 노드 4개 동일) ...
        Node(
            package='speed_module_pkg',
            executable='read_tof_node',
            name='read_tof_node',
            output='screen'
        ),
        Node(
            package='speed_module_pkg',
            executable='acc_node',
            name='acc_node',
            output='screen'
        ),
        Node(
            package='speed_module_pkg',
            executable='creep_node',
            name='creep_node',
            output='screen'
        ),
        Node(
            package='speed_module_pkg',
            executable='send_pwm_node',
            name='send_pwm_node',
            output='screen'
        ),
    ])