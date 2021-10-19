#!/usr/bin/env python3

"""
Display in a mujoco simulated environment the ball
as published by tennicam.
Required for this executable:
- in a first terminal: 'tennicam_client'
- in second terminal :'pam_mujoco tennicam_client_display'
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
    graphics = True
    accelerated_time = False
    handle = pam_mujoco.MujocoHandle(
        "tennicam_client_display",
        table=True,
        balls=(ball,),
        graphics=graphics,
        accelerated_time=accelerated_time,
    )
    return handle


def run():

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
            # getting information from tennicam
            ball_zmq = frontend.latest()
            position = ball_zmq.get_position()
            velocity = ball_zmq.get_velocity()
            # sending related command to mujoco ball
            ball.add_command(position, velocity, duration_ms, o80.Mode.OVERWRITE)
            ball.pulse()
            # imposing frequency
            frequency_manager.wait()
    except (KeyboardInterrupt, SystemExit):
        return


if __name__ == "__main__":
    run()
