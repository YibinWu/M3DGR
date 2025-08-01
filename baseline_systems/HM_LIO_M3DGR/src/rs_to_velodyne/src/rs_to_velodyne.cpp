//#include "utility.h"
#include <ros/ros.h>

#include <sensor_msgs/PointCloud2.h>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

std::string output_type;

static int RING_ID_MAP_RUBY[] = {
        3, 66, 33, 96, 11, 74, 41, 104, 19, 82, 49, 112, 27, 90, 57, 120,
        35, 98, 1, 64, 43, 106, 9, 72, 51, 114, 17, 80, 59, 122, 25, 88,
        67, 34, 97, 0, 75, 42, 105, 8, 83, 50, 113, 16, 91, 58, 121, 24,
        99, 2, 65, 32, 107, 10, 73, 40, 115, 18, 81, 48, 123, 26, 89, 56,
        7, 70, 37, 100, 15, 78, 45, 108, 23, 86, 53, 116, 31, 94, 61, 124,
        39, 102, 5, 68, 47, 110, 13, 76, 55, 118, 21, 84, 63, 126, 29, 92,
        71, 38, 101, 4, 79, 46, 109, 12, 87, 54, 117, 20, 95, 62, 125, 28,
        103, 6, 69, 36, 111, 14, 77, 44, 119, 22, 85, 52, 127, 30, 93, 60
};
static int RING_ID_MAP_16[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8
};

// rslidar和velodyne的格式有微小的区别
// rslidar的点云格式
struct RsPointXYZIRT {
    PCL_ADD_POINT4D;
    uint8_t intensity;
    uint16_t ring = 0;
    double timestamp = 0;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;
POINT_CLOUD_REGISTER_POINT_STRUCT(RsPointXYZIRT,
                                  (float, x, x)(float, y, y)(float, z, z)(uint8_t, intensity, intensity)
                                          (uint16_t, ring, ring)(double, timestamp, timestamp))

// velodyne的点云格式
struct VelodynePointXYZIRT {
    PCL_ADD_POINT4D

    PCL_ADD_INTENSITY;
    uint16_t ring;
    float time;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT (VelodynePointXYZIRT,
                                   (float, x, x)(float, y, y)(float, z, z)(float, intensity, intensity)
                                           (uint16_t, ring, ring)(float, time, time)
)

struct VelodynePointXYZIR {
    PCL_ADD_POINT4D

    PCL_ADD_INTENSITY;
    uint16_t ring;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT (VelodynePointXYZIR,
                                   (float, x, x)(float, y, y)
                                           (float, z, z)(float, intensity, intensity)
                                           (uint16_t, ring, ring)
)

ros::Subscriber subRobosensePC;
ros::Publisher pubRobosensePC;

template<typename T>
bool has_nan(T point) {

    // remove nan point, or the feature assocaion will crash, the surf point will containing nan points
    // pcl remove nan not work normally
    // ROS_ERROR("Containing nan point!");
    if (pcl_isnan(point.x) || pcl_isnan(point.y) || pcl_isnan(point.z)) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
void publish_points(T &new_pc, const sensor_msgs::PointCloud2 &old_msg) {
    // pc properties
    new_pc->is_dense = true;

    // publish
    sensor_msgs::PointCloud2 pc_new_msg;
    pcl::toROSMsg(*new_pc, pc_new_msg);
    pc_new_msg.header = old_msg.header;
    pc_new_msg.header.frame_id = "velodyne";
    pubRobosensePC.publish(pc_new_msg);
}

void rsHandler_XYZI(sensor_msgs::PointCloud2 pc_msg) {
    pcl::PointCloud<pcl::PointXYZI>::Ptr pc(new pcl::PointCloud<pcl::PointXYZI>());
    pcl::PointCloud<VelodynePointXYZIR>::Ptr pc_new(new pcl::PointCloud<VelodynePointXYZIR>());
    pcl::fromROSMsg(pc_msg, *pc);

    // to new pointcloud
    for (int point_id = 0; point_id < pc->points.size(); ++point_id) {
        if (has_nan(pc->points[point_id]))
            continue;

        VelodynePointXYZIR new_point;
        new_point.x = pc->points[point_id].x;
        new_point.y = pc->points[point_id].y;
        new_point.z = pc->points[point_id].z;
        new_point.intensity = pc->points[point_id].intensity;
        // remap ring id
        if (pc->height == 16) {
            new_point.ring = RING_ID_MAP_16[point_id / pc->width];
        } else if (pc->height == 128) {
            new_point.ring = RING_ID_MAP_RUBY[point_id % pc->height];
        }
        pc_new->points.push_back(new_point);
    }

    publish_points(pc_new, pc_msg);
}

void rsHandler_XYZI_M2DGR_PLUS(sensor_msgs::PointCloud2 pc_msg) {
    pcl::PointCloud<pcl::PointXYZI>::Ptr pc(new pcl::PointCloud<pcl::PointXYZI>());
    pcl::PointCloud<VelodynePointXYZIRT>::Ptr pc_new(new pcl::PointCloud<VelodynePointXYZIRT>());
    pcl::fromROSMsg(pc_msg, *pc);

    //add time
    bool halfPassed = false;    
        float startOrientation = -atan2(pc->points[0].y, pc->points[0].x);
        float endOrientation   = -atan2(pc->points[pc->points.size() - 1].y,
                                                     pc->points[pc->points.size() - 1].x) + 2 * M_PI;
            if (endOrientation - startOrientation > 3 * M_PI) {
            endOrientation -= 2 * M_PI;
            } else if (endOrientation - startOrientation < M_PI)
            endOrientation += 2 * M_PI;
            
        float orientationDiff = endOrientation - startOrientation;
    
    // to new pointcloud
    for (int point_id = 0; point_id < pc->points.size(); ++point_id) {
        if (has_nan(pc->points[point_id]))
            continue;

        VelodynePointXYZIRT new_point;
        new_point.x = pc->points[point_id].x;
        new_point.y = pc->points[point_id].y;
        new_point.z = pc->points[point_id].z;
        new_point.intensity = pc->points[point_id].intensity;
        // // remap ring id
        // if (pc->height == 16) {
        //     new_point.ring = RING_ID_MAP_16[point_id / pc->width];
        // } else if (pc->height == 128) {
        //     new_point.ring = RING_ID_MAP_RUBY[point_id % pc->height];
        // }
        
        float ori = -atan2(pc->points[point_id].y, pc->points[point_id].x);
            if (!halfPassed) {
                if (ori < startOrientation - M_PI / 2)
                    ori += 2 * M_PI;
                else if (ori > startOrientation + M_PI * 3 / 2)
                    ori -= 2 * M_PI;

                if (ori - startOrientation > M_PI)
                    halfPassed = true;
            } else {
                ori += 2 * M_PI;

                if (ori < endOrientation - M_PI * 3 / 2)
                    ori += 2 * M_PI;
                else if (ori > endOrientation + M_PI / 2)
                    ori -= 2 * M_PI;
            }
            float relTime = (ori - startOrientation) / orientationDiff;
        new_point.time = relTime;
        
        //add ring
        float verticalAngle = atan2(pc->points[point_id].z, sqrt(pc->points[point_id].x * pc->points[point_id].x + pc->points[point_id].y * pc->points[point_id].y)) * 180 / M_PI;
        new_point.ring = (verticalAngle + 30.1)/2.5;

        // 确保 ring 值在 0 到 16 之间
        if (new_point.ring < 0) 
        {
          std::cout << new_point.ring << std::endl;
          new_point.ring = 0;
        } 
        else if (new_point.ring > 15) {
          //std::cout << new_point.ring << std::endl;
          new_point.ring = 15;
        }

        pc_new->points.push_back(new_point);
        // ROS_INFO("Point %d: x=%f, y=%f, z=%f, intensity=%f, ring=%d, time=%f",
        //     point_id, new_point.x, new_point.y, new_point.z, new_point.intensity, new_point.ring, new_point.time);
    }

    publish_points(pc_new, pc_msg);
}




template<typename T_in_p, typename T_out_p>
void handle_pc_msg(const typename pcl::PointCloud<T_in_p>::Ptr &pc_in,
                   const typename pcl::PointCloud<T_out_p>::Ptr &pc_out) {

    // to new pointcloud
    for (int point_id = 0; point_id < pc_in->points.size(); ++point_id) {
        if (has_nan(pc_in->points[point_id]))
            continue;
        T_out_p new_point;
//        std::copy(pc->points[point_id].data, pc->points[point_id].data + 4, new_point.data);
        new_point.x = pc_in->points[point_id].x;
        new_point.y = pc_in->points[point_id].y;
        new_point.z = pc_in->points[point_id].z;
        new_point.intensity = pc_in->points[point_id].intensity;
//        new_point.ring = pc->points[point_id].ring;
//        // 计算相对于第一个点的相对时间
//        new_point.time = float(pc->points[point_id].timestamp - pc->points[0].timestamp);
        pc_out->points.push_back(new_point);
    }
}

template<typename T_in_p, typename T_out_p>
void add_ring(const typename pcl::PointCloud<T_in_p>::Ptr &pc_in,
              const typename pcl::PointCloud<T_out_p>::Ptr &pc_out) {
    // to new pointcloud
    int valid_point_id = 0;
    for (int point_id = 0; point_id < pc_in->points.size(); ++point_id) {
        if (has_nan(pc_in->points[point_id]))
            continue;
        // 跳过nan点
        pc_out->points[valid_point_id++].ring = pc_in->points[point_id].ring;
    }
}

template<typename T_in_p, typename T_out_p>
void add_time(const typename pcl::PointCloud<T_in_p>::Ptr &pc_in,
              const typename pcl::PointCloud<T_out_p>::Ptr &pc_out) {
    // to new pointcloud
    int valid_point_id = 0;
    for (int point_id = 0; point_id < pc_in->points.size(); ++point_id) {
        if (has_nan(pc_in->points[point_id]))
            continue;
        // 跳过nan点
        pc_out->points[valid_point_id++].time = float(pc_in->points[point_id].timestamp - pc_in->points[0].timestamp);
    }
}

void rsHandler_XYZIRT(const sensor_msgs::PointCloud2 &pc_msg) {
    pcl::PointCloud<RsPointXYZIRT>::Ptr pc_in(new pcl::PointCloud<RsPointXYZIRT>());
    pcl::fromROSMsg(pc_msg, *pc_in);

    if (output_type == "XYZIRT") {
        std::cout << "output type: XYZIRT" << std::endl;
        pcl::PointCloud<VelodynePointXYZIRT>::Ptr pc_out(new pcl::PointCloud<VelodynePointXYZIRT>());
        handle_pc_msg<RsPointXYZIRT, VelodynePointXYZIRT>(pc_in, pc_out);
        add_ring<RsPointXYZIRT, VelodynePointXYZIRT>(pc_in, pc_out);
        add_time<RsPointXYZIRT, VelodynePointXYZIRT>(pc_in, pc_out);
        publish_points(pc_out, pc_msg);
    } else if (output_type == "XYZIR") {
        pcl::PointCloud<VelodynePointXYZIR>::Ptr pc_out(new pcl::PointCloud<VelodynePointXYZIR>());
        handle_pc_msg<RsPointXYZIRT, VelodynePointXYZIR>(pc_in, pc_out);
        add_ring<RsPointXYZIRT, VelodynePointXYZIR>(pc_in, pc_out);
        publish_points(pc_out, pc_msg);
    } else if (output_type == "XYZI") {
        pcl::PointCloud<pcl::PointXYZI>::Ptr pc_out(new pcl::PointCloud<pcl::PointXYZI>());
        handle_pc_msg<RsPointXYZIRT, pcl::PointXYZI>(pc_in, pc_out);
        publish_points(pc_out, pc_msg);
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "rs_converter");
    ros::NodeHandle nh;
    if (argc < 3) {
        ROS_ERROR(
                "Please specify input pointcloud type( XYZI or XYZIRT) and output pointcloud type(XYZI, XYZIR, XYZIRT)!!!");
        exit(1);
    } else {
        // 输出点云类型
        output_type = argv[2];

        if (std::strcmp("XYZI", argv[1]) == 0) {
            subRobosensePC = nh.subscribe("/rslidar_points", 1, rsHandler_XYZI);
        } else if (std::strcmp("XYZIRT", argv[1]) == 0) {
            subRobosensePC = nh.subscribe("/rslidar_points", 1, rsHandler_XYZIRT);
        } else if (std::strcmp("m2dgrplus", argv[1]) == 0) {
            subRobosensePC = nh.subscribe("/rslidar_points", 1, rsHandler_XYZI_M2DGR_PLUS);
        } else {
            ROS_ERROR(argv[1]);
            ROS_ERROR("Unsupported input pointcloud type. Currently only support XYZI and XYZIRT.");
            exit(1);
        }
    }
    pubRobosensePC = nh.advertise<sensor_msgs::PointCloud2>("/velodyne_points", 1);

    ROS_INFO("Listening to /rslidar_points ......");
    ros::spin();
    return 0;
}
