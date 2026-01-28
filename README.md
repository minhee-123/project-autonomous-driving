# ROS2 기반 자율주행(ADAS) RC카 시스템
🚗**Original Repository**: [AutonomousVehicle](https://github.com/team-2niverse/AutonomousVehicle.git)

<br>

## 📝 프로젝트 개요
* **수행 기간:** 2025. 10. 30. ~ 2025. 11. 12. (현대오토에버, 현대엔지비, 한국전파진흥협회)
* **프로젝트 목표:** ROS2 환경에서 딥러닝 기반 차선 유지(LKAS) 및 ToF 센서 기반 스마트 크루즈 컨트롤(ACC) 기능을 갖춘 자율주행 RC카 시스템 개발
* **주요 내용:**
  * **LKAS(Lane Keeping Assist System)**: 딥러닝 기반 전방 이미지 분석 및 차량 선속도 입력을 통한 차선 추종(Lane Following) 기능
  * **ACC(Adaptive Cruise Control)**: ToF 센서를 활용한 전방 거리 측정 및 상태머신 기반의 적응형 정속 주행 제어
  * **장애물 회피**: 전방 장애물 인식 시 시퀀스 제어를 통한 회피 기동 수행
  * **표지판 인식 기반 주행**: 도로 표지판 색상 및 정보 인식을 통한 차선 변경 및 우회전 목적지 주행 로직 구현
<br>

---

## 👤 나의 역할 및 수행 업무
* **담당 역할:** 시스템 아키텍처 설계, CAN 인터페이스 및 제어 알고리즘 노드 개발
* **수행 업무:**
    * ROS2 Pub-Sub 통신 기반의 시스템 아키텍처 설계(5개의 핵심 패키지 구조 설계)
    * **CAN 인터페이스 노드**: ToF 센서 데이터 수신(0x200) 및 최종 PWM 제어 값 송신(0x100) 기능 구현
    * **Speed 패키지**: PI 제어 기반의 ACC 알고리즘 및 주행 모드별 속도 발행 노드 개발
    * **Motion 패키지**: 장애물 회피(DODGE), 차선 변경(SWITCH), 우회전(TURN) 시나리오 제어 로직 구현
    * Confluence를 활용한 V-Model 단계별 산출물 관리
<br>

---

## 🛠️ 사용 기술 및 환경

| 분류 | 상세 내용 |
| :--- | :--- |
| **Languages** | C, C++ |
| **MCU & SBC** | Infineon TC375, Raspberry Pi 4 |
| **Protocols & Frameworks** | ROS2-Humble(Pub-Sub Communication), CAN |
| **Tools** | Visual Studio Code, Docker, Git, Confluence |
<br>

---

## ⚙️ 시스템 아키텍처
**System Architecture (System Logic)**
<img width="1337" height="622" alt="image" src="https://github.com/user-attachments/assets/28573dfd-2442-4165-9d1d-bab2e766a4ed" />
<br>

**HW Architecture**<br>
* 상위 제어기인 RP4가 인식 및 판단
* 하위 제어기인 TC375가 제어 수행
<img width="632" height="462" alt="image" src="https://github.com/user-attachments/assets/2abe0ae1-2782-491b-bc1c-a0f96b38176b" />
<br>
<br>

**SW Architecture (SW Package)**
* ROS2 기반 전체 기능을 5개의 패키지로 모듈화하여 설계
* **Seonsor**: ACC에 사용하는 ToF 센서와 카메라 이미지 값을 처리하고 발행
* **Planner**: 현재 차량의 주행 모드를 관리(모드를 발행)
  * 모드에 따라 Speed 패키지와 Motion 패키지에서 특정 노드를 활성화
  * **CRUISE Mode**: 일반 주행 모드(ACC + Line Keeping)
  * **DODGE Mode**: 장애물 회피 모드(정속 주행 + Line Change)
  * **SWITCH Mode**: 차선 변경 모드(정속 주행 + Line Change)
  * **TURN Mode**: 우회전 모드(정속 주행 + Line Change)
* **Speed**: ACC 제어 알고리즘 및 정속 주행을 통해 계산한 속도(0.0 ~ 1.0)를 발행
* **Motion**: Line Keeping 혹은 Changing 알고리즘을 통해 계산한 조향(-1.0 ~ 1.0) 발행
* **Control**: 속도와 조향 값을 가져와 PWM으로 변환 및 MCU로 송신
<img width="865" height="513" alt="image" src="https://github.com/user-attachments/assets/4d974268-69ed-4bbd-9408-da2d36674b87" />


<br>
<br>

---

## 🎬 주요 기능 및 결과물
**데모 시나리오1**
* 자율주행(CRUISE Mode) → 장애물 회피(DODGE Mode) → 자율주행(CRUISE Mode)


https://github.com/user-attachments/assets/18d83e37-ae57-4b67-b6db-94784bd7c292


**데모 시나리오2**
* 자율주행(CRUISE Mode) → 장애물 회피(DODGE Mode) → 차선변경(SWITCH Mode) → 우회전(TURN Mode) → 자율주행(CRUISE Mode) 


https://github.com/user-attachments/assets/672401a8-06f7-4d87-94ed-b78adbe5ddbb


<br>

---

## 🔍 트러블슈팅 및 회고

#### 📌 Case #1. ROS2 CAN 인터페이스 환경 설정 및 노드 실행 오류
* **문제 상황:** ToF 센서 값 수신 노드를 개발한 후 실행했을 때 데이터 수신 및 노드 활성화가 정상적으로 이루어지지 않음
* **원인 분석:** 코드 자체의 오류보다는 Raspberry Pi 및 Docker 환경 내에서의 하드웨어 인터페이스(SPI, CAN0) 설정 누락 확인
* **해결 방법:**
  * `raspi-config`를 통해 SPI 인터페이스 활성화 및 부팅 설정 파일 수정
  * `ip link` 명령을 사용하여 `can0` 인터페이스를 500kbps 비트레이트로 활성화
  * Docker 컨테이너 내에서 `socketcan` 패키지 노드를 사전에 실행하여 통신 기반 확보
<br>
