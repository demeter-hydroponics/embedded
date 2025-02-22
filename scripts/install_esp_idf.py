import os


def make_alias():
    # Add alias to bashrc with the path to libs/esp-idf. alias is get_demeter_esp_idf
    bashrc_path = os.path.expanduser("~/.bashrc")

    esp_idf_export_path = os.path.join(os.getcwd())
    esp_idf_export_path += "/export.sh"

    # check if alias already exists
    with open(bashrc_path, "r") as f:
        for line in f:
            if "alias get_demeter_esp_idf" in line:
                return

    with open(bashrc_path, "a") as f:
        f.write(f'alias get_demeter_esp_idf=". {esp_idf_export_path}"\n')

    print(
        "Added alias get_demeter_esp_idf to ~/.bashrc. Run `source ~/.bashrc` to use it."
    )


def main():
    # cd to libs/esp-idf
    esp_idf_dir = os.path.join(os.getcwd(), "libs", "esp-idf")
    os.chdir(esp_idf_dir)

    os.system("git checkout release/v5.4")

    # Install ESP-IDF by running the install script
    os.system("./install.sh esp32,esp32s3")


if __name__ == "__main__":
    main()
    make_alias()
