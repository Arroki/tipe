Once downloaded, use **./build.sh [file type] [file name]** with

- [file type] : either "-pts" or "-epi", depending on whether you want to pass a file with the points or with the data of the cirlces (the file extension will have to match)

- "-pts" files contain one line with the number of points in the file, followed by that number of lines in the form **x coordinates, y coordinates**

- "-epi" files contain one line with the number of other lines in the file, followed by that number of lines in the form **radius, rotating speed, starting angle**

After executing the build command and having your drawing being drawn, you can : 

- Press *left arrow* or *right arrow* to slow down or accelerate the drawing speed (going too fast will result in loss of accuracy)

- Press *+* (or *up arrow*) and *-* (or *down arrow*) to zoom and de-zoom the drawing

- Press *escape* or *ctrl + c* to close the window

- Press *space* to pause the drawing

- Press *r* to erase the drawing

- Press *p* to show the input points (only if you entered a ".pts" file)

- Press *e* to hide the drawing

- Press *l* to show the circles

- Press *u* to hide the pendulum-alike figures
