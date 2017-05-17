from gi.repository import Nautilus, GObject
import signal
import os
from subprocess import check_output,CalledProcessError

class ColumnExtension(GObject.GObject, Nautilus.MenuProvider):

    global pid;

    def __init__(self):
        pass

    def get_pid(self,name):

	global pid

	try:
    	    pid=int(check_output(["pidof",name]))
	except CalledProcessError as e:
            pid=0;
	return pid;

    def menu_activate_cb(self, menu, files):

	# path should be read from config
	out_file = open("/tmp/selected_files", "w")

	# write number of files selected
	out_file.write(str(len(files)) + "\n")

	#write the path of the selected files
        for f in files:
            out_file.write(f.get_uri() + "\n")

	out_file.close();

        os.kill(pid, signal.SIGUSR1);

    def get_file_items(self, window, files):

        # If we have files selected, we will add menu entries

	pid = self.get_pid("projectPDS")

	if (pid == 0):
	    return;

        items = [];

        item = Nautilus.MenuItem(
            name="SimpleMenuExtension::Show_File_Name",
            label="Share",
            tip="Share"
        )
        item.connect('activate', self.menu_activate_cb, files)
        items.append(item)

        return items
