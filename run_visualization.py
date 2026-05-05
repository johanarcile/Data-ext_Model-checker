"""Wrapper to run scripts/visualize_state_space.py from project root.
Usage: python visualize_state_space.py [path/to/state_space]
"""
import sys
from pathlib import Path
import traceback
import os

ROOT = Path(__file__).resolve().parent
SCRIPTS = ROOT / "scripts"
sys.path.insert(0, str(SCRIPTS))

print(f"[vss-wrapper] CWD={os.getcwd()} argv={sys.argv}", flush=True)
try:
    import visualize_state_space as vss
    print("[vss-wrapper] imported scripts/visualize_state_space", flush=True)
except Exception as e:
    print("[vss-wrapper] failed to import scripts/visualize_state_space.py", flush=True)
    traceback.print_exc()
    raise SystemExit(f"Failed to import scripts/visualize_state_space.py: {e}")


def main():
    # forward args (strip this wrapper name)
    try:
        print("[vss-wrapper] calling vss.main()", flush=True)
        vss.main()
    except Exception as e:
        print("[vss-wrapper] exception in vss.main()", flush=True)
        traceback.print_exc()
        raise


if __name__ == "__main__":
    main()
