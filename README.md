# noddyverse 

## With the closure of Cloustor, which stored the Noddyverse files, access to the Noddyverse is unfortunately not possible at the moment. I am investigating various solutions for at least accessing the raw models which should be available by the end of January. More news as it comes to hand. Mark Jessell Jan/2024



 [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/Loop3D/noddyverse/HEAD?filepath=noddyverse-remote-files-1M.ipynb)


C Code to generate random Noddy models, together with example notebook to visualise randomly selected models from 1M model suite:

       
The notebook randomly samples and displays models from the 1 Million model suite, with the possibility to filter to specific deformation event sequences. 
   
For each model the magnetic response, the gravity response, the top surface, the north facing vertical section (looking from the south) and the west-facing vertical section (looking from the west) of the 4 x 4 x 4 km cube are displayed: 
   
![Example output](images/example.png)
   
The models consist of 20 m cubic voxels (200x200x200 voxels per model), and the history file used to define each model is provided as a link for each model. Windows software to read this history file is available at the <a href="http://tectonique.net/noddy">Noddy Site</a>, and the source code for a command line version of Noddy, as well as this notebook, is available here. The 1M models themselves were stored at  Cloudstor as individual files and as 343 tar files, one per history sequence but this service has closed an an alternative solution is currently being sought</a>.
   



