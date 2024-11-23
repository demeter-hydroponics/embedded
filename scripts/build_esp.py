import os
import argparse

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
    # confirm esp-idf is sourced
    esp_idf_path = os.environ.get("IDF_PATH")
    if esp_idf_path is None:
        raise Exception("IDF_PATH is not set. Run `get_demeter_esp_idf` to set it.")

    proj_root = os.getcwd()

    # create the config file
    private_dir = os.path.join(proj_root, "private")
    create_config_file(private_dir)

    paths = []

    if args.project == "all":
        # get the current working directory
        paths = [
            os.path.join(proj_root, project_path)
            for project_path in project_paths.values()
        ]
    else:
        paths = [os.path.join(proj_root, project_paths[args.project])]

    for path in paths:
        os.chdir(path)
        if args.build is True:
            os.system("idf.py build")

        if args.flash:
            os.system(f"idf.py -p {args.port} flash")

        if args.monitor:
            os.system(f"idf.py -p {args.port} monitor")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build ESP32 projects.")

    # add an arg to list what project to build
    parser.add_argument(
        "--project", help="Project to build", choices=project_paths.keys()
    )
    parser.add_argument(
        "--build", help="Build the project", action="store_true", default=True
    )
    parser.add_argument(
        "--flash", help="Flash the project to the ESP32", action="store_true"
    )
    parser.add_argument(
        "--monitor", help="Monitor the serial output of the ESP32", action="store_true"
    )

    # add an arg to specify the port
    parser.add_argument("--port", help="Serial port for the ESP32")

    args = parser.parse_args()

    # if flash or monitor are defined, ensure that there is a port defined
    if (args.flash or args.monitor) and (args.port is None):
        raise Exception("Port must be defined to flash or monitor the ESP32")
    main(args)
