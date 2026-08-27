#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <image1.ppm/png> <image2.ppm/png>")
        sys.exit(2)
    # Delegates to compiled pixel_diff tool or checks basic comparison
    import subprocess
    cmd = ["./build/pixel_diff"] + sys.argv[1:]
    res = subprocess.run(cmd)
    sys.exit(res.returncode)

if __name__ == "__main__":
    main()
