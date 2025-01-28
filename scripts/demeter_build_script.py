import os
import argparse
import click
import sys
from build_protobuf import build_protobuf

project_paths = {
    "node-controller": "controllers/node-controller",
    "pump-controller": "controllers/pump-controller",
}


def create_config_file(private_dir: str):
    if not os.path.exists(private_dir):
        raise Exception(f"Private directory {private_dir} does not exist.")

    # if a file called config_private.h exists, copy its contents to config.h
    config_private_path = os.path.join(private_dir, "config_private.h")
    config_default_path = os.path.join(private_dir, "config_default.h")
    config_path = os.path.join(private_dir, "config.h")

    if os.path.exists(config_private_path):
        with open(config_private_path, "r") as private_file:
            private_contents = private_file.read()
            with open(config_path, "w") as config_file:
                config_file.write(private_contents)
    else:
        with open(config_default_path, "r") as default_file:
            default_contents = default_file.read()
            with open(config_path, "w") as config_file:
                config_file.write(default_contents)


def main(args):
    # Confirm ESP-IDF is sourced
    esp_idf_path = os.environ.get("IDF_PATH")
    if esp_idf_path is None:
        raise Exception("IDF_PATH is not set. Run `get_demeter_esp_idf` to set it.")

    proj_root = os.getcwd()

    # Create the config file
    private_dir = os.path.join(proj_root, "private")
    create_config_file(private_dir)

    paths = []

    # Build Protobuf
    if not build_protobuf():
        click.secho("Failed to build protobufs", fg="red")
        sys.exit(1)

    if args.build is not True:
        click.secho("Note that no build option was specified", fg="yellow")

    if args.project is None:
        # Get the current working directory
        paths = [
            os.path.join(proj_root, project_path)
            for project_path in project_paths.values()
        ]
    else:
        paths = [os.path.join(proj_root, project_paths[args.project])]

    for path in paths:
        os.chdir(path)
        if args.build:
            retcode = os.system("idf.py build")
            if retcode != 0:
                click.secho(f"Failed to build {path}", fg="red")
                sys.exit(1)

    if (args.project is None) and (args.flash or args.monitor):
        click.secho("A project must be specified to flash or monitor", fg="red")
        sys.exit(1)

    if args.flash:
        retcode = os.system(f"idf.py -p {args.port} flash")
        if retcode != 0:
            click.secho("Failed to flash the project", fg="red")
            sys.exit(1)

    if args.monitor:
        retcode = os.system(f"idf.py -p {args.port} monitor")
        if retcode != 0:
            click.secho("Failed to start monitor", fg="red")
            sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build ESP32 projects.")

    # add an arg to list what project to build
    parser.add_argument(
        "--project", help="Project to build", choices=project_paths.keys()
    )
    parser.add_argument("--build", help="Build the project", action="store_true")
    parser.add_argument(
        "--flash", help="Flash the project to the ESP32", action="store_true"
    )
    parser.add_argument(
        "--monitor", help="Monitor the serial output of the ESP32", action="store_true"
    )

    # add an arg to specify the port
    parser.add_argument(
        "--port", help="Serial port for the ESP32", default="/dev/ttyUSB0"
    )

    args = parser.parse_args()

    # if flash or monitor are defined, ensure that there is a port defined
    if (args.flash or args.monitor) and (args.port is None):
        raise Exception("Port must be defined to flash or monitor the ESP32")
    main(args)
