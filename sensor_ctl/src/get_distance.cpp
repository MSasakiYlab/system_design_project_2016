/*!
  \example get_distance.cpp Obtains distance data
  \author Satofumi KAMIMURA

  $Id: get_distance.cpp,v c5747add6615 2015/05/07 03:18:34 alexandr $
*/
#include "ros/ros.h"

// #include "create_ctl/State.h"
#include "sensor_ctl/State.h"


#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include <iostream>

using namespace qrk;
using namespace std;


namespace
{
    short int print_data(const Urg_driver& urg,
                    const vector<long>& data, long time_stamp)
    {
#if 0
    // Shows only the front step
        int front_index = urg.step2index(0);
        cout << data[front_index] << " [mm], ("
             << time_stamp << " [msec])" << endl;

#else
    // Prints the X-Y coordinates for all the measurement points
        int space_l=0;
        int space_rad=0;
        long min_distance = urg.min_distance();
        long max_distance = urg.max_distance();
        size_t data_n = data.size();
        for (size_t i = 0; i < data_n; ++i) {
            long l = data[i];
            /*
            if ((l <= min_distance) || (l >= max_distance)) {
                continue;
            }
            */

            double radian = urg.index2rad(i);

            if(i==0 || i==125 || i==250 || i==375 || i==(data_n-1)){
        cout << "(l=" << l << ", rad=" << radian*(360/(2*3.141592)) << ")" << endl;
        if(space_l<l){
            space_l=l;
            space_rad=i;
        }
        }

    }
    switch(space_rad){
    case 0:
        space_rad = 1;
        break;
    case 125:
        space_rad = 2;
        break;
    case 250:
        space_rad = 3;
        break;
    case 375:
        space_rad = 4;
        break;
    default:
        space_rad = 5;
        break;
    }
    cout << "space_rad:" << space_rad << endl;
    cout << endl;
    return space_rad;
    }


#endif
    
}


int main(int argc, char *argv[])
{
    
    ros::init(argc, argv, "sensor_logger");
    ros::NodeHandle n;

    ros::Publisher pub = n.advertise<sensor_ctl::State>("input_data", 100);
    sensor_ctl::State msg;

    // msg.state = ?
    // pub.publish(msg)

    ros::Rate loop_rate(1);

    Connection_information information(argc, argv);

    // Connects to the sensor
    Urg_driver urg;
    if (!urg.open(information.device_or_ip_name(),
                  information.baudrate_or_port_number(),
                  information.connection_type())) {
        cout << "Urg_driver::open(): "
             << information.device_or_ip_name() << ": " << urg.what() << endl;
        return 1;
    }

    // Gets measurement data
#if 1
    // Case where the measurement range (start/end steps) is defined
    urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);
#endif
    // enum { Capture_times = 1 };
    urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
    // for (int i = 0; i < Capture_times; ++i) {
        vector<long> data;
        long time_stamp = 0;
    while (ros::ok()){
            if (!urg.get_distance(data, &time_stamp)) {
                cout << "Urg_driver::get_distance(): " << urg.what() << endl;
                return 1;
            }
            msg.state = print_data(urg, data, time_stamp);
            pub.publish(msg);
            // cout << "published " << msg.state << endl;
            printf("published %d\n",(int)msg.state);
            loop_rate.sleep();
        }
    // } 

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
