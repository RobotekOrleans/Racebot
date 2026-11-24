#include "rclcpp/rclcpp.hpp"                  // Framework ROS 2 C++
#include "std_msgs/msg/float32.hpp"           // Message pour recevoir l'erreur
#include "geometry_msgs/msg/twist.hpp"        // Message pour les commandes de vitesse
#include <cmath>                              // Pour std::abs
#include <algorithm>                          // Pour std::clamp

class Moteur_pid_noeud : public rclcpp::Node {
public:
    Moteur_pid_noeud() : Node("node_pid") {
        // Initialisation du temps de référence
        last_time_ = this->now();

        sub_erreur_ligne_ = this->create_subscription<std_msgs::msg::Float32>("/erreur_ligne", 10, 
        std::bind(&Moteur_pid_noeud::error_correction, this, std::placeholders::_1)
        );

        pub_cmd_vel_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        RCLCPP_INFO(this->get_logger(), "Nœud PID C++ initialisé ");
        }

private:
    //  VARIABLES
    /* GAINS PID */
    double kp_{0.005};
    double ki_{0.0001};
    double kd_{0.001};

    /* VARIABLES D'ÉTAT PID */
    double consigne_{0.0};      // 0 = Ligne centrée sous la Maker Line
    double erreur_prec_{0.0};   // Erreur au cycle précédent
    double erreur_i_{0.0};      // Accumulation intégrale

    /* GESTION DU TEMPS */
    rclcpp::Time last_time_;   // Horodatage du dernier calcul

    /* VITESSE & PUBLISHER/SUBSCRIBER */
    double base_speed_{0.2};    // Vitesse d'avancée
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_cmd_vel_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr sub_erreur_ligne_;

    //FONCTIONS 
    /* FONCTION DE CALCUL PID */
    double calcul_PID(double mesure) {
        // 1. Calcul précis du deltaT (en secondes)
        rclcpp::Time current_time = this->now();
        double deltaT = (current_time - last_time_).seconds();

        // Protection contre la division par zéro ou un temps invalide
        if (deltaT <= 0.0) {
            return 0.0;
        }

        //Calcul de l'erreur actuelle
        double erreur = consigne_ - mesure;

        //Terme Proportionnel (P)
        double erreur_p = erreur;

        //Terme Intégral (I) avec sécurité Anti-Windup
        erreur_i_ += erreur * deltaT;
        erreur_i_ = std::clamp(erreur_i_, -50.0, 50.0);

        //Terme Dérivé (D)
        double erreur_d = (erreur - erreur_prec_) / deltaT;

        //Commande globale (u)
        double u = (erreur_p * kp_) + (ki_ * erreur_i_) + (kd_ * erreur_d);

        // Mémorisation pour le prochain cycle
        erreur_prec_ = erreur;                                    
        last_time_ = current_time;

        return u;
    }

    /* CALLBACK DE RÉCEPTION */
    void error_correction(const std_msgs::msg::Float32::SharedPtr msg) {
        double mesure = msg->data;

        // Calcul de la commande PID à partir de la mesure
        double command = calcul_PID(mesure);

        //Création d'un message de position 3D
        auto cmd_msg = geometry_msgs::msg::Twist();
        cmd_msg.linear.x = base_speed_;   // Vitesse d'avancement
        cmd_msg.angular.z = command;     // correction PID

        pub_cmd_vel_->publish(cmd_msg);   // Envoi du message

        RCLCPP_INFO(this->get_logger(), "Mesure reçue : %.2f | Commande calculée : %.2f", mesure, command);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Moteur_pid_noeud>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}