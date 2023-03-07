#!/usr/bin/env python3

"""Display in a MuJoCo simulated environment the ball as published by tennicam.

Required for this executable:
- in a first terminal: 'tennicam_client'
- in second terminal: 'pam_mujoco tennicam_client_display'
- [if Vicon is used] in a third terminal: 'vicon_o80_standalone <IP>'
"""
from __future__ import annotations

import argparse
import logging
import typing as t

import o80
import signal_handler
import tennicam_client
import pam_mujoco

if t.TYPE_CHECKING:
    from vicon_transformer.transform import Transformation


# required for this executable:
# in a first terminal: 'tennicam_client'
# in second terminal :'pam_mujoco tennicam_client_display'

TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"


def _get_vicon_table_pose(vicon_segment_id: str) -> Transformation:
    # only import if actually needed
    import pam_vicon_o80

    logging.info("Get table pose from Vicon (using segment id '%s')", vicon_segment_id)
    vicon = pam_vicon_o80.PamVicon(vicon_segment_id)
    vicon.update()
    return vicon.get_table_pose(yaw_only=True)


def _get_handle(vicon_segment_id: t.Optional[str] = None) -> pam_mujoco.MujocoHandle:
    ball = pam_mujoco.MujocoItem(
        "ball", control=pam_mujoco.MujocoItem.CONSTANT_CONTROL, color=(1, 0, 0, 1)
    )
    table = pam_mujoco.MujocoTable("table")

    if vicon_segment_id:
        table_pose = _get_vicon_table_pose(vicon_segment_id)
        table.position = table_pose.translation.tolist()
        table.orientation = table_pose.rotation

    graphics = True
    accelerated_time = False
    handle = pam_mujoco.MujocoHandle(
        "tennicam_client_display",
        table=table,
        balls=[ball],
        graphics=graphics,
        accelerated_time=accelerated_time,
    )
    return handle


def run() -> None:
    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID

    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "--vicon",
        type=str,
        dest="vicon_segment_id",
        metavar="<segment id>",
        nargs="?",
        const="vicon",  # default if '--vicon' is set without a value
        help="""Get table pose from Vicon.  This requires an PAM o80 Vicon backend
            running with the specified segment ID (default: "%(const)s").
        """,
    )
    args = parser.parse_args()

    logging.basicConfig(
        format="[hysr_visualization.%(name)s] [%(levelname)s] %(message)s",
        level=logging.INFO,
    )

    # configuring and staring mujoco
    handle = _get_handle(args.vicon_segment_id)

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
