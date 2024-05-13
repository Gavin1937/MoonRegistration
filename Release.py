import re
import json
import os, sys
import argparse
import platform, shutil
from pathlib import Path
from zipfile import ZipFile
from itertools import product
from subprocess import Popen, PIPE, CalledProcessError


DEFAULT_RELEASE_STEPS = [
    'build', 'pack'
]

def execute(cmd:list, env:dict, cwd:str):
    popen = Popen(
        cmd, env=env, shell=True, cwd=cwd,
        stdout=PIPE, stderr=PIPE
    )
    # block process, wait until popen finished
    popen.wait()
    for char in iter(lambda: (popen.stdout.read(16), popen.stderr.read(16)), b''):
        has_out = len(char[0]) > 0
        has_err = len(char[1]) > 0
        if has_err and not has_out:
            sys.stderr.buffer.write(char[1])
            sys.stderr.flush()
        elif has_err and has_out:
            sys.stderr.buffer.write(char[1])
            sys.stderr.flush()
            sys.stdout.buffer.write(char[0])
        elif not has_err and has_out:
            sys.stdout.buffer.write(char[0])
            sys.stdout.flush()
        else: # not has_err and not has_out
            break
    sys.stdout.flush()
    sys.stderr.flush()
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
    
    mach = platform.machine().lower()
    arch = platform.architecture()
    if '64' in arch[0]: metadata['architecture'] = mach+'_64'
    elif '32' in arch[0]: metadata['architecture'] = mach+'_32'
    else: metadata['architecture'] = mach+'_unknown'
    
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
            cmd = [c for c in cmd if len(c.strip()) > 0]
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


def main(args:argparse.Namespace):
    if 'release_config' not in args or args.release_config is None:
        print(f'Usage: python3 Release.py /path/to/release_config.json')
        exit(0)
    
    metadata = collectMetadata()
    
    config_file = Path(args.release_config)
    config = None
    with open(config_file, 'r', encoding='utf-8') as file:
        raw = file.read()
        if metadata['platform'] == 'win':
            raw = re.sub(r'\$(\w+)', '%\g<1>%', raw)
        config = json.loads(raw)
    
    release_steps = None
    if 'steps' in args and args.steps is not None:
        release_steps = list(map(lambda i:i.lower(),args.steps.split(',')))
        for rel in release_steps:
            if rel not in DEFAULT_RELEASE_STEPS:
                raise ValueError(f'Input step does not exists: {rel}')
    if release_steps is None:
        release_steps = DEFAULT_RELEASE_STEPS
    
    specified_working_matrix = None
    if 'matrix' in args and args.matrix is not None:
        specified_working_matrix = list(
            map(lambda i:i.strip().split('='),filter(lambda s:len(s.strip())>0,args.matrix.split(';')))
        )
        for key,_ in specified_working_matrix:
            if key not in config['Matrix'].keys():
                raise ValueError('Input matrix key does not exists: %s' % key)
    
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
    
    def _matrix_proc_once(loc_build_env, attributes):
        # filtering matrix process by specific matrix key value
        if specified_working_matrix:
            for k,v in specified_working_matrix:
                if loc_build_env[k] != v:
                    print(f'Skip specified working matrix: {k}={v}')
                    return
        
        os.environ = loc_build_env
        working_dir = config['Cmd']['WorkingDir']
        working_dir = os.path.expandvars(working_dir)
        loc_build_env['WorkingDir'] = working_dir
        build_dir_name = config['Cmd']['BuildDirName']
        loc_build_env['BuildDirName'] = build_dir_name
        build_dir_name = Path(build_dir_name)
        print('working_dir:', working_dir)
        print('build_dir_name:', build_dir_name)
        
        def _loc_build_step():
            if build_dir_name.exists():
                shutil.rmtree(build_dir_name, ignore_errors=True)
            build_dir_name.mkdir(exist_ok=True)
            build_cmd_matrix = config['Cmd']['BuildCmdMatrix'] if 'BuildCmdMatrix' in config['Cmd'] else None
            build(metadata, config['Cmd']['BuildCmd'], build_cmd_matrix, loc_build_env)
        
        def _loc_pack_step():
            attribute_str = '-'.join(attributes)
            if len(attribute_str) > 0:
                attribute_str = '-'+attribute_str
            package_name = f'{config["ProjectName"]}-{metadata["app_version"]}-{metadata["platform"]}-{metadata["architecture"]}{attribute_str}.zip'
            pack(package_name, metadata, config['Cmd']['PackRelease'], loc_build_env)
            os.environ = loc_build_env
        
        for curr_step in release_steps:
            print(f'Running step: {curr_step}')
            if curr_step == 'build':
                _loc_build_step()
            elif curr_step == 'pack':
                _loc_pack_step()
    
    if matrix is None:
        _matrix_proc_once(build_env, '')
    else:
        tmp_build_env = build_env.copy()
        for k,v in matrix.items():
            print('build env:', k, v)
            for i in v:
                tmp_build_env[k] = i['value']
                _matrix_proc_once(tmp_build_env, i['attributes'])
    
    os.environ = original_env



if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '-s', '--steps',
        help='run with specified steps: "build,pack"',
        action='store', type=str
    )
    parser.add_argument(
        '-m', '--matrix',
        help='run on specified matrix: "key1=value1; key2=value2"',
        action='store', type=str
    )
    parser.add_argument('release_config', metavar='RELEASE_CONFIG', type=str, help='release config path')
    args = parser.parse_args()
    
    
    try:
        main(args)
    except KeyboardInterrupt:
        print()
        exit(0)
    except Exception as err:
        raise err
