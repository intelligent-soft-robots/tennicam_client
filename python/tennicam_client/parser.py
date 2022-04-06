import typing
import pathlib
import pam_configuration

Position = typing.Sequence[float]
Velocity = typing.Sequence[float]
Entry = typing.Tuple[int, int, Position, Velocity]

_CONFIG_FILE_SUFFIX = pathlib.Path("tennicam_client") / "config.toml"


def get_default_config_file() -> pathlib.Path:
    """
    Returns the absolute path to the default configuration, as it has been
    installed by the pam_configuration package, i.e.
    ~/.mpi-is/pam//tennicam_client/config.toml (if it exists) or
    /opt/mpi-is/pam/tennicam_client/config.toml (otherwise)
    """

    return pathlib.Path(pam_configuration.get_path()) / _CONFIG_FILE_SUFFIX


def parse(filepath: pathlib.Path) -> typing.Generator[Entry, None, bool]:
    """
    Parse the file and yield information about the ball

    Args:
        filepath: absolute path of a file generated using tennicam_client_logger

    Returns:
        tuples: (ball_id: int, time_stamp: int, position: 3d tuple, velocity: 3d tuple)
        time_stamp is in nanoseconds.
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
