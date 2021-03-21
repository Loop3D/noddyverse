import numpy as np
import matplotlib
from pandas import DataFrame
from matplotlib.pyplot import imshow
from matplotlib import pyplot as plt
from random import seed
import random
import time
import gzip
from urllib.request import urlopen
import pandas as pd
import io
import requests

#%matplotlib inline

def rand_cmap(nlabels, type='bright', first_color_black=True, last_color_black=False, verbose=True):
    """
    Creates a random colormap to be used together with matplotlib. Useful for segmentation tasks
    :param nlabels: Number of labels (size of colormap)
    :param type: 'bright' for strong colors, 'soft' for pastel colors
    :param first_color_black: Option to use first color as black, True or False
    :param last_color_black: Option to use last color as black, True or False
    :param verbose: Prints the number of labels and shows the colormap. True or False
    :return: colormap for matplotlib
    Thanks to https://gist.github.com/delestro/54d5a34676a8cef7477e
    
    """
    from matplotlib.colors import LinearSegmentedColormap
    import colorsys
    import numpy as np

    np.random.seed(seed=0)
    if type not in ('bright', 'soft'):
        print ('Please choose "bright" or "soft" for type')
        return

    if verbose:
        print('Number of labels: ' + str(nlabels))

    # Generate color map for bright colors, based on hsv
    if type == 'bright':
        randHSVcolors = [(np.random.uniform(low=0.0, high=1),
                          np.random.uniform(low=0.2, high=1),
                          np.random.uniform(low=0.9, high=1)) for i in range(nlabels)]

        # Convert HSV list to RGB
        randRGBcolors = []
        for HSVcolor in randHSVcolors:
            randRGBcolors.append(colorsys.hsv_to_rgb(HSVcolor[0], HSVcolor[1], HSVcolor[2]))

        if first_color_black:
            randRGBcolors[0] = [0, 0, 0]

        if last_color_black:
            randRGBcolors[-1] = [0, 0, 0]

        random_colormap = LinearSegmentedColormap.from_list('new_map', randRGBcolors, N=nlabels)

    # Generate soft pastel colors, by limiting the RGB spectrum
    if type == 'soft':
        low = 0.6
        high = 0.95
        randRGBcolors = [(np.random.uniform(low=low, high=high),
                          np.random.uniform(low=low, high=high),
                          np.random.uniform(low=low, high=high)) for i in range(nlabels)]

        if first_color_black:
            randRGBcolors[0] = [0, 0, 0]

        if last_color_black:
            randRGBcolors[-1] = [0, 0, 0]
        random_colormap = LinearSegmentedColormap.from_list('new_map', randRGBcolors, N=nlabels)

    # Display colorbar
    if verbose:
        from matplotlib import colors, colorbar
        from matplotlib import pyplot as plt
        fig, ax = plt.subplots(1, 1, figsize=(15, 0.5))

        bounds = np.linspace(0, nlabels, nlabels + 1)
        norm = colors.BoundaryNorm(bounds, nlabels)

        cb = colorbar.ColorbarBase(ax, cmap=random_colormap, norm=norm, spacing='proportional', ticks=None,
                                   boundaries=bounds, format='%1i', orientation=u'horizontal')

    return random_colormap

# download file, ungzip and stuff into numpy array
def get_gz_array(url,skiprows):
    my_gzip_stream = urlopen(url)
    my_stream = gzip.open(my_gzip_stream, 'r')
    return(np.loadtxt(my_stream,skiprows=skiprows))

def display_models(his_filter,display_number):
    cmap = rand_cmap(100, type='bright', first_color_black=False, last_color_black=False, verbose=False)
    models=pd.read_csv('model_list/models.csv')

    #display(models)
    models2=models[models['event03'].str.contains(his_filter[0]) & models['event04'].str.contains(his_filter[1]) & models['event05'].str.contains(his_filter[2])] 
    models2=models2.reset_index(drop=True)
    #display(models2)
    model_number2=len(models2)
    if(len(models2)):
        print("sampling from",len(models2),"models matching filter",his_filter)
    else:
        print("no models found with filter", his_filter, "check list syntax and spelling of events")
        return()
    #seed random number generator
    now = time.time()
    seed(int(now))

    url='https://cloudstor.aarnet.edu.au/plus/s/8ZT6tjOvoLWmLPx/download?path=%2f'
    used=[]
    z=0
    fail=0
    while z < display_number and fail < 1000:
        ran =random.randint(0,model_number2-1) 

        if(ran in used):
            continue
        else:
            used.append(ran)
        file_split=models2.iloc[ran]['root'].split('/')  
        tail=models2.iloc[ran]['event03']+'_'+models2.iloc[ran]['event04']+'_'+models2.iloc[ran]['event05']+'&files='
        root=url+tail+file_split[2]
        path=root+'.mag.gz'

        try:
            mag=get_gz_array(path,8)
        except:
            fail=fail+1
            continue
        path=root+'.grv.gz'
        grv=get_gz_array(path,skiprows=8)

        path=root+'.g12.gz'
        mod=get_gz_array(path,skiprows=0)

        mod2=mod.reshape((200,200,200))
        mod2=np.transpose(mod2,(0,2,1))
        mod2.shape
        vmin=np.amin(mod2)
        vmax=np.amax(mod2)
        show=True    
        print(ran,file_split[2],'STRATIGRAPHY TILT',models2.iloc[ran]['event_all'])

        if(show):
            fig, ax = plt.subplots(1,5,figsize=(13,13))
            ax[0].axis('off')
            ax[1].axis('off')
            ax[2].axis('off')
            ax[3].axis('off')
            ax[4].axis('off')
            ax[0].title.set_text('Mag' )
            ax[1].title.set_text('Grav')
            ax[2].title.set_text('Map')
            ax[3].title.set_text('E    (N)    W')
            ax[4].title.set_text('N    (W)    S')
            ax[0].imshow(mag,cmap='rainbow')
            ax[1].imshow(grv,cmap='rainbow')
            ax[2].imshow(mod2[0,:,:],cmap=cmap, interpolation="nearest",vmin=vmin,vmax=vmax)
            ax[3].imshow(mod2[:,0,:],cmap=cmap, interpolation="nearest",vmin=vmin,vmax=vmax)
            ax[4].imshow(mod2[:,:,0],cmap=cmap, interpolation="nearest",vmin=vmin,vmax=vmax)
            print("Download .his file:",root+".his.gz")
            plt.show()
        else:
            matplotlib.image.imsave('grv/model_'+models2.iloc[ran]['root']+'_grv.png', mag,cmap='rainbow')
            matplotlib.image.imsave('mag/model_'+models2.iloc[ran]['root']+'_mag.png', grv,cmap='rainbow')
            matplotlib.image.imsave('map/model_'+models2.iloc[ran]['root']+'_map.png', mod2[0,:,:],cmap=cmap,vmin=vmin,vmax=vmax)
            matplotlib.image.imsave('secWE/model_'+models2.iloc[ran]['root']+'_secWE.png', mod2[:,0,:],cmap=cmap,vmin=vmin,vmax=vmax)
            matplotlib.image.imsave('secNS/model_'+models2.iloc[ran]['root']+'_secNS.png', mod2[:,:,0],cmap=cmap,vmin=vmin,vmax=vmax)
        z=z+1