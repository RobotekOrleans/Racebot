#include "rclcpp/rclcpp.hpp"                  // Framework ROS 2 C++
#include "std_msgs/msg/float32.hpp"           // Message pour recevoir l'erreur
#include "geometry_msgs/msg/twist.hpp"        // Message pour les commandes de vitesse
#include <gpiod.hpp>


class obstacle_sensor_noeud : public rclcpp::Node {
public:
    obstacle_sensor_noeud() : Node("obstacle_sensor_noeud") {}

private:
    void gpio_init() {
        // Initialisation de la bibliothèque gpiod
        gpiod::chip chip("gpiochip0");
        gpiod::line line = chip.get_line(18); // Remplacer 18 par le numéro de ligne GPIO

        // Configuration de la ligne en entrée
        line.request({"obstacle_sensor_noeud", gpiod::line_request::DIRECTION_INPUT});
    }
};


int main(){

    return 0;
}