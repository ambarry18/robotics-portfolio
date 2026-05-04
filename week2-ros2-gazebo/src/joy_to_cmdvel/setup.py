import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'joy_to_cmdvel'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'),
            glob('launch/*.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='amadou',
    maintainer_email='barryamadou0198@gmail.com',
    description='ROS 2 node converting Joy messages to cmd_vel for TurtleBot3',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'joy_to_cmdvel = joy_to_cmdvel.joy_to_cmdvel:main',
        ],
    },
)
