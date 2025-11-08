// 좌우 pwm 값을 can을 통해 tc375 mcu로 보냄
// 1차 프로젝트의 topic_to_frame.cpp 리팩토링

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp" // UInt8MultiArray 메시지 타입을 이용 (uint8_t 타입의 배열)
#include "can_msgs/msg/frame.hpp" // can 프레임 메시지

class SendPwmNode : public rclcpp::Node {
public:
	/* 생성자 */
    // node: send_pwm_node
	SendPwmNode() : Node("send_pwm_node") {
		
		/* Publisher */
		// topic: to_can_bus
        // type: can frame
		pub_ = this->create_publisher<can_msgs::msg::Frame>("to_can_bus", 10);
		
		/* Subscriber */
		// topic: pwm_array
		// type: UInt8MultiArray
		pwm_sub_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
				"pwm_array", 10,
				std::bind(&SendPwmNode::pwm_callback, this, std::placeholders::_1)
				); // pwm_array 토픽에 메시지가 도착하면 callback 함수 호출
		
		RCLCPP_INFO(this->get_logger(), "Send PWM Node (ID: 100)가 준비되었습니다.");
	}
private:
    /* callback function */
	void pwm_callback(const std_msgs::msg::UInt8MultiArray::SharedPtr msg) {
		can_msgs::msg::Frame can_msg; // 전송할 메시지 생성

		can_msg.id = 0x100; // pwm can id = 0x100
		can_msg.dlc = 4; // data length = 4 bytes

		if (msg->data.size() >= 4) {
			can_msg.data[0] = msg->data[0]; // left dir
			can_msg.data[1] = msg->data[1]; // left duty
			can_msg.data[2] = msg->data[2]; // right dir
			can_msg.data[3] = msg->data[3]; // right duty
		}
        else {
			RCLCPP_WARN(this->get_logger(), "받은 PWM 배열의 크기가 4보다 작습니다! (크기: %zu)", msg->data.size());
			return; // CAN 메시지를 발행하지 않고 콜백 종료
		}

		// 나머지 4bytes는 0으로 채워둠
		for (int i = 4; i < 8; i++) {
			can_msg.data[i] = 0;
		}

		// can message header
		can_msg.is_rtr = false;
		can_msg.is_extended = false;
		can_msg.is_error = false;
		can_msg.header.frame_id = "base_link"; // 프레임 ID
		can_msg.header.stamp = this->now();     // 현재 시간

		// ****************************************************디버깅용 로그
		RCLCPP_INFO(this->get_logger(), "PWM CAN 전송 (ID 100): L_Dir=%u, L_Duty=%u, R_Dir=%u, R_Duty=%u", can_msg.data[0], can_msg.data[1], can_msg.data[2], can_msg.data[3]);
		// ****************************************************디버깅용 로그

		pub_->publish(can_msg); // topic /to_can_bus로 발행
	}

	/* 멤버 변수 선언 */
	rclcpp::Publisher<can_msgs::msg::Frame>::SharedPtr pub_; // can publisher
	rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr pwm_sub_; // pwm subscriber
};

/* main */
int main(int argc, char* argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<SendPwmNode>()); // SendPwmNode 클래스로 생성하고 실행
	rclcpp::shutdown();
	return 0;
}