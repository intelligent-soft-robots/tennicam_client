#!/usr/bin/env python3

"""
Display in a mujoco simulated environment the ball
as published by tennicam.
Required for this executable:
- in a first terminal: 'tennicam_client'
- in second terminal :'pam_mujoco tennicam_client_display'
Optionally, if you also wish the real robot to be mirrored:
set the real robot segment_id using the dialog.
"""

import o80
import signal_handler
import tennicam_client
import pam_mujoco


# required for this executable:
# in a first terminal: 'tennicam_client'
# in second terminal :'pam_mujoco tennicam_client_display'

TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"


def _get_handle():

    ball = pam_mujoco.MujocoItem(
        "ball", control=pam_mujoco.MujocoItem.CONSTANT_CONTROL, color=(1, 0, 0, 1)
    )
    table = pam_mujoco.MujocoTable("table")
    graphics = True
    accelerated_time = False
    handle = pam_mujoco.MujocoHandle(
        "tennicam_client_display",
        table=table,
        balls=(ball,),
        graphics=graphics,
        accelerated_time=accelerated_time,
    )
    return handle


def mirror_ball(frontend_from, frontend_to,duration_ms):
    # getting information from tennicam
    ball_zmq = frontend_from.latest()
    position = ball_zmq.get_position()
    velocity = ball_zmq.get_velocity()
    # sending related command to mujoco ball
    frontend_to.add_command(position, velocity, duration_ms, o80.Mode.OVERWRITE)
    frontend_to.pulse()
    

def run(
        tennicam_client_segment_id: str,
        real_robot_segment_id: typing.Optional[str],
        robot_type: typing.Optional[pam_mujoco.RobotType],
        robot_position: typing.Optional[typing.Tuple[float,float,float]],
        robot_orientation: typing.Optional[str]
):

    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID

    # configuring and staring mujoco
    handle = _get_handle()

    # getting a frontend for ball control
    ball = handle.frontends["ball"]

    # getting a frontend to the visually tracked ball
    frontend = tennicam_client.FrontEnd(TENNICAM_CLIENT_DEFAULT_SEGMENT_ID)

    # running at 100Hz
    frequency = 100.0
    frequency_manager = o80.FrequencyManager(frequency)
    duration_ms = o80.Duration_us.microseconds(int((1.0 / frequency) * 1e6))

    signal_handler.init()  # for detecting ctrl+c
    try:
        while not signal_handler.has_received_sigint():
            mirror_ball(frontend,ball,duration_ms)
            # imposing frequency
            frequency_manager.wait()
    except (KeyboardInterrupt, SystemExit):
        return


def _configure() -> BrightArgs:
    """
    Configuration dialog
    """

    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID
    config = BrightArgs("tennicam display")
    config.add_option(
        "segment_id",
        TENNICAM_CLIENT_DEFAULT_SEGMENT_ID,
        "segment_id of the o80 backend",
        str,
    )
    config.add_option(
        "robot_segment_id",
        None,
        "segment_id of the real robot (optional)",
        str,
    )
    config.add_option(
        "robot_type",
        str(pam_mujoco.RobotType.PAMY2),
        "type of the robot, pamy1 ({}) or pamy2 ({}) (used only if robot_segment_id not None)".format(
            str(pam_mujoco.RobotType.PAMY1), str(pam_mujoco.RobotType.PAMY2)
        ),
        str,
    )
    config.add_option(
        "robot_position",
        "[0.,0.,1.21]",
        "3d position of the robot (as string) (used only if robot_segment_id not None)"
        str,
    )
    config.add_option(
        "robot_orientation",
        "0.0 0.0 0.0",
        "orientation of the robot (as string) (used only if robot_segment_id not None)"
        str,
    )
    change_all = False
    config.dialog(change_all, sys.argv[1:])
    print()
    return config

def run():
    config = _configure()


    
if __name__ == "__main__":
    run()
