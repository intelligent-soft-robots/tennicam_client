#!/usr/bin/env python3

"""
Print in a terminal the information of the balls
published by tennicam.
Required for this executable:
- in another terminal: 'tennicam_client'
"""

import signal_handler
import tennicam_client

TENNICAM_CLIENT_DEFAULT_SEGMENT_ID = "tennicam_client"


def run():
    global TENNICAM_CLIENT_DEFAULT_SEGMENT_ID
    frontend = tennicam_client.FrontEnd(TENNICAM_CLIENT_DEFAULT_SEGMENT_ID)
    iteration = frontend.latest().get_iteration()
    signal_handler.init()  # for detecting ctrl+c
    try:
        while not signal_handler.has_received_sigint():
            iteration += 1
            obs = frontend.read(iteration)
            ball = obs.get()
            print(ball.to_string())
            # other useful methods:
            # position = obs.get_position()
            # velocity = obs.get_velocity()
            # iteration = obs.get_iteration()
            # time_stamp = obs.get_time_stamp()
            # ball_id = obs.get_ball_id()
    except (KeyboardInterrupt, SystemExit):
        pass
    except Exception as e:
        print("Error:", e)


if __name__ == "__main__":
    run()
