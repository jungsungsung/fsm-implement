#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>

extern "C" {
#include "controller.h"
#include "sensor.h"
}

// ===== 가짜 하드웨어 인터페이스 (actuator.c 대신 링크됨) =====
static std::vector<std::string> g_log;

extern "C" void MovingMotorInterface(MotorCommand c, MoveState s) {
    g_log.push_back(std::string(c == MOTOR_FORWARD ? "FWD" : "BWD")
        + (s == ENABLE ? "_ENABLE" : "_DISABLE"));
}
extern "C" void TurningMotorInterface(MotorCommand c) {
    g_log.push_back(c == MOTOR_TURN_LEFT ? "TURN_LEFT" : "TURN_RIGHT");
}
extern "C" void CleanerInterface(CleanerState s) {
    g_log.push_back(s == CLEANER_ON ? "CLEANER_ON"
        : s == CLEANER_OFF ? "CLEANER_OFF" : "POWER_UP");
}

// ===== 도우미 =====
class ControllerTest : public ::testing::Test {
protected:
    void SetUp() override { g_log.clear(); ControllerInit(); g_log.clear(); }
};

static ObstacleInfo O(int f, int l, int r) {
    ObstacleInfo o; o.front = f; o.left = l; o.right = r; return o;
}
static void tick(int f, int l, int r, int d, int n = 1) {
    for (int i = 0; i < n; ++i) Controller(O(f, l, r), d);
}
static bool has(const char* s) {
    return std::find(g_log.begin(), g_log.end(), s) != g_log.end();
}
static int count(const char* s) {
    return (int)std::count(g_log.begin(), g_log.end(), s);
}

// ===== 테스트 =====
// UT-08, 12, 17, 19 (좌회전 쪽 대칭 케이스)는 우회전 쪽(07, 11, 16, 18)과 코드 경로가
// 같아 단위 테스트에서 제외하고 통합 테스트(testcases)로만 확인한다.

// UT-01  초기화
// 입력: 0000
// 판정: FWD_ENABLE + CLEANER_ON 만 출력
TEST_F(ControllerTest, InitEnablesForwardAndCleaner) {
    g_log.clear();
    ControllerInit();
    tick(0, 0, 0, 0);            // tick 0: 초기화 직후, 입력 없음

    ASSERT_EQ(g_log.size(), 2u);           // 딱 2개만
    EXPECT_EQ(g_log[0], "FWD_ENABLE");     // 첫 번째
    EXPECT_EQ(g_log[1], "CLEANER_ON");     // 두 번째
}

// UT-02  전진 유지 (입력 없음)
// 입력: 0000 0000 0000 0000 0000
// 판정: 초기화 이후 출력 없음
TEST_F(ControllerTest, ForwardNoInput) {
    tick(0, 0, 0, 0,5);          // tick 0~4: 입력 없음

    EXPECT_TRUE(g_log.empty()) << "아무 명령도 나오면 안됨";
}

// UT-03  먼지 -> 파워업
// 입력: 0000 0001
// 판정: tick 1 에 POWER_UP 만 (FWD_DISABLE 없어야 함)
TEST_F(ControllerTest, PowerUpOnly) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(0, 0, 0, 1);            // tick 1: 먼지 감지
    ASSERT_EQ(g_log.size(), 1u);           // 딱 1개만
    EXPECT_EQ(g_log[0], "POWER_UP");    
    EXPECT_FALSE(has("FWD_DISABLE"));     
}

// UT-04  먼지 + 전방 장애물 동시 [D && !F]
// 입력: 0000 1001
// 판정: POWER_UP 안 나옴 / TURN_RIGHT 나옴
TEST_F(ControllerTest, DustWithFrontTurnsInstead) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(1, 0, 0, 1);            // tick 1: 전방 장애물 + 먼지 동시
    EXPECT_FALSE(has("POWER_UP"));
    EXPECT_TRUE(has( "TURN_RIGHT"));
}

// UT-05  파워업 중 먼지 재감지
// 입력: 0000 0001 0001
// 판정: POWER_UP 이 한 번만 (카운터 리셋 금지)
TEST_F(ControllerTest, PowerUpNotRestartedByDust) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(0, 0, 0, 1);            // tick 1: 먼지 -> 파워업 시작
    tick(0, 0, 0, 1);            // tick 2: 파워업 중 먼지 재감지
    EXPECT_EQ(count("POWER_UP"), 1);
}

// UT-06  파워업 종료 -> 일반 청소 복귀
// 입력: 0000 0001 0000 0000 0000 0000 0000 0000
// 판정: POWER_UP(tick 1) 후 5틱째인 tick 6 에 CLEANER_ON 한 번
TEST_F(ControllerTest, PowerUpReturnsAfterFiveTicks) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(0, 0, 0, 1);            // tick 1: 먼지 -> 파워업 시작 (카운터 5)
    ASSERT_EQ(g_log.size(), 1u);
    EXPECT_EQ(g_log[0], "POWER_UP");
    g_log.clear();

    tick(0, 0, 0, 0,4);          // tick 2~5: 파워업 유지 (카운터 4 -> 1)
    EXPECT_TRUE(g_log.empty());
    tick(0, 0, 0, 0);            // tick 6: 파워업 종료
    EXPECT_TRUE(has("CLEANER_ON"));

    tick(0, 0, 0, 0);            // tick 7: 입력 마지막 줄
    EXPECT_EQ(count("CLEANER_ON"),1);
}

// UT-07  파워업 중 장애물 -> 우회전
// 입력: 0000 0001 1000
// 판정: tick 2 에 CLEANER_OFF, FWD_DISABLE, TURN_RIGHT
TEST_F(ControllerTest, PowerUpToTurnRight) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(0, 0, 0, 1);            // tick 1: 먼지 -> 파워업 시작
    ASSERT_EQ(g_log.size(), 1u);
    EXPECT_EQ(g_log[0], "POWER_UP");
    g_log.clear();

    tick(1, 0, 0, 0);            // tick 2: 파워업 중 전방 장애물, 오른쪽 열림
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("TURN_RIGHT"));
}

// UT-09  파워업 중 장애물 -> 후진
// 입력: 0000 0001 1110
// 판정: tick 2 에 CLEANER_OFF, FWD_DISABLE, BWD_ENABLE
TEST_F(ControllerTest, PowerUpToBackward) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(0, 0, 0, 1);            // tick 1: 먼지 -> 파워업 시작
    ASSERT_EQ(g_log.size(), 1u);
    EXPECT_EQ(g_log[0], "POWER_UP");
    g_log.clear();

    tick(1, 1, 1, 0);            // tick 2: 파워업 중 전방·좌·우 모두 막힘
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("BWD_ENABLE"));
}

// UT-10  파워업 종료 + 장애물 동시   (D-03)
// 입력: 0000 0001 0000 0000 0000 0000 1000
// 판정: tick 6 출력의 첫 줄이 CLEANER_OFF (CLEANER_ON 선행 금지)
TEST_F(ControllerTest, PowerUpEndWithObstacleNoSpuriousOn) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(0, 0, 0, 1);            // tick 1: 먼지 -> 파워업 시작 (카운터 5)
    tick(0, 0, 0, 0, 4);         // tick 2~5: 카운트다운 5 -> 1
    g_log.clear();
    tick(1, 0, 0, 0);            // tick 6: 파워업 종료 틱 + 전방 장애물
    ASSERT_FALSE(g_log.empty());
    EXPECT_EQ(g_log.front(), "CLEANER_OFF") << "CLEANER_ON 이 먼저 나오면 안 됨";
}

// UT-11  전방 장애물 + 오른쪽 열림 -> 우회전
// 입력: 0000 1000
// 판정: CLEANER_OFF, FWD_DISABLE, TURN_RIGHT
TEST_F(ControllerTest, ForwardToTurnRight) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(1, 0, 0, 0);            // tick 1: 전방 장애물, 오른쪽 열림
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("TURN_RIGHT"));
}

// UT-13  전방·좌·우 모두 막힘 -> 후진
// 입력: 0000 1110
// 판정: CLEANER_OFF, FWD_DISABLE, BWD_ENABLE
TEST_F(ControllerTest, ForwardToBackward) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(1, 1, 1, 0);            // tick 1: 전방·좌·우 모두 막힘
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("BWD_ENABLE"));
}

// UT-14  회전 중에는 센서를 무시   (D-01)
// 입력: 0000 1000 1000 1000 1000 0000 0000 0000
// 판정: TURN_RIGHT 총 1번 (수정 전 4번)
TEST_F(ControllerTest, TurningIgnoresSensorInput) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(1, 0, 0, 0, 4);         // tick 1: 회전 시작, tick 2~4: 회전 중에도 계속 장애물
    tick(0, 0, 0, 0, 3);         // tick 5~7: 입력 없음
    EXPECT_EQ(count("TURN_RIGHT"), 1) << "회전 중 장애물 입력으로 다시 회전하면 안 됨";
}

// UT-15  회전 중 먼지 무시
// 입력: 0000 1000 0001 0000
// 판정: POWER_UP 안 나옴
TEST_F(ControllerTest, TurningIgnoresDust) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(1, 0, 0, 0);            // tick 1: 전방 장애물 -> 회전 시작
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("TURN_RIGHT"));
    g_log.clear();
    tick(0, 0, 0, 1);            // tick 2: 회전 중 먼지
    tick(0, 0, 0, 0);            // tick 3: 회전 중, 입력 없음
    EXPECT_FALSE(has("POWER_UP"));
}

// UT-16  회전은 정확히 5틱 후 종료
// 입력: 0000 1000 0000 0000 0000 0000 0000 0000
// 판정: 회전 시작(tick 1) 후 5틱째인 tick 6 에 CLEANER_ON + FWD_ENABLE
TEST_F(ControllerTest, TurnEndsAfterFiveTicks) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(1, 0, 0, 0);            // tick 1: 전방 장애물 -> 회전 시작
    g_log.clear();
    tick(0, 0, 0, 0, 4);         // tick 2~5: 회전 중
    EXPECT_TRUE(g_log.empty()) << "tick 5 까지는 아직 회전 중이어야 함";
    tick(0, 0, 0, 0);            // tick 6: 회전 완료
    EXPECT_TRUE(has("FWD_ENABLE"));
    EXPECT_TRUE(has("CLEANER_ON"));
    tick(0, 0, 0, 0);            // tick 7: 입력 마지막 줄
}

// UT-18  후진 3틱 후 오른쪽 열림 -> 우회전
// 입력: 0000 1110 1110 1110 0000 0000
// 판정: tick 4 에 BWD_DISABLE, TURN_RIGHT
TEST_F(ControllerTest, BackwardToTurnRight) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    EXPECT_TRUE(g_log.empty());

    tick(1, 1, 1, 0, 3);         // tick 1: 사방 막힘 -> 후진 진입, tick 2~3: 최소 후진 시간
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("BWD_ENABLE"));
    g_log.clear();

    tick(0, 0, 0, 0);            // tick 4: 최소 후진 끝 + 좌우 열림 -> 우회전
    EXPECT_TRUE(has("BWD_DISABLE"));
    EXPECT_TRUE(has("TURN_RIGHT"));

    tick(0, 0, 0, 0);            // tick 5: 입력 마지막 줄
}

// UT-20  좌우 막히면 후진 유지   (회귀)
// 입력: 0000 1110 1110 1110 1110 1110 1110 1110
// 판정: tick 1 이후 출력 없음 (BWD_DISABLE 나오면 실패)
TEST_F(ControllerTest, BackwardHoldsWhenBothSidesBlocked) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(1, 1, 1, 0);            // tick 1: 사방 막힘 -> 후진 진입
    g_log.clear();
    tick(1, 1, 1, 0, 6);         // tick 2~7: 계속 사방 막힘
    EXPECT_TRUE(g_log.empty()) << "Enable 유지로 계속 후진해야 함 (BWD_DISABLE 나오면 실패)";
}

// UT-21  후진 중 먼지 무시   (D-02)
// 입력: 0000 1110 0001 0000 0000 0000
// 판정: POWER_UP 안 나옴
TEST_F(ControllerTest, BackwardIgnoresDust) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(1, 1, 1, 0);            // tick 1: 사방 막힘 -> 후진 진입
    g_log.clear();
    tick(0, 0, 0, 1);            // tick 2: 후진 중 먼지
    tick(0, 0, 0, 0, 3);         // tick 3~5: 입력 없음
    EXPECT_FALSE(has("POWER_UP"));
}

// UT-22  회전 완료 틱 + 전방 장애물 -> 전이 하나만 (D-04)
// 입력: 0000 1000 0000 0000 0000 0000 1000 1000
// 판정: tick 6 에 CLEANER_ON+FWD_ENABLE 만 (TURN_RIGHT 없음), tick 7 에 CLEANER_OFF+FWD_DISABLE+TURN_RIGHT
TEST_F(ControllerTest, TurnEndTickNoSecondTransition) {
    tick(0, 0, 0, 0);            // tick 0: 입력 없음
    tick(1, 0, 0, 0);            // tick 1: 전방 장애물 -> 회전 시작
    g_log.clear();

    tick(0, 0, 0, 0, 4);         // tick 2~5: 회전 중
    tick(1, 0, 0, 0);            // tick 6: 회전 완료 틱 + 전방 장애물
    EXPECT_TRUE(has("CLEANER_ON"));
    EXPECT_TRUE(has("FWD_ENABLE"));
    EXPECT_FALSE(has("TURN_RIGHT"));

    tick(1, 0, 0, 0);            // tick 7: 다시 전방 장애물 -> 이번에 회전
    EXPECT_TRUE(has("CLEANER_OFF"));
    EXPECT_TRUE(has("FWD_DISABLE"));
    EXPECT_TRUE(has("TURN_RIGHT"));
}
