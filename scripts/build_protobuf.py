import os
import subprocess

# Path to the protobuf library relative to this script
PROTOBUF_LIB_PATH = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../libs/protobuf")
)

# Make the generated location at ../generated, absolute relative to this file's location
GENERATED_PATH = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "generated")
)


def build_protobuf():
    # Define the path to the script to run
    generate_script_path = os.path.join(
        PROTOBUF_LIB_PATH, "scripts/generate_protobuf.py"
    )

    # Build the command for subprocess
    command = [
        "python3",
        generate_script_path,
        "--generate_python",
        "--output_dir",
        GENERATED_PATH,  # Pass the output directory as an argument
    ]

    try:
        # Execute the command in a subprocess and capture output
        result = subprocess.run(
            command,
            cwd=PROTOBUF_LIB_PATH,  # Change to the protobuf library directory
            stdout=subprocess.PIPE,  # Capture standard output
            stderr=subprocess.PIPE,  # Capture standard error
            text=True,  # Output as text
            check=True,  # Raise exception on non-zero exit
        )
        print("Protobuf generation succeeded!")
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print("Protobuf generation failed!")
        print(f"Return code: {e.returncode}")
        print(f"Error output: {e.stderr}")


if __name__ == "__main__":
    build_protobuf()
