# M3DGR
M3DGR: A Multi-sensor, Multi-scenario and Massive-baseline SLAM Dataset for Ground Robots


## 🎯 Notice
This paper is under review now. Full datasets and codes will be released upon paper acceptance.
## 🔥 SLAM benchmark
- a list of tested cutting-edge methods on M3DGR dataset:

  - ① [Tartanvo: A generalizable learning-based vo](https://arxiv.org/abs/2011.00359) from PMLR2021
  - ② [Orb-slam2: An open-source slam system for monocular, stereo, and rgb-d cameras](https://arxiv.org/abs/1610.06475) from T-RO2017
  - ③ [Orb-slam3: An accurate open-source library for visual, visual–inertial, and multimap slam](https://arxiv.org/abs/2007.11898) from T-RO2021
  - ④ [DM-VIO: Delayed marginalization visual-inertial odometry](https://arxiv.org/abs/2201.04114) from RA-L2022
  - ⑤ [Vins-mono: A robust and versatile monocular visual-inertial state estimator](https://arxiv.org/abs/1708.03852) from T-RO2018
  - ⑥ [VINS-RGBD: RGBD-inertial trajectory estimation and mapping for ground robots](https://www.mdpi.com/1424-8220/19/10/2251) from Sensors2019
  - ⑦ [GVINS: Tightly coupled GNSS–visual–inertial fusion for smooth and consistent state estimation](https://arxiv.org/abs/2103.07899) from T-RO2022
  - ⑧ VIW-Fusion: visual-inertial-wheel fusion odometry from [open repository](https://github.com/TouchDeeper/VIW-Fusion)2021
  - ⑨ VINS-GPS-Wheel: Visual-Inertial Odometry Coupled with Wheel Encoder and GNSS from [open repository](https://github.com/Wallong/VINS-GPS-Wheel)2021
  - ⑩ [Ground-fusion: A low-cost ground slam system robust to corner cases](https://arxiv.org/abs/2402.14308) from ICRA2024


## SENSOR SETUP
### Acquisition Platform


### Sensor parameters

All the sensors and track devices and their most important parameters are listed as below:

* **LIDAR** Velodyne VLP-32C, 360 Horizontal Field of View (FOV),-30 to +10 vertical FOV,10Hz,Max Range 200 m,Range Resolution 3 cm, Horizontal Angular Resolution 0.2°.  

* **RGB Camera** FLIR Pointgrey CM3-U3-13Y3C-CS,fish-eye lens,1280*1024,190 HFOV,190 V-FOV, 15 Hz  
* **GNSS** Ublox M8T, GPS/BeiDou, 1Hz  
* **Infrared Camera**,PLUG 617,640*512,90.2 H-FOV,70.6 V-FOV,25Hz;  
* **V-I Sensor**,Realsense d435i,RGB/Depth 640*480,69H-FOV,42.5V-FOV,15Hz;IMU 6-axix, 200Hz  
* **Event Camera** Inivation DVXplorer, 640*480,15Hz;  
* **IMU**,Handsfree A9,9-axis,150Hz;  
* **GNSS-IMU** Xsens Mti 680G. GNSS-RTK,localization precision 2cm,100Hz;IMU 9-axis,100 Hz;  
* **Laser Scanner** Leica MS60, localization 1mm+1.5ppm  
* **Motion-capture System** Vicon Vero 2.2, localization accuracy 1mm, 50 Hz;

The rostopics of our rosbag sequences are listed as follows:

* LIDAR: `/velodyne_points` 

* RGB Camera: 
`/camera/left/image_raw/compressed `,  
`/camera/right/image_raw/compressed `,  
`/camera/third/image_raw/compressed `,  
`/camera/fourth/image_raw/compressed `,  
`/camera/fifth/image_raw/compressed `,  
`/camera/sixth/image_raw/compressed `,  
`/camera/head/image_raw/compressed `  
* GNSS Ublox M8T:  
`/ublox/aidalm `,  
`/ublox/aideph `,  
`/ublox/fix `,  
`/ublox/fix_velocity `,  
`/ublox/monhw `,  
`/ublox/navclock `,  
`/ublox/navpvt `,  
`/ublox/navsat `,  
`/ublox/navstatus `,  
`/ublox/rxmraw `  


* Infrared Camera:`/thermal_image_raw ` 
* V-I Sensor:  
`/camera/color/image_raw/compressed `,  
`/camera/imu`
* Event Camera:  
`/dvs/events`,  
`/dvs_rendering/compressed`
* IMU: `/handsfree/imu `
 

## DATASET SEQUENCES

### Visual Challenge


### Wheel Slippage


### GNSS Denied



### LiDAR Degeneration





