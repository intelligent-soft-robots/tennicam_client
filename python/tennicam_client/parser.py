import typing
import pathlib

Position = typing.TypeVar("Position", float, float, float)
Velocity = typing.TypeVar("Velocity", float, float, float)
Entry = typing.TypeVar("Entry", int, int, Position, Velocity)


def parse(filepath: pathlib.Path) -> typing.Generator[Entry, None, bool]:
    """
    Parse the file and yield information about the ball
    
    Args:
        filepath: absolute path of a file generated using tennicam_client_logger
    
    Returns:
        tuples: (ball_id: int, time_stamp: int, position: 3d tuple, velocity: 3d tuple)
        time_stamp is in milliseconds.
        ball_id is -1 if invalid ball (ball not detected by the visual tracking)
    """

    if not filepath.exists():
        raise FileNotFoundError(
            "tennicam_client parse: failed to find: {}".format(filepath)
        )

    with open(filepath) as f:
        for line in f:
            try:
                yield eval(line)
            except:
                return False
                
    return True
