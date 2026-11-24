#include "rclcpp/rclcpp.hpp"                  // Framework ROS 2 C++
#include "std_msgs/msg/float32.hpp"           // Message pour recevoir l'erreur
#include "geometry_msgs/msg/twist.hpp"        // Message pour les commandes de vitesse
#include <cmath>                              // Pour std::abs
#include <algorithm>                          // Pour std::clamp
#include <gpiod.hpp>


class line_sensor_noeud : public rclcpp::Node {
public:
    double d1_{};
    double d2_{};
    double d3_{};
    double d4_{};
    double d5_{};

private:

    void gpio_init() {
        // Initialisation de la bibliothèque gpiod (remplacer x par  les numéros de ligne GPIO)
        gpiod::chip chip("gpiochip0"); // Remplacer "gpiochip0" par le nom de  puce GPIO
        gpiod::line line1 = chip.get_line(x); 
        gpiod::line line2 = chip.get_line(x); 
        gpiod::line line3 = chip.get_line(x);
        gpiod::line line4 = chip.get_line(x);  
        gpiod::line line5 = chip.get_line(x);

        // Configuration des lignes en entrée
        line1.request({"line_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});
        line2.request({"line_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});
        line3.request({"line_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});
        line4.request({"line_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});
        line5.request({"line_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});

        // Lecture des valeurs des lignes
        d1_ = line1.get_value();
        d2_ = line2.get_value();
        d3_ = line3.get_value();
        d4_ = line4.get_value();
        d5_ = line5.get_value();
        }
}


int main(int argc, char **argv){
    rclcpp::init(argc, argv);   
    auto node = std::make_shared<line_sensor_noeud>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}