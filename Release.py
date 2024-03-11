import json
import os, sys
import platform
from pathlib import Path
from zipfile import ZipFile
from subprocess import Popen, PIPE


def collectMetadata(config:dict):
    metadata = {}
    
    plat = platform.system()
    if plat == 'Windows': metadata['platform'] = 'win'
    elif plat == 'Linux': metadata['platform'] = 'linux'
    elif plat == 'Darwin': metadata['platform'] = 'mac'
    elif plat == 'Java': metadata['platform'] = 'java'
    else: metadata['platform'] = 'unknown'
    
    arch = platform.architecture()
    if '64' in arch[0]: metadata['architecture'] = '64'
    elif '32' in arch[0]: metadata['architecture'] = '32'
    else: metadata['architecture'] = 'unknown'
    
    metadata['python_version'] = platform.python_version_tuple()
    
    version_file = Path(config['Version'])
    if version_file.exists():
        with open(version_file, 'r', encoding='utf-8') as file:
            metadata['app_version'] = file.read().strip()
    else:
        metadata['app_version'] = config['Version']
    
    return metadata

def build(metadata:dict, config:dict, env:dict):
    for cmd in config:
        if metadata['platform'] == 'win':
            cmd = cmd.replace('$', '$env:')
        print(cmd)
        print(env['BuildDirName'])
        tmp = Popen(['echo', '"$BuildDirName"'], shell=True, env=env, stdout=PIPE, stderr=PIPE)
        print(tmp.stdout.read())
        # proc = Popen(
        #     [cmd], env=env, shell=True, cwd=env['WorkingDir'],
        # )

def pack(zip_filename:str, metadata:dict, config:dict, env:dict):
    with ZipFile(zip_filename, 'w') as zp:
        for file in config['Files']:
            file = Path(file)
            if not file.exists():
                raise ValueError(f'Input file does not exists: {file}')
            zp.write(file, config['ArchiveBaseName'])
        for folder in config['Folders']:
            folder = Path(folder)
            if not folder.exists():
                raise ValueError(f'Input folder does not exists: {folder}')
            for file in folder.rglob('*'):
                if file.is_file():
                    zp.write(file, f"{config['ArchiveBaseName']}/{file.relative_to(folder)}")


def main():
    if len(sys.argv) != 2:
        print(f'Usage: python3 Release.py /path/to/release_config.json')
        exit(0)
    
    config_file = Path(sys.argv[1])
    config = None
    with open(config_file, 'r', encoding='utf-8') as file:
        config = json.load(file)
    
    metadata = collectMetadata(config)
    
    matrix = config['Matrix']
    if matrix is None:
        matrix = ["EMPTY"]
    
    build_env = {
        "REPO_ROOT": str(config_file.parent.absolute())
    }
    
    for mat in matrix:
        working_dir = config['Cmd']['WorkingDir']
        build_env['WorkingDir'] = working_dir
        build_dir_name = config['Cmd']['BuildDirName']
        build_env['BuildDirName'] = build_dir_name
        
        build(metadata, config['Cmd']['BuildCmd'], {**os.environ, **build_env})
        
        package_name = f'{config["ProjectName"]}-{metadata["app_version"]}-{metadata["platform"]}{metadata["architecture"]}.zip'
        pack(package_name, metadata, config['Cmd']['PackRelease'], {**os.environ, **build_env})


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()
        exit(0)
    except Exception as err:
        raise err
