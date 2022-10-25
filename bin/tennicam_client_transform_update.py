#!/usr/bin/env python3

"""
Implements a dialog that allows to update the transform applied by the driver
to the ball before they are written in the (o80) shared memory.
tennicam_client needs to be started in active mode (using the starting dialog
of tennicam_client)
"""


import tennicam_client

TENNICAM_CLIENT_SEGMENT_ID = "tennicam_client"


def _update_transform(segment_id: str, dim: int, value: float, translation: bool):
    dim = int(dim)
    value = float(value)
    transform = tennicam_client.read_transform_from_memory(segment_id)

    if translation:
        transform[0][dim] = value
    else:
        transform[1][dim] = value
    tennicam_client.write_transform_to_memory(segment_id, transform[0], transform[1])


def _update_translation(segment_id: str, dim: int, value: float):
    _update_transform(segment_id, dim, value, True)


def _update_rotation(segment_id: str, dim: int, value: float):
    _update_transform(segment_id, dim, value, False)


def _print_transform(transform):
    print("\ttransform:")
    print("\t\ttranslation:", transform[0])
    print("\t\trotation:", transform[1])


def user_update_transform(segment_id, config_file_path):

    transform = tennicam_client.read_transform_from_memory(segment_id)

    print()
    _print_transform(transform)
    print()

    translation = input("\tenter 0 to change translation, 1 to change rotation: ")
    translation = int(translation)
    if translation not in (0, 1):
        raise ValueError("value should be 0 or 1")

    values = input("\tnew values (three values separated by spaces):")
    values = values.split(" ")
    if len(values)!=3:
        raise ValueError("3 values expected")
    try:
        values = [float(v) for v in values]
    except Exception as e:
        raise ValueError(f"at least one value could not be cast to float: {e}")

    if translation == 0:
        for dim,value in enumerate(values):
            _update_translation(segment_id, dim, value)
    else:
        for dim,value in enumerate(values):
            _update_rotation(segment_id, dim, value)

    transform = tennicam_client.read_transform_from_memory(segment_id)

    print()
    _print_transform(transform)
    print()

    save = input("\ttype 1 if you wish to save this transform: ")
    try:
        save = int(save)
    except:
        pass
    else:
        if save == 1:
            tennicam_client.update_transform_config_file(
                str(config_file_path), transform[0], transform[1]
            )
            print()
            print("transform saved in", config_file_path)

    print()


if __name__ == "__main__":
    user_update_transform(
        TENNICAM_CLIENT_SEGMENT_ID, tennicam_client.get_default_config_file()
    )
