#!/usr/bin/env python3

"""
Display in a mujoco simulated environment the ball
as published by tennicam and the robot.
Required for this executable:
- in a first terminal: start robot (e.g. o80_pamy2)
- in a second terminal: 'tennicam_client'
- in a third terminal :'pam_mujoco mirroring'
"""

import sys, time, logging
import o80, o80_pam, pam_mujoco, context
import signal_handler
import tennicam_client
import pam_mujoco
from pam_mujoco import mirroring
from lightargs import BrightArgs, Set, Range, Positive, FileExists

from learning_table_tennis_from_scratch.hysr_one_ball import HysrOneBallConfig
from learning_table_tennis_from_scratch.jsonconfig import get_json_config


# required for this executable:
# in a first terminal: start robot (e.g. o80_pamy2)
# in a second terminal: 'tennicam_client'
# in a third terminal :'pam_mujoco mirroring'


SEGMENT_ID_ROBOT_MIRROR = "simulated_robot"
MUJOCO_ID_MIRRORING = "mirroring"

TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"


def configure():
    config = BrightArgs(
        "pam mirroring: having an instance of pam_mujoco mirroring another robot"
    )
    config.add_option(
        "segment_id_real_robot",
        o80_pam.segment_ids.robot,
        "segment_id of the robot that should be mirrored",
        str,
    )
    config.add_option(
        "mujoco_id_mirroring",
        MUJOCO_ID_MIRRORING,
        "mujoco_id of the pam_mujoco instance that will mirror the other robot",
        str,
    )
    config.add_option("frequency", 300, "mirroring frequency", float, [Positive()])

    change_all = False
    config.dialog(change_all)
    print()
    return config



def get_hysr_config_path():
    files = get_json_config(expected_keys=["hysr_config"])
    return files["hysr_config"]

def configure_simulation(
    mujoco_id=MUJOCO_ID_MIRRORING,
    segment_id=SEGMENT_ID_ROBOT_MIRROR,
    graphics=True,
):

    accelerated_time = False
    burst_mode = False

    hysr_path = get_hysr_config_path()
    hysr_config = HysrOneBallConfig.from_json(hysr_path)

    robot = pam_mujoco.MujocoRobot(
        segment_id, control=pam_mujoco.MujocoRobot.JOINT_CONTROL,
        position=hysr_config.robot_position
    )
    
    ball = pam_mujoco.MujocoItem(
    "ball", control=pam_mujoco.MujocoItem.CONSTANT_CONTROL, color=(1, 0, 0, 1))
    balls = (ball,)
    table = pam_mujoco.MujocoTable(
        "table",
        position=hysr_config.table_position,
        orientation=hysr_config.table_orientation
    )
    
    handle = pam_mujoco.MujocoHandle(
        mujoco_id,
        graphics=graphics,
        accelerated_time=accelerated_time,
        burst_mode=burst_mode,
        robot1=robot,
        balls=balls,
        table=table,
    )

    return handle


def run():

    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID

    config = configure()

 

    # running at 100Hz
    frequency = 100.0
    frequency_manager = o80.FrequencyManager(frequency)
    duration_ms = o80.Duration_us.microseconds(int((1.0 / frequency) * 1e6))

    signal_handler.init()  # for detecting ctrl+c
    
    log_handler = logging.StreamHandler(sys.stdout)
    logging.basicConfig(
        format="[pam mirroring {} {}] %(message)s".format(
            config.segment_id_real_robot, config.mujoco_id_mirroring
        ),
        level=logging.DEBUG,
        handlers=[log_handler],
    )

    logging.info(
        "creating o80 frontend to the robot to mirror: {}".format(
            config.segment_id_real_robot
        )
    )
    pressures = o80_pam.o80Pressures(config.segment_id_real_robot)

    handle = configure_simulation(
        mujoco_id=config.mujoco_id_mirroring
    )
    joints = handle.interfaces[SEGMENT_ID_ROBOT_MIRROR]

    # getting a frontend for ball control
    ball = handle.frontends["ball"]

    # getting a frontend to the visually tracked ball
    frontend = tennicam_client.FrontEnd(TENNICAM_CLIENT_DEFAULT_SEGMENT_ID)

    logging.info("creating frequency manager")
    frequency_manager = o80.FrequencyManager(config.frequency)

    logging.info("starting")


    signal_handler.init()  # for detecting ctrl+c
    try:
        while not signal_handler.has_received_sigint():
            try:
                _, __, joint_positions, joint_velocities = pressures.read()
                print(
                    "{:.2f} {:.2f} {:.2f} | {:.2f} {:.2f} {:.2f}".format(
                        *joint_positions, *joint_velocities
                    )
                )
                joints.set(
                    joint_positions, joint_velocities, duration_ms=None, wait=False
                )
                
                # getting information from tennicam
                ball_zmq = frontend.latest()
                position = ball_zmq.get_position()
                velocity = ball_zmq.get_velocity()
                # sending related command to mujoco ball
                ball.add_command(position, velocity, duration_ms, o80.Mode.OVERWRITE)
                ball.pulse()
            except Exception as e:
                logging.info("exception: {}. keyboard interrupt ? ".format(e))
                break
            frequency_manager.wait()
    except KeyboardInterrupt:
        logging.info("keyboard interrupt, exiting")

    logging.info("exit")
    print()




if __name__ == "__main__":
    run()



