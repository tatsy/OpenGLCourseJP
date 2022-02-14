import os
import pytest
import platform
import subprocess

SRC_DIR = "src"
EXE_DIR = "build/bin"

programs = [f for f in os.listdir(SRC_DIR) if os.path.isdir(os.path.join(SRC_DIR, f))]


@pytest.mark.parametrize('exe', programs)
def test_cxx(exe):
    exe_path = os.path.join(EXE_DIR, exe)
    if platform.system() == "Windows":
        exe_path += ".exe"

    assert os.path.isfile(exe_path), "executable \"{:s}\" not fould!".format(exe_path)
    p = subprocess.Popen([exe_path], shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    with pytest.raises(subprocess.TimeoutExpired):
        outs, errs = p.communicate(timeout=1)

    p.kill()
    outs, errs = p.communicate()

    outs, errs = outs.decode('ascii'), errs.decode('ascii')
    print("retcode={:d}, stdout={:s}, stderr={:s}".format(p.returncode, outs, errs))


@pytest.mark.parametrize('code', programs)
def test_python(code):
    code_path = os.path.join(SRC_DIR, code, code + ".py")

    assert os.path.isfile(code_path), "executable \"{:s}\" not fould!".format(code_path)
    p = subprocess.Popen(["python", code_path], shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    with pytest.raises(subprocess.TimeoutExpired):
        outs, errs = p.communicate(timeout=1)
        outs, errs = outs.decode('ascii'), errs.decode('ascii')
        print("retcode={:d}, stdout={:s}, stderr={:s}".format(p.returncode, outs, errs))

    p.kill()
    p.communicate()
