#!/usr/bin/env python3

import pathlib
import signal_handler
import tennicam_client
from lightargs import BrightArgs

"""
Dump the output of tennicam_client into a file,
with format for each line:
 repr([ball_id,time_stamp,position,velocity])
with:
- ball_id : int
- time_stamp: int (nanoseconds)
- position: 3d tuple
- velocity: 3d tuple
"""

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


def _run(segment_id: str, filepath: pathlib.Path):
    """
    Creates an o80 frontend and uses it to get all
    ball observations and dumping a corresponding 
    string in filepath.
    """
    
    frontend = tennicam_client.FrontEnd(segment_id)
    iteration = frontend.latest().get_iteration()

    getters = ("get_ball_id", "get_time_stamp", "get_position", "get_velocity")

    with filepath.open(mode="w") as f:
        try:
            while not signal_handler.has_received_sigint():
                iteration += 1
                obs = frontend.read(iteration)
                values = tuple((getattr(obs, getter)() for getter in getters))
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
        "filepath",
        str(_unique_path(TENNICAM_CLIENT_DEFAULT_FOLDER)),
        "absolute path of the log file",
        str,
    )
    change_all = False
    config.dialog(change_all)
    print()
    return config


if __name__ == "__main__":
    config = _configure()
    segment_id = config.segment_id
    filepath = pathlib.Path(config.filepath)
    _run(segment_id, filepath)
