# fitantid

Several fitness devices can store activity data in .fit format.

This is a command line utility which can search .fit files for device info messages, and extract ANT IDs and other information.  Note that it actually finds the serial number, which is often used to form the ANT ID.

## Building from source
This project requires gcc and has been tested on Windows, OS X and Ubuntu.  Mingw is required for the Windows buid.  It should work on any other Unix as well.

This project requires the FIT SDK, available from http://www.thisisant.com/resources/fit.  Due to license restrictions, you will need to agree to the licence for the FIT SDK, and acquire the source on your own accord.

If you have agreed to download the FIT SDK, copy all files from the c/ subfolder in the SDK into a folder resource/fit in your copy of the fitantid project.
- Modify the file named fit_config.h so that the line #define FIT_USE_STDINT_H is not commented out.
- Remove the file fit_product.c
- run make at the command line

## Running the command
- Ensure that the utility is in your path, or that your command prompt is in the directory holding the executable.
- From your command prompt or terminal window, simply type fitantid <.fit file>

