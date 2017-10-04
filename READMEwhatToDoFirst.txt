In your home terminal, type:
1. sudo apt-get install gtk-doc-tools
2. sudo apt-get install python-dev
3. sudo apt-get install gnome-common
4. sudo apt-get install python-gtk2-dev python-gobject-dev python-gnome2-dev libnautilus-extension-dev
5. sudo apt-get install python-nautilus

Then:
6. cd /usr/share/nautilus-python/extensions
7. sudo cp /*path of the file->*/nautilus-python-script.py .
8. sudo chmod +x nautilus-python-script.py
9. nautilus -q (don't worry about failures)
10. nautilus --no-desktop

Close the terminal if necessary.
To be sure, reboot the system.

Then your application will do the rest.

HAVE FUN!!
