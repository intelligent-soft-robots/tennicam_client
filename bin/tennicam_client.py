#!/usr/bin/env python3

"""
Instantiate an o80 Standalone for tennicam, i.e.
this process will subscribed to tennicam and write into the 
shared memory the related ball information.
"""

import sys
import time
import logging
import o80
import signal_handler
import tennicam_client
from lightargs import BrightArgs, Positive, FileExists

TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"
TENNICAM_CLIENT_DEFAULT_FREQUENCY = 200.0


def configure():
    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID
    global TENNICAM_CLIENT_DEFAULT_FREQUENCY
    config = BrightArgs("o80 tennicam client standalone")
    config.add_option(
        "segment_id",
        TENNICAM_CLIENT_DEFAULT_SEGMENT_ID,
        "segment_id of the o80 backend",
        str,
    )
    config.add_option(
        "config_path",
        tennicam_client.get_default_config_file(),
        "configuration file",
        str,
        [FileExists()],
    )
    config.add_option(
        "frequency", TENNICAM_CLIENT_DEFAULT_FREQUENCY, float, [Positive()]
    )
    config.add_option(
        "active_transform",
        False,
        "if true, the driver will read transform parameter at each iteration",
        bool,
    )
    change_all = False
    config.dialog(change_all, sys.argv[1:])
    print()
    return config


def run(segment_id, frequency, config_path, active_transform):

    log_handler = logging.StreamHandler(sys.stdout)
    logging.basicConfig(
        format=str(
            "[tennicam_client | segment_id: {} |"
            + " %(levelname)s %(asctime)s] %(message)s"
        ).format(segment_id),
        level=logging.DEBUG,
        handlers=[log_handler],
    )

    logging.info("clearning shared memory on {}".format(segment_id))
    o80.clear_shared_memory(segment_id)

    logging.info("starting o80 standalone with frequency {}".format(frequency))
    if active_transform:
        active_transform_segment_id = segment_id
    else:
        active_transform_segment_id = ""
    tennicam_client.start_standalone(
        segment_id, frequency, False, config_path, active_transform_segment_id
    )

    logging.info("running, ctrl+c for exiting")
    signal_handler.init()  # for detecting ctrl+c
    try:
        while not signal_handler.has_received_sigint():
            time.sleep(0.01)
    except (KeyboardInterrupt, SystemExit):
        logging.info("exiting ...")
    except Exception as e:
        logging.error(str(e))

    logging.info("stopping o80 tennicam")
    tennicam_client.stop_standalone(segment_id)
    logging.info("exiting")


if __name__ == "__main__":
    config = configure()
    run(
        config.segment_id, config.frequency, config.config_path, config.active_transform
    )
