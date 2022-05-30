#!/usr/bin/env python3

"""
Dump the output of tennicam_client into a file.
The first line is reserved for meta-data, which is by default empty.
Then, format for each line:
 repr([ball_id,time_stamp,position,velocity])
with:
- ball_id : int
- time_stamp: int (nanoseconds)
- position: 3d tuple
- velocity: 3d tuple
If a robot segment-id is provided, then the meta-data
is a string repr(robot_position,robot_orientation), with
robot position a tuple of four floats and orientation a string
of 4 values (e.g. "0 0 0 0", i.e. mujoco format for orientation).
Then, for each line:
 repr([ball_id,time_stamp,position,velocity,robot_time_stamp, joint_positions, joint_velocities])
with:
- robot_time_stamp: int (nanoseconds)
- joint_positions: tuple of 4 values, in radians
- joint_velocities: tuple of 4 values, in radians per second
"""


import sys
import typing
import pathlib
import signal_handler
import tennicam_client
import o80_pam
from lightargs import BrightArgs


TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"
TENNICAM_CLIENT_DEFAULT_FOLDER = pathlib.Path("/tmp")


def _unique_path(
    directory: pathlib.Path, name_pattern="tennicam_{:03d}"
) -> pathlib.Path:
    """
    returns the path to a file /directory/tennicam_{x} that does not exists yet
    """

    counter = 0
    while True:
        counter += 1
        path = directory / name_pattern.format(counter)
        if not path.exists():
            return path


def _run(
    segment_id: str, robot_segment_id: typing.Optional[str], filepath: pathlib.Path
):
    """
    Creates an o80 frontend and uses it to get all
    ball observations and dumping a corresponding
    string in filepath.
    If robot_segment_id is not None: the current
    robot's joints position and velocities are added
    to the observations.
    """

    frontend = tennicam_client.FrontEnd(segment_id)
    iteration = frontend.latest().get_iteration()

    robot_frontend: typing.Optional[o80_pam.Frontend]
    robot_getters: typing.Optional[typing.Tuple[str,...]]
    meta_data: str
    
    if robot_segment_id:
        robot_frontend = o80_pam.Frontend(robot_segment_id)
        robot_getters = ("get_positions", "get_velocities")
        meta_data = repr(pam_mujoco.MujocoRobot.read_positon(robot_segment_id))
    else:
        robot_frontend = None
        robot_getters = None
        meta_data = ""
        
    getters = ("get_ball_id", "get_time_stamp", "get_position", "get_velocity")

    with filepath.open(mode="w") as f:

        f.write(meta_data+"\n")

        try:
            while not signal_handler.has_received_sigint():
                iteration += 1
                obs = frontend.read(iteration)
                if not robot_frontend or not robot_getters:
                    values = tuple((getattr(obs, getter)() for getter in getters))
                else:
                    robot_obs = robot_frontend.latest()
                    values = tuple(
                        [getattr(obs, getter)() for getter in getters]
                        + [getattr(robot_obs, getter)() for getter in robot_getters]
                    )
                f.write(repr(values))
                f.write("\n")
        except (KeyboardInterrupt, SystemExit):
            pass
        except Exception as e:
            print("Error:", e)


def _configure() -> BrightArgs:
    """
    Configuration dialog
    """

    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID
    global TENNICAM_CLIENT_DEFAULT_FOLDER
    config = BrightArgs("tennicam logger")
    config.add_option(
        "segment_id",
        TENNICAM_CLIENT_DEFAULT_SEGMENT_ID,
        "segment_id of the o80 backend",
        str,
    )
    config.add_option(
        "robot_segment_id",
        None,
        "segment_id of the o80 backend of the robot (optional)",
        str,
    )
    config.add_option(
        "filepath",
        str(_unique_path(TENNICAM_CLIENT_DEFAULT_FOLDER)),
        "absolute path of the log file",
        str,
    )
    change_all = False
    config.dialog(change_all, sys.argv[1:])
    print()
    return config


if __name__ == "__main__":
    config = _configure()
    segment_id = config.segment_id
    robot_segment_id = config.robot_segment_id
    filepath = pathlib.Path(config.filepath)
    _run(segment_id, robot_segment_id, filepath)
