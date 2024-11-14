import os
import argparse

project_paths = {
    "node-controller": "controllers/node-controller",
    "pump-controller": "controllers/pump-controller",
}


def main(args):
    # confirm esp-idf is sourced
    esp_idf_path = os.environ.get("IDF_PATH")
    if esp_idf_path is None:
        raise Exception("IDF_PATH is not set. Run `get_demeter_esp_idf` to set it.")

    proj_root = os.getcwd()

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
    parser.add_argument(
        "--port", help="Port to flash/monitor the ESP32 on", default="/dev/ttyUSB0"
    )

    args = parser.parse_args()

    # if flash or monitor are defined, ensure that there is a port defined
    if args.flash or args.monitor and not args.port:
        raise Exception("Port must be defined to flash or monitor the ESP32")
    main(args)
