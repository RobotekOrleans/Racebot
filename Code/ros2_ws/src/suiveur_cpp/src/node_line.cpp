#include "rclcpp/rclcpp.hpp"                  // Framework ROS 2 C++
#include "std_msgs/msg/float32.hpp"           // Message pour recevoir l'erreur
#include "geometry_msgs/msg/twist.hpp"        // Message pour les commandes de vitesse
#include <gpiod.hpp>


class line_sensor_noeud : public rclcpp::Node {
public:
    double d1_{};
    double d2_{};
    double d3_{};
    double d4_{};
    double d5_{};

    line_sensor_noeud() : Node("line_sensor_noeud") {
        gpio_init();
        
        publisher_cmd_vel_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        RCLCPP_INFO(this->get_logger(), "Nœud capteur de ligne initialisé !");
    }

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

        }
        

    void read_line_sensors() {
        // Lecture des valeurs des lignes
        d1_ = line1.get_value();
        d2_ = line2.get_value();
        d3_ = line3.get_value();
        d4_ = line4.get_value();
        d5_ = line5.get_value();

        //Logique de contrôle

        if(d1_ == 1 && d2_ == 1 && d3_ == 1 && d4_ == 1 && d5_ == 1) {
            // Tous les capteurs détectent la ligne
            publish_cmd_vel(0.0, 0.0); // Arrêt du robot  //////////////// A modifier pour un ralentissement à 0.5 m/s
        } 
        
        else if(d1_ == 0 && d2_ == 0 && d3_ == 0 && d4_ == 0 && d5_ == 0) {
            // Aucun capteur ne détecte la ligne
            publish_cmd_vel(0.2, 0.0); // Avancer tout droit
        } 

        else if(d3_ == 1) {
            // Ligne détectée au centre
            publish_cmd_vel(0.2, 0.0); // Avancer tout droit
        }
        
        //GAUCHE

        else if(d2_ == 1 && d3_ == 1) {
            // Ligne détectée à légerement à gauche
            publish_cmd_vel(0.2, -0.25); // Tourner légerement à gauche
        }

        else if(d2_ == 1) {
            // Ligne détectée à modérement à gauche
            publish_cmd_vel(0.2, -0.5); // Tourner modérement à gauche
        } 
        
        

        else if(d2_ == 1 && d1_ == 1) {
            // Ligne détectée à gauche
            publish_cmd_vel(0.2, -0.75); // Tourner  à gauche
        }


        else if(d1_ == 1) {
            // Ligne détectée fortement  à gauche
            publish_cmd_vel(0.2, -0.75); // Tourner fortement à gauche
        }

        //DROITE

        else if(d4_ == 1 && d3_ == 1) {
            // Ligne détectée à légerement à droite
            publish_cmd_vel(0.2, 0.25); // Tourner légerement à droite
        }

        else if(d4_ == 1) {
            // Ligne détectée à modérement à droite
            publish_cmd_vel(0.2, 0.5); // Tourner modérement à droite
        }

        else if(d4_ == 1 && d5_ == 1) {
            // Ligne détectée à droite
            publish_cmd_vel(0.2, 0.75); // Tourner  à droite
        }


        else if(d5_ == 1) {
            // Ligne détectée fortement  à droite
            publish_cmd_vel(0.2, 0.75); // Tourner fortement à droite
       
        

    }
        

    }
    
    void publish_cmd_vel(double linear_x, double angular_z) {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = linear_x;
        message.angular.z = angular_z;
        publisher_cmd_vel_->publish(message);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_cmd_vel_;
}


    
    void publish_cmd_vel(double linear_x, double angular_z) {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = linear_x;
        message.angular.z = angular_z;
        publisher_cmd_vel_->publish(message);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_cmd_vel_;



int main(int argc, char **argv){
    rclcpp::init(argc, argv);   
    auto node = std::make_shared<line_sensor_noeud>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}