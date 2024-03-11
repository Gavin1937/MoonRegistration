#!/usr/bin/env python3

from pathlib import Path
from zipfile import ZipFile
from subprocess import Popen
from platform import system as currentOS


def pack(root, config) -> None:
    
    print("Packing Release...")
    
    # prep
    archive = root/config["archive_name"]
    
    # rm old archive
    if archive.exists():
        archive.unlink()
    
    # pack new archive
    print("Save Release to: "+str(archive))
    with ZipFile(archive, "w") as zip:
        for single in config["singlefiles"]:
            zip.write(single, config["name"]+"/"+str(single.relative_to(root)))
        for recur_root, recur in config["recursivefile"]:
            if recur is not None and recur.exists():
                for r in recur.rglob("*"):
                    if r.is_file():
                        zip.write(r, config["name"]+"/"+str(r.relative_to(recur_root)))


if __name__ == "__main__":
    VERSION = ""
    ROOT = Path(".")
    with open("VERSION", "r") as file:
        VERSION = file.read().strip();
    ARCHITECTURES = [
        ("linux-x86_64", "build_linux"),
        ("js", "platforms/js/build_wasm"),
    ]
    for ARC,BUILD_PATH in ARCHITECTURES:
        BUILD_PATH = ROOT/BUILD_PATH
        pack(ROOT, {
            "name": "",
            "archive_name": "MoonRegistration-"+VERSION+"-"+ARC+".zip",
            "singlefiles": [],
            "recursivefile": [
                (BUILD_PATH/"release", BUILD_PATH/"release"),
            ],
        })
