import numpy as np
import json
import matplotlib.pyplot as plt
import pandas as pd
import uproot
import awkward as ak
import mpl_toolkits.mplot3d.art3d as art3d
from matplotlib.patches import Wedge
import matplotlib.animation as animation
import array
#import time
#from timeit import default_timer as timer

#/*************************************************************/
#/*              TPC Cluster Drift Animator                  */
#/*         Aditya Prasad Dash, Thomas Marshall              */
#/*      aditya55@physics.ucla.edu, rosstom@ucla.edu         */
#/*************************************************************/
#Code in python
#Input:
# root or json file containing TPC cluster positions
#Output:
# Animation of drifting of TPC clusters with user defined speed and option to save in .mp4 format

def TPC_surface(inner_radius,outer_radius, length_z):
    ngridpoints=30
    z = np.linspace(-length_z, length_z, ngridpoints)
    phi = np.linspace(0, 2*np.pi, ngridpoints)
    phi_grid, z_grid=np.meshgrid(phi, z)
    x_grid_inner = inner_radius*np.cos(phi_grid)
    y_grid_inner = inner_radius*np.sin(phi_grid)
    x_grid_outer = outer_radius*np.cos(phi_grid)
    y_grid_outer = outer_radius*np.sin(phi_grid)
    
    return x_grid_inner,y_grid_inner,x_grid_outer,y_grid_outer,z_grid

def TPC_endcap(inner_radius,outer_radius, length_z):
    ngridpoints=30
    radius = np.linspace(inner_radius, outer_radius, 5)
    phi = np.linspace(0, 2*np.pi, ngridpoints)
    phi_grid, r_grid=np.meshgrid(phi, radius)
    x_grid=[]
    y_grid=[]
    for r in radius:
        x_grid_radius = r*np.cos(phi_grid)
        y_grid_radius = r*np.sin(phi_grid)
        x_grid=np.append(x_grid,x_grid_radius)
        y_grid=np.append(y_grid,y_grid_radius)
    
    z_grid=x_grid
    return x_grid,y_grid,z_grid


def raddist_cluster(cluster_pos):
    radius=np.sqrt(cluster_pos[0]*cluster_pos[0]+cluster_pos[1]*cluster_pos[1])
    return radius
    
def theLoop(iteration,dataPoint,scatter):
    #effective_time=iteration-dataPoint[4]
    effective_time=iteration
    if(effective_time>=0):
        effective_z = 0
        if(dataPoint[2]>0):
            effective_z=dataPoint[2]+drift_speed_posz[2]*effective_time
        if(dataPoint[2]<0):
            effective_z=dataPoint[2]+drift_speed_negz[2]*effective_time
        if(abs(effective_z)<len_TPC+drift_speed_posz[2]):
            scatter._offsets3d = (dataPoint[0:1], dataPoint[1:2], [effective_z])
            color=['r','g','b','c','m','y']

            if(abs(effective_z)<len_TPC):
                #scatter.set_color(color[int(dataPoint[3]%6)])
                scatter.set_color('r')
                scatter.set_sizes([0.1])
            if(abs(effective_z)>=len_TPC):
                scatter.set_color('white')
                scatter.set(alpha=1.0)
                scatter.set_sizes([0.1])

        elif(abs(effective_z)<len_TPC+2*drift_speed_posz[2]):
                scatter._offsets3d = ([100], [-100], [100]) #to plot all points outside TPC at one point
                scatter.set_color('black')
                scatter.set_sizes([0.1])
    return 0
    
#Parallel processing
def animate_scatters(iteration, data,
    scatters,fig_text,time_scale,iteration_time,start_iteration):
    iteration=iteration+start_iteration
    if(iteration%1==0):
        print("iteration=")
        print(iteration)
    iter_array=[iteration]*len(data)
    time=(iteration)*iteration_time/time_scale*(10**3)
    theLoop_return = [theLoop(iteration,da,scat) for da,scat in zip(data,scatters)]
    fig_text.set_text(str(round(time,1))+"$\mu$s")

    return scatters,fig_text
    
#Normal Processing
#def animate_scatters(iteration, data, scatters,fig_text,time_scale,iteration_time,start_iteration):
#    print("iteration=")
#    print(iteration)
#    for i in range(data.shape[0]):
#        time=(iteration+start_iteration)*iteration_time/time_scale*(10**3)
#        effective_time=iteration-data[i,4]
#        if(effective_time>=0):
#            if(data[i,2]>0):
#                effective_z=data[i,2]+drift_speed_posz[2]*effective_time
#            if(data[i,2]<0):
#                effective_z=data[i,2]+drift_speed_negz[2]*effective_time
#            if(abs(effective_z)<len_TPC+drift_speed_posz[2]):
#                scatters[i]._offsets3d = (data[i,0:1], data[i,1:2], [effective_z])
#                color=['r','g','b','c','m','y']
#
#                if(abs(effective_z)<len_TPC):
#                    scatters[i].set_color(color[int(data[i,3]%6)])
#
#                if(abs(effective_z)>=len_TPC):
#                    scatters[i].set_color('white')
#                    scatters[i].set(alpha=1.0)
#
#            if(abs(effective_z)>=len_TPC+2*drift_speed_posz[2]):
#                    scatters[i]._offsets3d = ([100], [-100], [100]) #to plot all points outside TPC at one point
#                    scatters[i].set_color('black')
#                    scatters[i].set_sizes([0.01])
#        else:
#            scatters[i]._offsets3d = ([100], [-100], [100]) #clusters from event not yet taken place
#            scatters[i].set_color('black')
#
#    fig_text.set_text(str(round(time,2))+"$\mu$s")
#
#    return scatters,fig_text

def animate_clusters(data,animation_name="Animated_clusters_TPC.mp4",save=False,start_iteration=0,no_iterations=1):

    # Attaching 3D axis to the figure
    fig = plt.figure(figsize=[7.5,7.5],layout='constrained')
    ax = fig.add_subplot(111, projection='3d',facecolor='black',alpha=0.0)
    ax.grid(False)
    ax.margins(0.0)
    ax.xaxis.set_pane_color((1,1,1,0))
    ax.xaxis.line.set_color('w')
    ax.spines['top'].set_color('w')
    ax.spines['bottom'].set_color('w')
    ax.spines['left'].set_color('w')
    ax.spines['right'].set_color('w')
    
    ax.yaxis.set_pane_color((1,1,1,0))
    ax.yaxis.line.set_color('w')
    ax.zaxis.set_pane_color((1,1,1,0))
    ax.zaxis.line.set_color('w')
    
    #Drawing TPC
    endcap1=Wedge((0, 0), 80, 0, 360, color='blue',alpha=0.7)
    endcap2=Wedge((0, 0), 80, 0, 360, color='blue',alpha=0.7)
    endcap1.set_width(60)
    endcap2.set_width(60)
    
    ax.add_artist(endcap1)
    ax.add_artist(endcap2)
    
    art3d.pathpatch_2d_to_3d(endcap1, z=len_TPC, zdir="z")
    art3d.pathpatch_2d_to_3d(endcap2, z=-len_TPC, zdir="z")
    
    Xc_in,Yc_in,Xc_out,Yc_out,Zc = TPC_surface(20,80,len_TPC)
    ax.plot_surface(Xc_in, Yc_in, Zc, alpha=0.5)
    ax.plot_surface(Xc_out, Yc_out, Zc, alpha=0.5)
    
    # Setting the axes properties
    ax.set_xlim3d([-100, 100])
    ax.set_xlabel('X (cm)',color='white',fontsize=7)
    ax.xaxis.set_tick_params(colors='white',labelsize=7)
    
    ax.set_ylim3d([-100, 100])
    ax.set_ylabel('Y (cm)',color='white',fontsize=7)
    ax.yaxis.set_tick_params(colors='white',labelsize=7)

    ax.set_zlim3d([-120, 120])
    ax.set_zlabel('Z (cm)',color='white',fontsize=7)
    ax.zaxis.set_tick_params(colors='white',labelsize=7)

    ax.set_title('Clusters drifting in TPC') #(time scaled by $2*10^{5}$)')
    fig_text=ax.text(-100,80,100,  'time', size=10,color='w',alpha=0.9)
    fig_text_sPhenix=ax.text(-100,130,100,  'sPHENIX', size=10,fontweight='bold',style='italic',color='w',alpha=0.9)
    fig_text_TPC=ax.text(-60,134,70,  'Time Projection Chamber', size=10,color='w',alpha=0.9)
    fig_text_collisions=ax.text(-130,110,90,  'Diffused laser with TPC HV on and Laser 98% ALL ON Trigger modebit@2', size=10,color='w',alpha=0.9)
    fig_text_frame=ax.text(-130,95,90,  '2023-07-06, Run 11011', size=10,color='w',alpha=0.9)
    
    #fig_text_sPhenix=ax.text(-130,100,90,  'Au+Au, $\sqrt{s_{NN}}$ = 200 GeV', size=10,color='w',alpha=0.9)
    
    
    # Providing the starting angle for the view
    ax.view_init(10,70,0,'y')
    
    # Initializing scatters
    scatters = [ ax.scatter([100],[-100],[100]) for i in range(data.shape[0])]
    for i in range(data.shape[0]): scatters[i].set_color('black')
    print("Plot initialized")
    #start = timer()
    ani = animation.FuncAnimation(fig, animate_scatters, iterations, fargs=(data, scatters,fig_text,time_scale,iteration_time,start_iteration),
                                       interval=iteration_time, blit=False, repeat=False) #interval is in milliseconds and is the time between each frame
    if save:
        print("Saving animation as"+animation_name)
        ani.save(animation_name,writer='ffmpeg')
        print("Animation saved")
    #end = timer()
    #print("Time for process=")
    #print(end-start)
    plt.show()

def read_cluster_pos(inFile):
    if(inFile.lower().endswith('.json')):
        print("Reading data from json file")
        file=open(inFile)
        data_json=json.load(file)
        file.close()
        dict_json = data_json.items()
        numpy_array_json = np.array(list(dict_json))
        #print(numpy_array_json[2][1]['TRACKHITS'])
        mom_dict=numpy_array_json[2][1] #only works for this format the 3rd row is TRACKS and and 1 refers to INNERTRACKER
        #print(mom_dict)
        innertracker_arr=mom_dict['TRACKHITS'] #stores the tracks information as an array
        #print(innertracker_arr[0])
        #print(innertracker_arr)
        print("Reading clusters")
        #Getting the cluster positions of a track
        x_y_z_clusters=np.array([])
        #print(x_y_z_clusters)
        no_hits=len(innertracker_arr) #set no_tracks=10 for testing
        print(no_hits)
        #no_hits=1000
        for hit_no in range(no_hits):
            x_y_z_dict_track=innertracker_arr[hit_no].copy() #innertracker_arr[i] reads the ith track
            #print(list(x_y_z_dict_track.values()))
            x_y_z_clusters_track=np.array(list(x_y_z_dict_track.values())) #2D array the x,y,z positions corresponding to each cluster e.g. x_y_z[0][0] is the x coordinate of the 1st cluster
            x_y_z_clusters_track=x_y_z_clusters_track[:3]
            #print(x_y_z_clusters_track)
            #hit_no=0
            if(raddist_cluster(x_y_z_clusters_track)<30 or x_y_z_clusters_track[2]==0.0):
               continue
            else:
               if(hit_no==0):
                x_y_z_clusters=[np.copy(x_y_z_clusters_track)]
    
               else:
                #if(hit_no==1):
                #x_y_z_clusters=np.append(x_y_z_clusters,x_y_z_clusters_track,axis=0)
                #x_y_z_clusters=np.append(x_y_z_clusters,x_y_z_clusters_track)
                  x_y_z_clusters=np.vstack([x_y_z_clusters,x_y_z_clusters_track])
                #np.vstack([a,l])
        #print(x_y_z_clusters)
        return x_y_z_clusters

    if(inFile.lower().endswith('.root')):
        print("Reading data from root file")
        file = uproot.open(inFile)
        ntp_cluster_tree=file['hitTree']
        branches=ntp_cluster_tree.arrays(["x","y","z"])
        #branches=branches[~np.isnan(branches.gvt)]
        branches=branches[((branches.x)**2+(branches.y)**2)>900]
        #branches=branches[branches.layer>6]
        #branches=branches[branches.layer<55]
        #branches=branches[branches.event>=75]
        #branches=branches[branches.event<55]

        print("Reading clusters")
        x_y_z_clusters_run=np.array([])
        len_events=len(np.unique(branches.event))
        len_events=200
        event_times=[0]
        event_times=np.append(event_times,np.random.poisson(mean_eventtime*1000,len_events-1))
        event_times=np.cumsum(event_times,dtype=float)
        for cluster in range(len(branches)):
            #gvt_event=event_times[int(branches[cluster]['event'])]
            gvt_event=1
            gvt_event=gvt_event*(10**(-6))*time_scale #Time in milliseconds scaled for animation
            gvt_event=gvt_event/(iteration_time) #this is the time in terms of iterations
            x_y_z_clusters_track=np.array([[branches[cluster]['x'], branches[cluster]['y'], branches[cluster]['z'],branches[cluster]['event'],gvt_event]])
            if(cluster==0):
                x_y_z_clusters_run=np.copy(x_y_z_clusters_track)
            else:
                x_y_z_clusters_run=np.append(x_y_z_clusters_run,x_y_z_clusters_track,axis=0)
        return x_y_z_clusters_run
        
print("Generating data for animation")


# Main Program starts from here
np.random.seed(3)
#User defined values
time_scale=4.0*(10.0**5) #inverse of speed scale
collision_rate=4.0 #MHz #Set fig_text_sPhenix in line 188
mean_eventtime=1/collision_rate
print("Mean event time (microseconds)=")
print(mean_eventtime)
TPC_drift_speed=8.0*(10.0**3) #Actual TPC drift speed =8cm/microsecond=8*10^3cm/millisecond
iteration_time=100.0 #actual duration between frames in FuncAnimation
len_TPC=105.0

drift_speed_posz=np.array([0.0,0.0,TPC_drift_speed/time_scale*iteration_time,0.0,0.0])
drift_speed_negz=np.array([0.0,0.0,-TPC_drift_speed/time_scale*iteration_time,0.0,0.0])
print("Drift_speed_posz(cm/iteration)=")
print(drift_speed_posz)
print("Drift_speed_negz(cm/iteration)=")
print(drift_speed_negz)

#ANIMATION
#data=read_cluster_pos("Data_files/G4sPHENIX_g4svtx_eval.root")
data=read_cluster_pos("Data_files/TPCEventDisplay_11011.json")
print(data)
# Number of iterations
#no_events=np.max(data[:,3])+1
no_events=1
print("no_events=")
print(no_events)

iterations = int((no_events-1)*mean_eventtime*time_scale/(iteration_time*1000)+len_TPC/drift_speed_posz[2])+5
#iterations=10

print("number of iterations=")
print(iterations)
print("Animation starting!")

#Saving takes a long time so use Save=True only when necessary
#increase drift_speed_posz and drift_speed_negz if desired
animate_clusters(data,"Animated_clusters_TPC_beam.mp4",save=True,start_iteration=50,no_iterations=iterations)


#Merge mp4 files using ffmpeg -f concat -safe 0 -i filelist.txt -c copy mergedVideo.mp4
