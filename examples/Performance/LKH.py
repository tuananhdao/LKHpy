import sys
import LkhPy as lk

def main():
    if len(sys.argv) < 2:
        print("Usage: python example.py example.par")
        sys.exit(1)

    fileName = sys.argv[1]
    lk.FromPar(fileName)

if __name__ == "__main__":
    main()