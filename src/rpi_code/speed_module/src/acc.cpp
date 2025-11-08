// ACC 기능을 수행하는 노드 생성
// current mode - normal에서 동작

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"  // 모드 구독용
#include "std_msgs/msg/u_int32.hpp" // ToF 구독용 (단위: mm)
#include "std_msgs/msg/u_int8.hpp"  // PWM 발행용 (0-255)

class AccNode : public rclcpp::Node {
public:
    /* 생성자 */
    // node: acc_node
    AccNode() : Node("acc_node"), current_mode_("normal") {
        // normal 모드로 초기화

        /* publisher */
        // topic: /target_speed
        // type: UInt8 (pwm 0~255)
        pwm_pub_ = this->create_publisher<std_msgs::msg::UInt8>("/target_speed", 10);

        /* subscriber 1 */
        // topic: /tof_distance_mm
        // type: UInt32
        tof_sub_ = this->create_subscription<std_msgs::msg::UInt32>(
            "/tof_distance_mm", 10,
            std::bind(&AccNode::tof_callback, this, std::placeholders::_1)
        );

        /* subscriber 2 */
        // topic: /current_mode
        // type: string
        mode_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/current_mode", 10,
            std::bind(&AccNode::mode_callback, this, std::placeholders::_1)
        );

        RCLCPP_INFO(this->get_logger(), "ACC Node가 준비되었습니다.");
    }
private:
    /* callback function 1 */
    void mode_callback(const std_msgs::msg::String::SharedPtr msg){
        this->current_mode_ = msg->data;
    }

    /* callback function 2 */
    void tof_callback(const std_msgs::msg::UInt32::SharedPtr msg){
        
        // 현재 저장된 모드가 normal이 아닐 경우 skip (즉시 종료)
        if (current_mode_ != "normal"){
            return;
        }

        // acc 로직 수행
        unsigned int current_distance_mm = msg->data;
        uint8_t pwm_output = 0; // target_speed로 발행할 pwm 값

        const int target_distance_mm = 100; // 목표 유지 거리: 100mm
        int error = current_distance_mm - target_distance_mm; // 유지 거리 오차

        const float Kp = 0.2; // 비례 상수
        int default_speed = 50; // pwm 주행속도 기준점
        int weight_speed = (int)(Kp * error); // 속도 가중치

        if (error <= 10){ // 전방거리: 110m 이하
            pwm_output = 0;
        }
        else{ // 전방거리: 110mm 초과 (후진이 불가능)
            if (weight_speed >= 20) weight_speed = 20;
            pwm_output = default_speed + weight_speed;
        }

        // ****************************************************디버깅용 로그
        RCLCPP_INFO(this->get_logger(), "Dist: %umm, Mode: %s, PWM: %d", current_distance_mm, this->current_mode_.c_str(), pwm_output);
        // ****************************************************디버깅용 로그

        // 속도 발행 (0 ~ 255 범위의 pwm)
        auto speed_msg = std_msgs::msg::UInt8();
        speed_msg.data = pwm_output;
        pwm_pub_->publish(speed_msg);
    }

    /* 멤버 변수 선언 */
    // publisher
    rclcpp::Publisher<std_msgs::msg::UInt8>::SharedPtr pwm_pub_; // PWM 발행용
    // subscribers
    rclcpp::Subscription<std_msgs::msg::UInt32>::SharedPtr tof_sub_;  // ToF 구독용
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr mode_sub_; // 모드 구독용
    // variable
    std::string current_mode_; // 현재 모드를 저장하기 위한 변수
};

/* main */
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AccNode>());
    rclcpp::shutdown();
    return 0;
}