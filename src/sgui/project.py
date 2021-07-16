import os

from sgui import util
from sglib.constants import (
    DEFAULT_PROJECT_DIR,
    MAJOR_VERSION,
)
from sglib.lib.util import set_file_setting, PROJECT_FILE_TYPE
from sglib.log import LOG
from sgui.sgqt import *

__all__ = [
    'new_project',
    'open_project',
]

def new_project(a_parent=None):
    try:
        f_last_dir = DEFAULT_PROJECT_DIR
        while True:
            f_file = QFileDialog.getExistingDirectory(
                a_parent,
                'New Project',
                f_last_dir,
                QFileDialog.ShowDirsOnly | QFileDialog.DontUseNativeDialog,
            )
            if f_file and str(f_file):
                f_file = str(f_file)
                f_last_dir = f_file
                if (
                    not util.check_for_empty_directory(f_file)
                    or
                    not util.check_for_rw_perms(f_file)
                ):
                    continue
                f_file = os.path.join(
                    f_file,
                    f"{MAJOR_VERSION}.project",
                )
                set_file_setting("last-project", f_file)
                return True
            else:
                return False
    except Exception as ex:
        LOG.exception(ex)
        QMessageBox.warning(a_parent, "Error", str(ex))

def open_project(a_parent=None):
    try:
        f_file, f_filter = QFileDialog.getOpenFileName(
            parent=a_parent,
            caption='Open Project',
            directory=DEFAULT_PROJECT_DIR,
            filter=PROJECT_FILE_TYPE,
            options=QFileDialog.DontUseNativeDialog,
        )
        if f_file is None:
            return False
        f_file_str = str(f_file)
        if not f_file_str:
            return False
        if not util.check_for_rw_perms(f_file):
            return False
        #global_open_project(f_file_str)
        set_file_setting("last-project", f_file_str)
        return True
    except Exception as ex:
        LOG.exception(ex)
        QMessageBox.warning(a_parent, "Error", str(ex))

