import re
import io
import json
import os, sys
import platform, shutil
from pathlib import Path
from zipfile import ZipFile
from itertools import product
from subprocess import Popen, PIPE, CalledProcessError


def execute(cmd:list, env:dict, cwd:str):
    popen = Popen(
        cmd, env=env, shell=True, cwd=cwd,
        stdout=PIPE, stderr=PIPE
    )
    for char in iter(lambda: (popen.stdout.read(1), popen.stderr.read(1)), b''):
        len0 = len(char[0])
        len1 = len(char[1])
        if len0 > 0:
            sys.stdout.buffer.write(char[0])
        if len1 > 0:
            sys.stderr.buffer.write(char[1])
        if len0 <= 0 and len1 <= 0:
            break
    popen.stdout.close()
    popen.stderr.close()
    return_code = popen.wait()
    if return_code:
        raise CalledProcessError(return_code, cmd)

def collectMetadata():
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
    
    return metadata

def build(metadata:dict, cmds:list, cmd_matrix:dict, env:dict):
    def _update_cmd_env():
        tmp_env = env.copy()
        if cmd_matrix is not None:
            tmp_env = env.copy()
            expanded_matrix = (dict(zip(cmd_matrix.keys(), values)) for values in product(*cmd_matrix.values()))
            for i in expanded_matrix:
                for k,v in i.items():
                    tmp_env[k] = v
                os.environ = tmp_env
                yield i
        else:
            yield dict()
    
    for cmd_env in _update_cmd_env():
        print('cmd env:', cmd_env)
        for cmd in cmds:
            cmd = os.path.expandvars(cmd)
            cmd = cmd.split(' ')
            if cmd[0] == 'python':
                exe = sys.executable
            else:
                exe = shutil.which(cmd[0])
            exe = exe if exe is not None else cmd[0]
            newcmd = [exe, *cmd[1:]] if metadata['platform'] == 'win' else ' '.join([exe, *cmd[1:]])
            print(newcmd)
            execute(newcmd, env, env['WorkingDir'])

def pack(zip_filename:str, metadata:dict, config:dict, env:dict):
    with ZipFile(zip_filename, 'w') as zp:
        for file in config['Files']:
            file = os.path.expandvars(file)
            file = Path(file).absolute()
            if not file.exists():
                raise ValueError(f'Input file does not exists: {file}')
            zp.write(file, config['ArchiveBaseName'])
        for folder in config['Folders']:
            folder = os.path.expandvars(folder)
            folder = Path(folder).absolute()
            if not folder.exists():
                raise ValueError(f'Input folder does not exists: {folder}')
            for file in folder.rglob('*'):
                if file.is_file():
                    zp.write(file, f"{config['ArchiveBaseName']}/{file.relative_to(folder)}")
    print('output to:', zip_filename)


def main():
    if len(sys.argv) != 2:
        print(f'Usage: python3 Release.py /path/to/release_config.json')
        exit(0)
    
    metadata = collectMetadata()
    
    config_file = Path(sys.argv[1])
    config = None
    with open(config_file, 'r', encoding='utf-8') as file:
        raw = file.read()
        if metadata['platform'] == 'win':
            raw = re.sub(r'\$(\w+)', '%\g<1>%', raw)
        config = json.loads(raw)
    
    build_env = {
        'REPO_ROOT': str(Path(__file__).parent.absolute())
    }
    original_env = os.environ.copy()
    build_env = {**os.environ.copy(), **build_env}
    os.environ = build_env
    
    version_file = Path(os.path.expandvars(config['Version']))
    if version_file.exists():
        with open(version_file, 'r', encoding='utf-8') as file:
            metadata['app_version'] = file.read().strip()
    else:
        metadata['app_version'] = config['Version']
    
    matrix = config['Matrix']
    
    print('version:', version_file)
    print('metadata:', metadata)
    
    def _build_once(loc_build_env, attributes):
        os.environ = loc_build_env
        working_dir = config['Cmd']['WorkingDir']
        working_dir = os.path.expandvars(working_dir)
        loc_build_env['WorkingDir'] = working_dir
        build_dir_name = config['Cmd']['BuildDirName']
        loc_build_env['BuildDirName'] = build_dir_name
        build_dir_name = Path(build_dir_name)
        if build_dir_name.exists():
            shutil.rmtree(build_dir_name, ignore_errors=True)
        build_dir_name.mkdir(exist_ok=True)
        print('working_dir:', working_dir)
        print('build_dir_name:', build_dir_name)
        
        build_cmd_matrix = config['Cmd']['BuildCmdMatrix'] if 'BuildCmdMatrix' in config['Cmd'] else None
        build(metadata, config['Cmd']['BuildCmd'], build_cmd_matrix, loc_build_env)
        
        attribute_str = '-'.join(attributes)
        if len(attribute_str) > 0:
            attribute_str = '-'+attribute_str
        package_name = f'{config["ProjectName"]}-{metadata["app_version"]}-{metadata["platform"]}{metadata["architecture"]}{attribute_str}.zip'
        pack(package_name, metadata, config['Cmd']['PackRelease'], loc_build_env)
        os.environ = loc_build_env
    
    if matrix is None:
        _build_once(build_env, '')
    else:
        tmp_build_env = build_env.copy()
        for k,v in matrix.items():
            print('build env:', k, v)
            for i in v:
                tmp_build_env[k] = i['value']
                _build_once(tmp_build_env, i['attributes'])
    
    os.environ = original_env



if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()
        exit(0)
    except Exception as err:
        raise err
