import subprocess
import sys
from pathlib import Path


SCRIPT_ROOT = Path(__file__).resolve().parent


def execute(script):

    script_path = SCRIPT_ROOT / script

    print()
    print("=" * 70)
    print(f"RUNNING: {script}")
    print("=" * 70)

    result = subprocess.run(
        [
            sys.executable,
            str(script_path)
        ]
    )

    if result.returncode != 0:

        print()
        print(f"[ERROR] {script}")

        sys.exit(
            result.returncode
        )


def main():

    print()
    print("=" * 70)
    print("KALEX URBAN")
    print("SJL CITY GENERATOR")
    print("=" * 70)

    execute(
        "download_osm.py"
    )

    execute(
        "process_osm.py"
    )

    print()
    print("=" * 70)
    print("CITY DATA GENERATED")
    print("=" * 70)


if __name__ == "__main__":
    main()